#pragma once 

#include "Fourcc.h"
#include "VideoHelper.h"


namespace FFmpeg {
    struct video_palette_t {
        int i_entries;                                  /**< number of in-use palette entries */
        uint8_t palette[VIDEO_PALETTE_COLORS_MAX][4];   /**< 4-byte RGBA/YUVA palette */
    };

    struct viewpoint_t {
        float yaw;   /* yaw in degrees */
        float pitch; /* pitch in degrees */
        float roll;  /* roll in degrees */
        float fov;   /* field of view in degrees */
    };


    struct video_format_t
    {
        fourcc_t i_chroma;                               /**< picture chroma */

        unsigned int i_width;                                 /**< picture width */
        unsigned int i_height;                               /**< picture height */

        unsigned int i_x_offset;               /**< start offset of visible area */
        unsigned int i_y_offset;               /**< start offset of visible area */

        unsigned int i_visible_width;                 /**< width of visible area */
        unsigned int i_visible_height;               /**< height of visible area */

        unsigned int i_sar_num;                   /**< sample/pixel aspect ratio */
        unsigned int i_sar_den;

        unsigned int i_frame_rate;                     /**< frame rate numerator */
        unsigned int i_frame_rate_base;              /**< frame rate denominator */

        video_palette_t* p_palette;              /**< video palette from demuxer */
        video_orientation_t orientation;                /**< picture orientation */

        video_color_primaries_t primaries;                  /**< color primaries */

        video_transfer_func_t transfer;                   /**< transfer function */
        video_color_space_t space;                        /**< YCbCr color space */

        video_color_range_t color_range;            /**< 0-255 instead of 16-235 */
        video_chroma_location_t chroma_location;      /**< YCbCr chroma location */

        video_multiview_mode_t multiview_mode;        /** Multiview mode, 2D, 3D */
        bool b_multiview_right_eye_first;   /** Multiview left or right eye first*/

        video_projection_mode_t projection_mode;            /**< projection mode */
        viewpoint_t pose;
        struct {
            /* similar to SMPTE ST 2086 mastering display color volume */
            uint16_t primaries[3 * 2]; /* G,B,R / x,y */
            uint16_t white_point[2]; /* x,y */

            uint32_t max_luminance;
            uint32_t min_luminance;
        } mastering;
        struct {
            /* similar to CTA-861.3 content light level */
            uint16_t MaxCLL;  /* max content light level */
            uint16_t MaxFALL; /* max frame average light level */
        } lighting;
        struct {
            uint8_t version_major;
            uint8_t version_minor;

            unsigned profile : 7;

            unsigned level : 6;
            unsigned rpu_present : 1;

            unsigned el_present : 1;
            unsigned bl_present : 1;
        } dovi;
        uint32_t i_cubemap_padding; /**< padding in pixels of the cube map faces */
    };

	void DecoderDeviceRelease(decoder_device* device);
	[[nodiscard]] void* VideoContextGetPrivate(
		video_context* vctx,
		enum video_context_type type);

	void ViewpointInit(viewpoint_t* p_vp);
	void VideoFormatInit(
		video_format_t* p_src,
		fourcc_t i_chroma);

	void VideoFormatClean(video_format_t* p_src);

	[[nodiscard]] bool VideoFormatIsSameChroma(
		const video_format_t* f1,
		const video_format_t* f2);
	[[nodiscard]] bool VideoFormatIsSimilar(
		const video_format_t* f1,
		const video_format_t* f2);

	void VideoContextRelease(video_context* vctx);

	void VideoFormatSetup(
		video_format_t* p_fmt, fourcc_t i_chroma,
		int i_width, int i_height,
		int i_visible_width, int i_visible_height,
		int i_sar_num, int i_sar_den);
	void VideoFormatCopyCrop(
		video_format_t* p_dst,
		const video_format_t* p_src);

	int VideoFormatCopy(
		video_format_t* p_dst,
		const video_format_t* p_src);
} // namespace FFmpeg