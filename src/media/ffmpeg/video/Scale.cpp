#include "Scale.h"

#include <cassert>
#include <cstdlib>
#include <malloc.h>

#include "Atomic.h"
#include "Picture.h"

#include "Fourcc.h"
#include "VideoFormat.h"

#include "Tools.h"
#include "ColorSpace.h"

#include "VideoFormat.h"
#include "Chroma.h"

#include "Object.h"


extern "C" {
    #include <libswscale/swscale.h>
    #include <libswscale/version.h>
}

#include <qDebug>

#include <Windows.h>

#ifdef min
#undef min
#endif // min

#ifdef max
#undef max
#endif // max

#define ALLOW_YUVP (false)

namespace FFmpeg {
    int GetCpuCount() {
#if defined Q_OS_WIN
        SYSTEM_INFO info;
        GetSystemInfo(&info);

        return info.dwNumberOfProcessors;
#endif // Q_OS_WIN
        return 1;
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

    void FixParameters(enum AVPixelFormat* pi_fmt, bool* pb_has_a, fourcc_t fmt)
    {
        switch (fmt)
        {
        case CODEC_YUV422A:
            *pi_fmt = AV_PIX_FMT_YUVA422P;
            *pb_has_a = true;
            break;
        case CODEC_YUV420A:
            *pi_fmt = AV_PIX_FMT_YUVA420P;
            *pb_has_a = true;
            break;
        case CODEC_YUVA:
            *pi_fmt = AV_PIX_FMT_YUV444P;
            *pb_has_a = true;
            break;
        case CODEC_RGBA:
            *pi_fmt = AV_PIX_FMT_RGBA;
            *pb_has_a = true;
            break;
        case CODEC_ARGB:
            *pi_fmt = AV_PIX_FMT_ARGB;
            *pb_has_a = true;
            break;
        case CODEC_BGRA:
            *pi_fmt = AV_PIX_FMT_BGRA;
            *pb_has_a = true;
            break;
        case CODEC_ABGR:
            *pi_fmt = AV_PIX_FMT_ABGR;
            *pb_has_a = true;
            break;
        default:
            break;
        }
    }

    int GetParameters(ScalerConfiguration* p_cfg,
        const video_format_t* p_fmti,
        const video_format_t* p_fmto,
        int i_sws_flags_default)
    {
        enum AVPixelFormat i_fmti;
        enum AVPixelFormat i_fmto;

        bool b_has_ai = false;
        bool b_has_ao = false;
        int i_sws_flags = i_sws_flags_default;
        bool b_swap_uvi = false;
        bool b_swap_uvo = false;

        i_fmti = FindFfmpegChroma(p_fmti->i_chroma, &b_swap_uvi);
        i_fmto = FindFfmpegChroma(p_fmto->i_chroma, &b_swap_uvo);

        if (p_fmti->i_chroma == p_fmto->i_chroma)
        {
            if (p_fmti->i_chroma == CODEC_YUVP && ALLOW_YUVP)
            {
                i_fmti = i_fmto = AV_PIX_FMT_GRAY8;
                i_sws_flags = SWS_POINT;
            }
        }

        FixParameters(&i_fmti, &b_has_ai, p_fmti->i_chroma);
        FixParameters(&i_fmto, &b_has_ao, p_fmto->i_chroma);

#if !defined (__ANDROID__) && !defined(TARGET_OS_IPHONE)
        /* FIXME TODO removed when ffmpeg is fixed
         * Without SWS_ACCURATE_RND the quality is really bad for some conversions */
        switch (i_fmto)
        {
        case AV_PIX_FMT_ARGB:
        case AV_PIX_FMT_RGBA:
        case AV_PIX_FMT_ABGR:
            i_sws_flags |= SWS_ACCURATE_RND;
            break;
        default:
            break;
        }
#endif

        if (p_cfg)
        {
            p_cfg->i_fmti = i_fmti;
            p_cfg->i_fmto = i_fmto;
            p_cfg->b_has_a = b_has_ai && b_has_ao;
            p_cfg->b_add_a = (!b_has_ai) && b_has_ao;
            p_cfg->b_copy = i_fmti == i_fmto &&
                p_fmti->i_visible_width == p_fmto->i_visible_width &&
                p_fmti->i_visible_height == p_fmto->i_visible_height;
            p_cfg->b_swap_uvi = b_swap_uvi;
            p_cfg->b_swap_uvo = b_swap_uvo;
            p_cfg->i_sws_flags = i_sws_flags;
        }

        if (i_fmti < 0 || i_fmto < 0)
            return EGENERIC;

        return SUCCESS;
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

        p_sys->desc_in = FourccGetChromaDescription(p_fmti->i_chroma);
        p_sys->desc_out = FourccGetChromaDescription(p_fmto->i_chroma);
        if (p_sys->desc_in == NULL || p_sys->desc_out == NULL)
            return EGENERIC;

        if (p_sys->desc_in->plane_count == 0 || p_sys->desc_out->plane_count == 0)
            return EGENERIC;

        /* swscale does not like too small width */
        p_sys->i_extend_factor = 1;
        while (std::min(p_fmti->i_visible_width, p_fmto->i_visible_width) * p_sys->i_extend_factor < MINIMUM_WIDTH)
            p_sys->i_extend_factor++;

        const unsigned i_fmti_visible_width = p_fmti->i_visible_width * p_sys->i_extend_factor;
        const unsigned i_fmto_visible_width = p_fmto->i_visible_width * p_sys->i_extend_factor;
        for (int n = 0; n < (cfg.b_has_a ? 2 : 1); n++)
        {
            const AVPixelFormat i_fmti = n == 0 ? cfg.i_fmti : AV_PIX_FMT_GRAY8;
            const AVPixelFormat i_fmto = n == 0 ? cfg.i_fmto : AV_PIX_FMT_GRAY8;
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
            UReduce(&i_sar_num, &i_sar_den, i_sar_num, i_sar_den, 65536);
            i_sar_num *= p_fmto->i_visible_height;
            i_sar_den *= p_fmti->i_visible_height;
            UReduce(&i_sar_num, &i_sar_den, i_sar_num, i_sar_den, 65536);
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