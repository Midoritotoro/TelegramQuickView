#include "Fourcc.h"


namespace FFmpeg {

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


} // namespace FFmpeg