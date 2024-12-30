#pragma once

#include "Fourcc.h"
#include "Picture.h"


#define MINIMUM_WIDTH               (32)

namespace FFmpeg {
    struct filter_sys_t {
        SwsFilter* p_filter;
        int i_sws_flags;

        video_format_t fmt_in;
        video_format_t fmt_out;

        const chroma_description_t* desc_in;
        const chroma_description_t* desc_out;

        struct SwsContext* ctx;
        struct SwsContext* ctxA;

        picture_t* p_src_a;
        picture_t* p_dst_a;

        int i_extend_factor;

        picture_t* p_src_e;
        picture_t* p_dst_e;

        bool b_add_a;
        bool b_copy;

        bool b_swap_uvi;
        bool b_swap_uvo;
    };

    struct ScalerConfiguration {
        enum AVPixelFormat i_fmti;
        enum AVPixelFormat i_fmto;

        bool b_has_a;
        bool b_add_a;

        int  i_sws_flags;
        bool b_copy;

        bool b_swap_uvi;
        bool b_swap_uvo;
    };


	[[nodiscard]] int GetCpuCount();

	void Clean(filter_t* p_filter);

	void FixParameters(
		enum AVPixelFormat* pi_fmt,
		bool* pb_has_a, fourcc_t fmt);
	[[nodiscard]] int GetParameters(ScalerConfiguration* p_cfg,
		const video_format_t* p_fmti,
		const video_format_t* p_fmto,
		int i_sws_flags_default);

	[[nodiscard]] int Init(filter_t* p_filter);
} // namespace FFmpeg