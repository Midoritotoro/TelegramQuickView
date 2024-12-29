#include "Scale.h"

#include <cassert>
#include <cstdlib>
#include <malloc.h>

#include "Atomic.h"
#include "Picture.h"

#include "Fourcc.h"


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
        UReduce(&p_fmt->i_sar_num, &p_fmt->i_sar_den,
            i_sar_num, i_sar_den, 0);

    }
    void ViewpointInit(viewpoint_t* p_vp)
    {
        p_vp->yaw = p_vp->pitch = p_vp->roll = 0.0f;
        p_vp->fov = FIELD_OF_VIEW_DEGREES_DEFAULT;
    }

    void PictureDestroyFromFormat(picture_t* pic)
    {
        picture_buffer_t* res = (picture_buffer_t*)pic->p_sys;

        if (res != NULL)
            PictureDeallocate(res->fd, res->base, res->size);
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

     void VideoFormatInit(
         video_format_t* p_src,
         fourcc_t i_chroma)
     {
         memset(p_src, 0, sizeof(video_format_t));
         p_src->i_chroma = i_chroma;

         ViewpointInit(&p_src->pose);
     }


     void VideoFormatClean(video_format_t* p_src)
     {
         free(p_src->p_palette);
         memset(p_src, 0, sizeof(video_format_t));
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
}