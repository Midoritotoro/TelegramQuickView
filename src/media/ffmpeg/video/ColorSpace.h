#pragma once 

#include "Picture.h"

extern "C" {
    #include <libswscale/swscale.h>
}


namespace FFmpeg {
    struct filter_sys_t {
        SwsFilter* p_filter;
        int i_sws_flags;

        video_format_t fmt_in;
        video_format_t fmt_out;

        const struct chroma_description_t* desc_in;
        const struct chroma_description_t* desc_out;

        struct SwsContext* ctx;
        struct SwsContext* ctxA;

        struct picture_t* p_src_a;
        struct picture_t* p_dst_a;

        int i_extend_factor;

        struct picture_t* p_src_e;
        struct picture_t* p_dst_e;

        bool b_add_a;
        bool b_copy;

        bool b_swap_uvi;
        bool b_swap_uvo;
    };

    [[nodiscard]] int GetSwsColorspace(const video_format_t* format);
    void SetColorspace(filter_sys_t* p_sys);
} // namespace FFmpeg