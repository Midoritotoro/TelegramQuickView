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

    struct filter_t;
    struct filter_owner_t
    {
        union
        {
            const struct filter_video_callbacks* video;
            const struct filter_audio_callbacks* audio;
            const struct filter_subpicture_callbacks* sub;
        };

        /* Input attachments
         * XXX use filter_GetInputAttachments */
        int (*pf_get_attachments)(filter_t*, input_attachment_t***, int*);

        void* sys;
    };

    struct filter_t
    {
        module_t* p_module;
        void* p_sys;

        /* Input format */
        es_format_t         fmt_in;
        video_context* vctx_in;  // video filter, set by owner

        /* Output format of filter */
        es_format_t         fmt_out;
        video_context* vctx_out; // video filter, handled by the filter
        bool                b_allow_fmt_out_change;

        /* Name of the "video filter" shortcut that is requested, can be NULL */
        const char* psz_name;
        /* Filter configuration */
        const config_chain_t* p_cfg;

        /* Implementation of filter API */
        const struct filter_operations* ops;

        /** Private structure for the owner of the filter */
        filter_owner_t      owner;
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