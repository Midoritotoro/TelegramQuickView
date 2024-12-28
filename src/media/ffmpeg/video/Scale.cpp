#include "Scale.h"

#include <cassert>

extern "C" {
    #include <libswscale/swscale.h>
    #include <libswscale/version.h>
}

#define container_of(ptr, type, member) \
    ((type *)(((char *)(ptr)) - offsetof(type, member)))

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))


#include <Windows.h>

namespace FFmpeg {
    int GetCpuCount() {
#if defined Q_OS_WIN
        SYSTEM_INFO info;
        GetSystemInfo(&info);

        return info.dwNumberOfProcessors;
#endif // Q_OS_WIN
        return 1;
    }

    bool AtomicRcDec(atomic_rc_t* rc)
    {
        uintptr_t prev = atomic_fetch_sub_explicit(&rc->refs, (uintptr_t)1,
            std::memory_order_acq_rel);
        assert(prev);

        return prev == 1;
    }

    void PictureRelease(picture_t* picture)
    {
        if (AtomicRcDec(&picture->refs))
            PictureDestroy(picture);
    }

    void DecoderDeviceRelease(decoder_device* device)
    {
        struct decoder_device_priv* priv =
            container_of(device, struct decoder_device_priv, device);
        if (AtomicRcDec(&priv->rc))
        {
            if (device->ops->close != NULL)
                device->ops->close(device);
            //  vlc_objres_clear(VLC_OBJECT(device));
            //  vlc_object_delete(device);
        }
    }


    int FourccCmp(
        const void* key,
        const void* ent) 
    {
        return memcmp(key, ent, 4);
    }

    fourcc_t Lookup(
        fourcc_t fourcc,
        const char** dsc,
        const struct fourcc_mapping* mapv, size_t mapc,
        const struct fourcc_desc* dscv, size_t dscc)
    {
        const struct fourcc_mapping* mapping;
        const struct fourcc_desc* desc;

        mapping = (fourcc_mapping*)bsearch(&fourcc, mapv, mapc, sizeof(*mapv), fourcc_cmp);
        if (mapping != NULL)
        {
            if (dsc != NULL)
            {
                desc = (fourcc_desc*)bsearch(&fourcc, dscv, dscc, sizeof(*dscv), fourcc_cmp);
                if (desc != NULL)
                {
                    *dsc = desc->desc;
                    return mapping->fourcc;
                }
            }
            fourcc = mapping->fourcc;
        }

        desc = (fourcc_desc*)bsearch(&fourcc, dscv, dscc, sizeof(*dscv), fourcc_cmp);
        if (desc == NULL)
            return 0; /* Unknown FourCC */
        if (dsc != NULL)
            *dsc = desc->desc;
        return fourcc; /* Known FourCC (has a description) */
    }

    fourcc_t LookupVideo(
        fourcc_t fourcc,
        const char** dsc)
    {
        return Lookup(fourcc, dsc, mapping_video, ARRAY_SIZE(mapping_video),
            desc_video, ARRAY_SIZE(desc_video));
    }

    fourcc_t LookupAudio(
        fourcc_t fourcc,
        const char** dsc)
    {
        return Lookup(fourcc, dsc, mapping_audio, ARRAY_SIZE(mapping_audio),
            desc_audio, ARRAY_SIZE(desc_audio));
    }

    fourcc_t LookupSpu(
        fourcc_t fourcc,
        const char** dsc)
    {
        return Lookup(fourcc, dsc, mapping_spu, ARRAY_SIZE(mapping_spu),
            desc_spu, ARRAY_SIZE(desc_spu));
    }

    fourcc_t LookupCat(
        fourcc_t fourcc,
        const char** dsc,
        int cat)
    {
        switch (cat)
        {
        case VIDEO_ES:
            return LookupVideo(fourcc, dsc);
        case AUDIO_ES:
            return LookupAudio(fourcc, dsc);
        case SPU_ES:
            return LookupSpu(fourcc, dsc);
        }

        fourcc_t ret = LookupVideo(fourcc, dsc);
        if (!ret)
            ret = LookupAudio(fourcc, dsc);
        if (!ret)
            ret = LookupSpu(fourcc, dsc);
        return ret;
    }


    void* VideoContextGetPrivate(
        video_context* vctx,
        enum video_context_type type)
    {
        if (vctx && vctx->private_type == type)
            return &vctx->_private;
        return NULL;
    }


    void VideoContextRelease(video_context* vctx)
    {
        if (AtomicRcDec(&vctx->rc))
        {
            if (vctx->device)
                DecoderDeviceRelease(vctx->device);
            if (vctx->ops && vctx->ops->destroy)
                vctx->ops->destroy(VideoContextGetPrivate(vctx, vctx->private_type));
            free(vctx);
        }
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

    void AncillaryRelease(struct ancillary* ancillary)
    {
        if (AtomicRcDec(&ancillary->rc))
        {
            if (ancillary->free_cb != NULL)
                ancillary->free_cb(ancillary->data);
            free(ancillary);
        }
    };

    void AncillaryArrayClear(struct ancillary*** array)
    {
        if (*array != NULL)
        {
            for (struct ancillary** ancillary = *array;
                *ancillary != NULL; ancillary++)
            {
                AncillaryRelease(*ancillary);
            }

            free(*array);
            *array = NULL;
        }
    }

    fourcc_t FourccGetCodec(int cat, fourcc_t fourcc)
    {
        fourcc_t codec = LookupCat(fourcc, NULL, cat);
        return codec ? codec : fourcc;
    }

    void VideoFormatSetup(
        video_format_t* p_fmt, fourcc_t i_chroma,
        int i_width, int i_height,
        int i_visible_width, int i_visible_height,
        int i_sar_num, int i_sar_den)
    {
        p_fmt->i_chroma = FourccGetCodec(VIDEO_ES, i_chroma);
        p_fmt->i_width = i_width;
        p_fmt->i_visible_width = i_visible_width;
        p_fmt->i_height = i_height;
        p_fmt->i_visible_height = i_visible_height;
        p_fmt->i_x_offset =
            p_fmt->i_y_offset = 0;
        p_fmt->orientation = ORIENT_NORMAL;
        vlc_ureduce(&p_fmt->i_sar_num, &p_fmt->i_sar_den,
            i_sar_num, i_sar_den, 0);

    }
    void ViewpointInit(viewpoint_t* p_vp)
    {
        p_vp->yaw = p_vp->pitch = p_vp->roll = 0.0f;
        p_vp->fov = FIELD_OF_VIEW_DEGREES_DEFAULT;
    }

     void VideoFormatInit(
         video_format_t* p_src,
         fourcc_t i_chroma)
     {
         memset(p_src, 0, sizeof(video_format_t));
         p_src->i_chroma = i_chroma;

         ViewpointInit(&p_src->pose);
     }

     picture_t* PictureNew(
        fourcc_t i_chroma, int i_width,
        int i_height, int i_sar_num,
        int i_sar_den)
     {
         video_format_t fmt;

         VideoFormatInit(&fmt, 0);
         VideoFormatSetup(&fmt, i_chroma, i_width, i_height,
             i_width, i_height, i_sar_num, i_sar_den);

         return picture_NewFromFormat(&fmt);
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

    void Clean(filter_t* p_filter)
    {
        filter_sys_t* p_sys = (filter_sys_t*)p_filter->p_sys;

        if (p_sys->p_src_e)
            PictureRelease(p_sys->p_src_e);
        if (p_sys->p_dst_e)
            PictureRelease(p_sys->p_dst_e);

        if (p_sys->p_src_a)
            PictureRelease(p_sys->p_src_a);
        if (p_sys->p_dst_a)
            PictureRelease(p_sys->p_dst_a);

        if (p_sys->ctxA)
            sws_freeContext(p_sys->ctxA);

        if (p_sys->ctx)
            sws_freeContext(p_sys->ctx);

        /* We have to set it to null has we call be called again :( */
        p_sys->ctx = NULL;
        p_sys->ctxA = NULL;
        p_sys->p_src_a = NULL;
        p_sys->p_dst_a = NULL;
        p_sys->p_src_e = NULL;
        p_sys->p_dst_e = NULL;
    }


    bool VideoFormatIsSameChroma(
        const video_format_t* f1,
        const video_format_t* f2)
    {
        if (f1->i_chroma != f2->i_chroma)
            return false;
        return true;
    }

    bool VideoFormatIsSimilar(const video_format_t* f1,
        const video_format_t* f2)
    {
        if (!VideoFormatIsSameChroma(f1, f2))
            return false;

        if (f1->i_width != f2->i_width || f1->i_height != f2->i_height ||
            f1->i_visible_width != f2->i_visible_width ||
            f1->i_visible_height != f2->i_visible_height ||
            f1->i_x_offset != f2->i_x_offset || f1->i_y_offset != f2->i_y_offset)
            return false;

        if ((int64_t)f1->i_sar_num * f2->i_sar_den !=
            (int64_t)f2->i_sar_num * f1->i_sar_den)
            return false;

        if (f1->orientation != f2->orientation)
            return false;

        if (f1->multiview_mode != f2->multiview_mode)
            return false;

        return true;
    }

    int Init(filter_t* p_filter)
    {
        filter_sys_t* p_sys = (filter_sys_t*)p_filter->p_sys;

        const video_format_t* p_fmti = &p_filter->fmt_in.video;
        video_format_t* p_fmto = &p_filter->fmt_out.video;

        if (p_fmti->orientation != p_fmto->orientation)
            return EGENERIC;

        if (VideoFormatIsSimilar(p_fmti, &p_sys->fmt_in) &&
            VideoFormatIsSimilar(p_fmto, &p_sys->fmt_out) &&
            p_sys->ctx)
        {
            return SUCCESS;
        }

        Clean(p_filter);

        /* Init with new parameters */
        ScalerConfiguration cfg;
        if (GetParameters(&cfg, p_fmti, p_fmto, p_sys->i_sws_flags))
        {
            qDebug() << QString("format not supported '%1s' %2x%3 -> '%4s' %5x%6").arg(
                (const char*)&p_fmti->i_chroma).arg(p_fmti->i_visible_width).arg(p_fmti->i_visible_height).arg(
                (const char*)&p_fmto->i_chroma).arg(p_fmto->i_visible_width).arg(p_fmto->i_visible_height);

            return EGENERIC;
        }
        if (p_fmti->i_visible_width == 0 || p_fmti->i_visible_height == 0 ||
            p_fmto->i_visible_width == 0 || p_fmto->i_visible_height == 0)
        {
            qDebug() << QString("invalid scaling: %1x%2 -> %3x%4").arg(
                p_fmti->i_visible_width, p_fmti->i_visible_height).arg(
                p_fmto->i_visible_width, p_fmto->i_visible_height);
            return EGENERIC;
        }

        p_sys->desc_in = vlc_fourcc_GetChromaDescription(p_fmti->i_chroma);
        p_sys->desc_out = vlc_fourcc_GetChromaDescription(p_fmto->i_chroma);
        if (p_sys->desc_in == NULL || p_sys->desc_out == NULL)
            return GENERIC;

        if (p_sys->desc_in->plane_count == 0 || p_sys->desc_out->plane_count == 0)
            return EGENERIC;

        /* swscale does not like too small width */
        p_sys->i_extend_factor = 1;
        while (__MIN(p_fmti->i_visible_width, p_fmto->i_visible_width) * p_sys->i_extend_factor < MINIMUM_WIDTH)
            p_sys->i_extend_factor++;

        const unsigned i_fmti_visible_width = p_fmti->i_visible_width * p_sys->i_extend_factor;
        const unsigned i_fmto_visible_width = p_fmto->i_visible_width * p_sys->i_extend_factor;
        for (int n = 0; n < (cfg.b_has_a ? 2 : 1); n++)
        {
            const int i_fmti = n == 0 ? cfg.i_fmti : AV_PIX_FMT_GRAY8;
            const int i_fmto = n == 0 ? cfg.i_fmto : AV_PIX_FMT_GRAY8;
            struct SwsContext* ctx;

            ctx = sws_getContext(i_fmti_visible_width, p_fmti->i_visible_height, i_fmti,
                i_fmto_visible_width, p_fmto->i_visible_height, i_fmto,
                cfg.i_sws_flags,
                p_sys->p_filter, NULL, 0);
            if (n == 0)
                p_sys->ctx = ctx;
            else
                p_sys->ctxA = ctx;
        }
        if (p_sys->ctxA)
        {
            p_sys->p_src_a = PictureNew(CODEC_GREY, i_fmti_visible_width, p_fmti->i_visible_height, 0, 1);
            p_sys->p_dst_a = PictureNew(CODEC_GREY, i_fmto_visible_width, p_fmto->i_visible_height, 0, 1);
        }
        if (p_sys->i_extend_factor != 1)
        {
            p_sys->p_src_e = PictureNew(p_fmti->i_chroma, i_fmti_visible_width, p_fmti->i_visible_height, 0, 1);
            p_sys->p_dst_e = PictureNew(p_fmto->i_chroma, i_fmto_visible_width, p_fmto->i_visible_height, 0, 1);

            if (p_sys->p_src_e)
                memset(p_sys->p_src_e->p[0].p_pixels, 0, p_sys->p_src_e->p[0].i_pitch * p_sys->p_src_e->p[0].i_lines);
            if (p_sys->p_dst_e)
                memset(p_sys->p_dst_e->p[0].p_pixels, 0, p_sys->p_dst_e->p[0].i_pitch * p_sys->p_dst_e->p[0].i_lines);
        }

        if (!p_sys->ctx ||
            (cfg.b_has_a && (!p_sys->ctxA || !p_sys->p_src_a || !p_sys->p_dst_a)) ||
            (p_sys->i_extend_factor != 1 && (!p_sys->p_src_e || !p_sys->p_dst_e)))
        {
            qDebug() << "could not init SwScaler and/or allocate memory";
            Clean(p_filter);
            return EGENERIC;
        }

        if (p_filter->b_allow_fmt_out_change)
        {
            /*
             * If the transformation is not homothetic we must modify the
             * aspect ratio of the output format in order to have the
             * output picture displayed correctly and not stretched
             * horizontally or vertically.
             * WARNING: this is a hack, ideally this should not be needed
             * and the vout should update its video format instead.
             */
            unsigned i_sar_num = p_fmti->i_sar_num * p_fmti->i_visible_width;
            unsigned i_sar_den = p_fmti->i_sar_den * p_fmto->i_visible_width;
            vlc_ureduce(&i_sar_num, &i_sar_den, i_sar_num, i_sar_den, 65536);
            i_sar_num *= p_fmto->i_visible_height;
            i_sar_den *= p_fmti->i_visible_height;
            vlc_ureduce(&i_sar_num, &i_sar_den, i_sar_num, i_sar_den, 65536);
            p_fmto->i_sar_num = i_sar_num;
            p_fmto->i_sar_den = i_sar_den;
        }

        p_sys->b_add_a = cfg.b_add_a;
        p_sys->b_copy = cfg.b_copy;
        p_sys->fmt_in = *p_fmti;
        p_sys->fmt_out = *p_fmto;
        p_sys->b_swap_uvi = cfg.b_swap_uvi;
        p_sys->b_swap_uvo = cfg.b_swap_uvo;

        SetColorspace(p_sys);

        return SUCCESS;
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
}