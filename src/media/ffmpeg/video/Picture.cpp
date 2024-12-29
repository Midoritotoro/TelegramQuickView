#include "Picture.h"


namespace FFmpeg {
    picture_t* PictureNew(
        fourcc_t i_chroma, int i_width,
        int i_height, int i_sar_num,
        int i_sar_den)
    {
        video_format_t fmt;

        VideoFormatInit(&fmt, 0);
        VideoFormatSetup(&fmt, i_chroma, i_width, i_height,
            i_width, i_height, i_sar_num, i_sar_den);

        return PictureNewFromFormat(&fmt);
    }

    void PictureDestroy(picture_t* picture)
    {
        assert(AtomicRcDec(&picture->refs) == 0);

        PictureDestroyContext(picture);

        picture_priv_t* priv = container_of(picture, picture_priv_t, picture);
        assert(priv->gc.destroy != NULL);

        priv->gc.destroy(picture);

        AncillaryArrayClear(&priv->ancillaries);
        VideoFormatClean(&picture->format);
        free(priv);
    }

    void PictureRelease(picture_t* picture)
    {
        if (AtomicRcDec(&picture->refs))
            PictureDestroy(picture);
    }

    void PictureDestroyContext(picture_t* p_picture)
    {
        picture_context_t* ctx = p_picture->context;
        if (ctx != NULL)
        {
            video_context* vctx = ctx->vctx;
            ctx->destroy(ctx);
            if (vctx)
                VideoContextRelease(vctx);
            p_picture->context = NULL;
        }
    }


    void PictureDestroyFromFormat(picture_t* pic)
    {
        picture_buffer_t* res = (picture_buffer_t*)pic->p_sys;

        if (res != NULL)
            PictureDeallocate(res->fd, res->base, res->size);
    }

    void* PictureAllocate(int* fdp, size_t size)
    {
        assert((size % 64) == 0);
        *fdp = -1;
        return _aligned_malloc(64, size);
    }

    void PictureDestroyDummy(picture_t* p_picture) {
        (void)p_picture;
    }

    void PictureDeallocate(int fd, void* base, size_t size)
    {
        assert(fd == -1);
        aligned_free(base);
        assert((size % 64) == 0);
    }

    bool PictureInitPrivate(
        const video_format_t* p_fmt,
        picture_priv_t* priv,
        const picture_resource_t* p_resource)
    {
        picture_t* p_picture = &priv->picture;

        memset(p_picture, 0, sizeof(*p_picture));
        p_picture->date = VLC_TICK_INVALID;

        VideoFormatCopy(&p_picture->format, p_fmt);
        /* Make sure the real dimensions are a multiple of 16 */
        if (PictureSetup(p_picture, p_fmt))
        {
            VideoFormatClean(&p_picture->format);
            return false;
        }

        AtomicRcInit(&p_picture->refs);
        priv->gc.opaque = NULL;

        p_picture->p_sys = p_resource->p_sys;

        if (p_resource->pf_destroy != NULL)
            priv->gc.destroy = p_resource->pf_destroy;
        else
            priv->gc.destroy = PictureDestroyDummy;

        AncillaryArrayInit(&priv->ancillaries);

        return true;
    }


    picture_t* PictureNewFromFormat(const video_format_t* fmt)
    {
        static_assert(offsetof(struct picture_priv_buffer_t, priv) == 0,
            "misplaced picture_priv_t, destroy won't work");

        struct picture_priv_buffer_t* privbuf = (picture_priv_buffer_t*)malloc(sizeof(*privbuf));
        if (unlikely(privbuf == NULL))
            return NULL;

        picture_buffer_t* res = &privbuf->res;

        picture_resource_t pic_res = {
            .p_sys = res,
            .pf_destroy = PictureDestroyFromFormat,
        };

        picture_priv_t* priv = &privbuf->priv;
        if (!PictureInitPrivate(fmt, priv, &pic_res))
            goto error;

        picture_t* pic = &priv->picture;
        if (pic->i_planes == 0) {
            pic->p_sys = NULL; // not compatible with picture_DestroyFromFormat
            return pic;
        }

        /* Calculate how big the new image should be */
        assert(pic->i_planes <= PICTURE_PLANE_MAX);
        size_t plane_sizes[PICTURE_PLANE_MAX];
        size_t pic_size = 0;

        for (int i = 0; i < pic->i_planes; i++)
        {
            const plane_t* p = &pic->p[i];

            if (unlikely(ckd_mul(&plane_sizes[i], p->i_pitch, p->i_lines))
                || unlikely(ckd_add(&pic_size, pic_size, plane_sizes[i])))
                goto error;
        }

        if (unlikely(pic_size >= PICTURE_SW_SIZE_MAX))
            goto error;

        unsigned char* buf = picture_Allocate(&res->fd, pic_size);
        if (unlikely(buf == NULL))
            goto error;

        res->base = buf;
        res->size = pic_size;
        res->offset = 0;

        /* Fill the p_pixels field for each plane */
        for (int i = 0; i < pic->i_planes; i++)
        {
            pic->p[i].p_pixels = buf;
            buf += plane_sizes[i];
        }

        return pic;
    error:
        video_format_Clean(&priv->picture.format);
        free(privbuf);
        return NULL;
    }

    picture_t* FilterPicture(filter_t* p_filter, picture_t* p_pic)
    {
        filter_sys_t* p_sys = (filter_sys_t*)p_filter->p_sys;

        const video_format_t* p_fmti = &p_filter->fmt_in.video;
        const video_format_t* p_fmto = &p_filter->fmt_out.video;

        picture_t* p_pic_dst;

        /* Check if format properties changed */
        if (Init(p_filter))
        {
            PictureRelease(p_pic);
            return NULL;
        }

        /* Request output picture */
        p_pic_dst = filter_NewPicture(p_filter);
        if (!p_pic_dst)
        {
            picture_Release(p_pic);
            return NULL;
        }

        /* */
        picture_t* p_src = p_pic;
        picture_t* p_dst = p_pic_dst;
        if (p_sys->i_extend_factor != 1)
        {
            p_src = p_sys->p_src_e;
            p_dst = p_sys->p_dst_e;

            CopyPad(p_src, p_pic);
        }

        if (p_sys->b_copy && p_sys->b_swap_uvi == p_sys->b_swap_uvo)
            picture_CopyPixels(p_dst, p_src);
        else if (p_sys->b_copy)
            SwapUV(p_dst, p_src);
        else
        {
            /* Even if alpha is unused, swscale expects the pointer to be set */
            const int n_planes = !p_sys->ctxA && (p_src->i_planes == 4 ||
                p_dst->i_planes == 4) ? 4 : 3;
            Convert(p_filter, p_sys->ctx, p_dst, p_src, p_fmti->i_visible_height,
                n_planes, p_sys->b_swap_uvi, p_sys->b_swap_uvo);
        }
        if (p_sys->ctxA)
        {
            /* We extract the A plane to rescale it, and then we reinject it. */
            if (p_fmti->i_chroma == VLC_CODEC_RGBA || p_fmti->i_chroma == VLC_CODEC_BGRA)
                ExtractA(p_sys->p_src_a, p_src, OFFSET_A);
            else if (p_fmti->i_chroma == VLC_CODEC_ARGB || p_fmti->i_chroma == VLC_CODEC_ABGR)
                ExtractA(p_sys->p_src_a, p_src, 0);
            else
                plane_CopyPixels(p_sys->p_src_a->p, p_src->p + A_PLANE);

            Convert(p_filter, p_sys->ctxA, p_sys->p_dst_a, p_sys->p_src_a,
                p_fmti->i_visible_height, 1, false, false);
            if (p_fmto->i_chroma == VLC_CODEC_RGBA || p_fmto->i_chroma == VLC_CODEC_BGRA)
                InjectA(p_dst, p_sys->p_dst_a, OFFSET_A);
            else if (p_fmto->i_chroma == VLC_CODEC_ARGB || p_fmto->i_chroma == VLC_CODEC_ABGR)
                InjectA(p_dst, p_sys->p_dst_a, 0);
            else
                plane_CopyPixels(p_dst->p + A_PLANE, p_sys->p_dst_a->p);
        }
        else if (p_sys->b_add_a)
        {
            /* We inject a complete opaque alpha plane */
            if (p_fmto->i_chroma == VLC_CODEC_RGBA || p_fmto->i_chroma == VLC_CODEC_BGRA)
                FillA(&p_dst->p[0], OFFSET_A);
            else if (p_fmto->i_chroma == VLC_CODEC_ARGB || p_fmto->i_chroma == VLC_CODEC_ABGR)
                FillA(&p_dst->p[0], 0);
            else
                FillA(&p_dst->p[A_PLANE], 0);
        }

        if (p_sys->i_extend_factor != 1)
        {
            picture_CopyPixels(p_pic_dst, p_dst);
        }

        picture_CopyProperties(p_pic_dst, p_pic);
        picture_Release(p_pic);
        return p_pic_dst;
    }

} // namespace FFmpeg