#include "Types.h"
#include <QImage>


namespace FFmpeg {
	[[nodiscard]] int GetCpuCount();

   

    void DecoderDeviceRelease(decoder_device* device);
	[[nodiscard]] void* VideoContextGetPrivate(
		video_context* vctx,
		enum video_context_type type);

	void AncillaryRelease(struct ancillary* ancillary);
    void VideoContextRelease(video_context* vctx);

	void VideoFormatSetup(
		video_format_t* p_fmt, fourcc_t i_chroma,
		int i_width, int i_height,
		int i_visible_width, int i_visible_height,
		int i_sar_num, int i_sar_den);

	void AncillaryArrayClear(struct ancillary*** array);

	void ViewpointInit(viewpoint_t* p_vp);
	void VideoFormatInit(
		video_format_t* p_src,
		fourcc_t i_chroma);

	void VideoFormatClean(video_format_t* p_src);


	void Clean(filter_t* p_filter);

	[[nodiscard]] bool VideoFormatIsSameChroma(
		const video_format_t* f1,
		const video_format_t* f2);
	[[nodiscard]] bool VideoFormatIsSimilar(
		const video_format_t* f1,
		const video_format_t* f2);

	[[nodiscard]] int Init(filter_t* p_filter);
} // namespace FFmpeg