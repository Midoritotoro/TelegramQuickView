#include "Types.h"
#include <QImage>


namespace FFmpeg {
	[[nodiscard]] int GetCpuCount();

    [[nodiscard]] bool AtomicRcDec(atomic_rc_t* rc);
    void PictureRelease(picture_t* picture);

    void DecoderDeviceRelease(decoder_device* device);
	[[nodiscard]] void* VideoContextGetPrivate(
		video_context* vctx,
		enum video_context_type type);

	void AncillaryRelease(struct ancillary* ancillary);
    void VideoContextRelease(video_context* vctx);

	[[nodiscard]] int FourccCmp(
		const void* key,
		const void* ent);

	[[nodiscard]] fourcc_t Lookup(
		fourcc_t fourcc,
		const char** dsc,
		const struct fourcc_mapping* mapv, size_t mapc,
		const struct fourcc_desc* dscv, size_t dscc);

	[[nodiscard]] fourcc_t LookupVideo(
        fourcc_t fourcc,
        const char** dsc);

	[[nodiscard]] fourcc_t LookupAudio(
        fourcc_t fourcc, 
        const char** dsc);
	[[nodiscard]] fourcc_t LookupSpu(
        fourcc_t fourcc,
        const char** dsc);

	[[nodiscard]] fourcc_t LookupCat(
		fourcc_t fourcc,
		const char** dsc,
		int cat);
	[[nodiscard]] fourcc_t FourccGetCodec(int cat, fourcc_t fourcc);

	void VideoFormatSetup(
		video_format_t* p_fmt, fourcc_t i_chroma,
		int i_width, int i_height,
		int i_visible_width, int i_visible_height,
		int i_sar_num, int i_sar_den);

	void PictureDestroyContext(picture_t* p_picture);
	void AncillaryArrayClear(struct ancillary*** array);

	void ViewpointInit(viewpoint_t* p_vp);
	void VideoFormatInit(
		video_format_t* p_src,
		fourcc_t i_chroma);

	[[nodiscard]] picture_t* PictureNew(
		fourcc_t i_chroma, int i_width, 
		int i_height, int i_sar_num,
		int i_sar_den);

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