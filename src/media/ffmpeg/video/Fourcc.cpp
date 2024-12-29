#include "Fourcc.h"

#include "Tools.h"
#include "FourccHelper.h"


namespace FFmpeg {
    static const chroma_description_t p_list_chroma_description[] = {
        { CODEC_I411,                  PLANAR_8(3, 4, 1) },
        { CODEC_I410,                  PLANAR_8(3, 4, 4) },
        { CODEC_I420,                  PLANAR_8(3, 2, 2) },
        { CODEC_YV12,                  PLANAR_8(3, 2, 2) },
        { CODEC_NV12,                  SEMIPLANAR(2, 2, 8) },
        { CODEC_NV21,                  SEMIPLANAR(2, 2, 8) },
        { CODEC_I422,                  PLANAR_8(3, 2, 1) },
        { CODEC_NV16,                  SEMIPLANAR(2, 1, 8) },
        { CODEC_NV61,                  SEMIPLANAR(2, 1, 8) },
        { CODEC_I440,                  PLANAR_8(3, 1, 2) },
        { CODEC_I444,                  PLANAR_8(3, 1, 1) },
        { CODEC_NV24,                  SEMIPLANAR(1, 1, 8) },
        { CODEC_NV42,                  SEMIPLANAR(1, 1, 8) },
        { CODEC_YUVA,                  PLANAR_8(4, 1, 1) },
        { CODEC_YUV420A,               PLANAR_8(4, 2, 2) },
        { CODEC_YUV422A,               PLANAR_8(4, 2, 1) },

        { CODEC_GBR_PLANAR,            PLANAR_8(3, 1, 1) },
        { CODEC_GBR_PLANAR_9L,         PLANAR_16(3, 1, 1, 9) },
        { CODEC_GBR_PLANAR_9B,         PLANAR_16(3, 1, 1, 9) },
        { CODEC_GBR_PLANAR_10L,        PLANAR_16(3, 1, 1, 10) },
        { CODEC_GBR_PLANAR_10B,        PLANAR_16(3, 1, 1, 10) },
        { CODEC_GBR_PLANAR_12L,        PLANAR_16(3, 1, 1, 12) },
        { CODEC_GBR_PLANAR_12B,        PLANAR_16(3, 1, 1, 12) },
        { CODEC_GBR_PLANAR_14L,        PLANAR_16(3, 1, 1, 14) },
        { CODEC_GBR_PLANAR_14B,        PLANAR_16(3, 1, 1, 14) },
        { CODEC_GBR_PLANAR_16L,        PLANAR_16(3, 1, 1, 16) },
        { CODEC_GBR_PLANAR_16B,        PLANAR_16(3, 1, 1, 16) },
        { CODEC_GBRA_PLANAR,           PLANAR_8(4, 1, 1) },
        { CODEC_GBRA_PLANAR_10L,       PLANAR_16(4, 1, 1, 10) },
        { CODEC_GBRA_PLANAR_10B,       PLANAR_16(4, 1, 1, 10) },
        { CODEC_GBRA_PLANAR_12L,       PLANAR_16(4, 1, 1, 12) },
        { CODEC_GBRA_PLANAR_12B,       PLANAR_16(4, 1, 1, 12) },
        { CODEC_GBRA_PLANAR_16L,       PLANAR_16(4, 1, 1, 16) },
        { CODEC_GBRA_PLANAR_16B,       PLANAR_16(4, 1, 1, 16) },

        { CODEC_I420_16L,              PLANAR_16(3, 2, 2, 16) },
        { CODEC_I420_16B,              PLANAR_16(3, 2, 2, 16) },
        { CODEC_I420_12L,              PLANAR_16(3, 2, 2, 12) },
        { CODEC_I420_12B,              PLANAR_16(3, 2, 2, 12) },
        { CODEC_I420_10L,              PLANAR_16(3, 2, 2, 10) },
        { CODEC_I420_10B,              PLANAR_16(3, 2, 2, 10) },
        { CODEC_I420_9L,               PLANAR_16(3, 2, 2,  9) },
        { CODEC_I420_9B,               PLANAR_16(3, 2, 2,  9) },
        { CODEC_I422_16L,              PLANAR_16(3, 2, 1, 16) },
        { CODEC_I422_16B,              PLANAR_16(3, 2, 1, 16) },
        { CODEC_I422_12L,              PLANAR_16(3, 2, 1, 12) },
        { CODEC_I422_12B,              PLANAR_16(3, 2, 1, 12) },
        { CODEC_I422_10L,              PLANAR_16(3, 2, 1, 10) },
        { CODEC_I422_10B,              PLANAR_16(3, 2, 1, 10) },
        { CODEC_I422_9L,               PLANAR_16(3, 2, 1,  9) },
        { CODEC_I422_9B,               PLANAR_16(3, 2, 1,  9) },
        { CODEC_I444_12L,              PLANAR_16(3, 1, 1, 12) },
        { CODEC_I444_12B,              PLANAR_16(3, 1, 1, 12) },
        { CODEC_I444_10L,              PLANAR_16(3, 1, 1, 10) },
        { CODEC_I444_10B,              PLANAR_16(3, 1, 1, 10) },
        { CODEC_I444_9L,               PLANAR_16(3, 1, 1,  9) },
        { CODEC_I444_9B,               PLANAR_16(3, 1, 1,  9) },
        { CODEC_I444_16L,              PLANAR_16(3, 1, 1, 16) },
        { CODEC_I444_16B,              PLANAR_16(3, 1, 1, 16) },
        { CODEC_YUVA_444_10L,          PLANAR_16(4, 1, 1, 10) },
        { CODEC_YUVA_444_10B,          PLANAR_16(4, 1, 1, 10) },
        { CODEC_YUVA_444_12L,          PLANAR_16(4, 1, 1, 12) },
        { CODEC_YUVA_444_12B,          PLANAR_16(4, 1, 1, 12) },
        { CODEC_P010,                  SEMIPLANAR(2, 2, 10) },
        { CODEC_P016,                  SEMIPLANAR(2, 2, 16) },

        { CODEC_YUYV,                  PACKED_FMT(2, 16) },
        { CODEC_YVYU,                  PACKED_FMT(2, 16) },
        { CODEC_UYVY,                  PACKED_FMT(2, 16) },
        { CODEC_VYUY,                  PACKED_FMT(2, 16) },
        { CODEC_YUV2,                  PACKED_FMT(2, 16) },
        { CODEC_RGB233,                PACKED_FMT(1, 8) },
        { CODEC_BGR233,                PACKED_FMT(1, 8) },
        { CODEC_RGB332,                PACKED_FMT(1, 8) },
        { CODEC_YUVP,                  PACKED_FMT(1, 8) },
        { CODEC_RGBP,                  PACKED_FMT(1, 8) },
        { CODEC_GREY,                  PACKED_FMT(1, 8) },
        { CODEC_GREY_10L,              PACKED_FMT(2, 10) },
        { CODEC_GREY_10B,              PACKED_FMT(2, 10) },
        { CODEC_GREY_12L,              PACKED_FMT(2, 12) },
        { CODEC_GREY_12B,              PACKED_FMT(2, 12) },
        { CODEC_GREY_16L,              PACKED_FMT(2, 16) },
        { CODEC_GREY_16B,              PACKED_FMT(2, 16) },

        { CODEC_RGB555BE,              PACKED_FMT(2, 15) },
        { CODEC_RGB555LE,              PACKED_FMT(2, 15) },
        { CODEC_BGR555LE,              PACKED_FMT(2, 15) },
        { CODEC_BGR555BE,              PACKED_FMT(2, 15) },
        { CODEC_RGB565LE,              PACKED_FMT(2, 16) },
        { CODEC_RGB565BE,              PACKED_FMT(2, 16) },
        { CODEC_BGR565LE,              PACKED_FMT(2, 16) },
        { CODEC_BGR565BE,              PACKED_FMT(2, 16) },
        { CODEC_RGB24,                 PACKED_FMT(3, 24) },
        { CODEC_BGR24,                 PACKED_FMT(3, 24) },
        { CODEC_RGBX,                  PACKED_FMT(4, 24) },
        { CODEC_XRGB,                  PACKED_FMT(4, 24) },
        { CODEC_BGRX,                  PACKED_FMT(4, 24) },
        { CODEC_XBGR,                  PACKED_FMT(4, 24) },
        { CODEC_RGBA,                  PACKED_FMT(4, 32) },
        { CODEC_ARGB,                  PACKED_FMT(4, 32) },
        { CODEC_BGRA,                  PACKED_FMT(4, 32) },
        { CODEC_ABGR,                  PACKED_FMT(4, 32) },
        { CODEC_RGBA10LE,              PACKED_FMT(4, 32) },
        { CODEC_RGBA64,                PACKED_FMT(8, 64) },
        { CODEC_VUYA,                  PACKED_FMT(4, 32) },
        { CODEC_Y210,                  PACKED_FMT(4, 32) },
        { CODEC_Y410,                  PACKED_FMT(4, 32) },

        { CODEC_Y211,                 1, { {{1,4}, {1,1}} }, 4, 32 },
        { CODEC_XYZ_12L,               PACKED_FMT(6, 48) },
        { CODEC_XYZ_12B,               PACKED_FMT(6, 48) },

        { CODEC_VDPAU_VIDEO,           FAKE_FMT() },
        { CODEC_VDPAU_OUTPUT,          FAKE_FMT() },
        { CODEC_ANDROID_OPAQUE,        FAKE_FMT() },
        { CODEC_MMAL_OPAQUE,           FAKE_FMT() },
        { CODEC_D3D9_OPAQUE,           FAKE_FMT() },
        { CODEC_D3D11_OPAQUE,          FAKE_FMT() },
        { CODEC_D3D9_OPAQUE_10B,       FAKE_FMT() },
        { CODEC_D3D11_OPAQUE_10B,      FAKE_FMT() },
        { CODEC_D3D11_OPAQUE_RGBA,     FAKE_FMT() },
        { CODEC_D3D11_OPAQUE_BGRA,     FAKE_FMT() },
        { CODEC_D3D11_OPAQUE_ALPHA,    FAKE_FMT() },

        { CODEC_NVDEC_OPAQUE_16B,      FAKE_FMT() },
        { CODEC_NVDEC_OPAQUE_10B,      FAKE_FMT() },
        { CODEC_NVDEC_OPAQUE,          FAKE_FMT() },

        { CODEC_NVDEC_OPAQUE_444,      FAKE_FMT() },
        { CODEC_NVDEC_OPAQUE_444_16B,  FAKE_FMT() },

        { CODEC_CVPX_NV12,             FAKE_FMT() },
        { CODEC_CVPX_UYVY,             FAKE_FMT() },
        { CODEC_CVPX_I420,             FAKE_FMT() },
        { CODEC_CVPX_BGRA,             FAKE_FMT() },

        { CODEC_CVPX_P010,             FAKE_FMT() },

        { CODEC_GST_MEM_OPAQUE,        FAKE_FMT() },

        { CODEC_VAAPI_420,             FAKE_FMT() },
        { CODEC_VAAPI_420_10BPP,       FAKE_FMT() },
};



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

        mapping = (fourcc_mapping*)bsearch(&fourcc, mapv, mapc, sizeof(*mapv), FourccCmp);
        if (mapping != NULL)
        {
            if (dsc != NULL)
            {
                desc = (fourcc_desc*)bsearch(&fourcc, dscv, dscc, sizeof(*dscv), FourccCmp);
                if (desc != NULL)
                {
                    *dsc = desc->desc;
                    return mapping->fourcc;
                }
            }
            fourcc = mapping->fourcc;
        }

        desc = (fourcc_desc*)bsearch(&fourcc, dscv, dscc, sizeof(*dscv), FourccCmp);
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

    fourcc_t FourccGetCodec(int cat, fourcc_t fourcc)
    {
        fourcc_t codec = LookupCat(fourcc, NULL, cat);
        return codec ? codec : fourcc;
    }


    int64_t GCD(
        int64_t a,
        int64_t b)
    {
        while (b)
        {
            int64_t c = a % b;
            a = b;
            b = c;
        }
        return a;
    }

    int LCM(int a, int b) {
        return a * b / GCD(a, b);
    }

    bool UReduce(
        unsigned* pi_dst_nom, unsigned* pi_dst_den,
        uint64_t i_nom, uint64_t i_den, uint64_t i_max)
    {
        bool b_exact = 1;
        uint64_t i_gcd;

        if (i_den == 0)
        {
            *pi_dst_nom = 0;
            *pi_dst_den = 1;
            return 1;
        }

        i_gcd = GCD(i_nom, i_den);
        i_nom /= i_gcd;
        i_den /= i_gcd;

        if (i_max == 0) i_max = INT64_C(0xFFFFFFFF);

        if (i_nom > i_max || i_den > i_max)
        {
            uint64_t i_a0_num = 0, i_a0_den = 1, i_a1_num = 1, i_a1_den = 0;
            b_exact = 0;

            for (; ; )
            {
                uint64_t i_x = i_nom / i_den;
                uint64_t i_a2n = i_x * i_a1_num + i_a0_num;
                uint64_t i_a2d = i_x * i_a1_den + i_a0_den;

                if (i_a2n > i_max || i_a2d > i_max) break;

                i_nom %= i_den;

                i_a0_num = i_a1_num; i_a0_den = i_a1_den;
                i_a1_num = i_a2n; i_a1_den = i_a2d;
                if (i_nom == 0) break;
                i_x = i_nom; i_nom = i_den; i_den = i_x;
            }
            i_nom = i_a1_num;
            i_den = i_a1_den;
        }

        *pi_dst_nom = i_nom;
        *pi_dst_den = i_den;

        return b_exact;
    }

    const chroma_description_t* FourccGetChromaDescription(fourcc_t i_fourcc)
    {
        for (size_t i = 0; i < ARRAY_SIZE(p_list_chroma_description); i++)
        {
            if (p_list_chroma_description[i].fcc == i_fourcc)
                return &p_list_chroma_description[i];
        }
        return NULL;
    }


} // namespace FFmpeg