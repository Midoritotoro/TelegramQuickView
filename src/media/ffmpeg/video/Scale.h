#include "Types.h"
#include <QImage>


namespace FFmpeg {
	[[nodiscard]] int GetCpuCount();


    bool AtomicRcDec(atomic_rc_t* rc);
    void PictureRelease(picture_t* picture);

    void DecoderDeviceRelease(decoder_device* device);
	void* VideoContextGetPrivate(
		video_context* vctx,
		enum video_context_type type);

	void AncillaryRelease(struct ancillary* ancillary);
    void VideoContextRelease(video_context* vctx);

	static void PictureDestroyContext(picture_t* p_picture);
	void AncillaryArrayClear(struct ancillary*** array);

	void PictureDestroy(picture_t* picture);
	void Clean(filter_t* p_filter);

	[[nodiscard]] bool VideoFormatIsSameChroma(
		const video_format_t* f1,
		const video_format_t* f2);
	[[nodiscard]] bool VideoFormatIsSimilar(
		const video_format_t* f1,
		const video_format_t* f2);

	[[nodiscard]] int Init(filter_t* p_filter);
	[[nodiscard]] picture_t* FilterPicture(
		filter_t* p_filter,
		picture_t* p_pic);
} // namespace FFmpeg