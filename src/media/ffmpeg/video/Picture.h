#pragma once 

#include "Types.h"


namespace FFmpeg {
	void PictureRelease(picture_t* picture);
	void PictureDestroyContext(picture_t* p_picture);

	void PictureDestroyFromFormat(picture_t* pic);
	[[nodiscard]] void* PictureAllocate(int* fdp, size_t size);

	void PictureDestroyDummy(picture_t* p_picture);

	void PictureDestroy(picture_t* picture);
	void PictureDeallocate(
		int fd,
		void* base,
		size_t size);

	[[nodiscard]] int PictureSetup(
		picture_t* p_picture,
		const video_format_t* fmt);

	[[nodiscard]] picture_t* PictureNewFromFormat(const video_format_t* fmt);

	[[nodiscard]] bool PictureInitPrivate(
		const video_format_t* p_fmt,
		picture_priv_t* priv,
		const picture_resource_t* p_resource);

	[[nodiscard]] picture_t* PictureNew(
		fourcc_t i_chroma, int i_width,
		int i_height, int i_sar_num,
		int i_sar_den);

	void PictureCopyPixels(
		picture_t* p_dst,
		const picture_t* p_src);
	void PlaneCopyPixels(plane_t* p_dst, const plane_t* p_src);
} // namespace FFmpeg