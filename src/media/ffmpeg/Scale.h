extern "C" {
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
	#include <libavcodec/avcodec.h>
	#include <libavutil/pixfmt.h>
}

#include <QImage>

namespace FFmpeg {

	void AlignedImageBufferCleanupHandler(void* data);
	[[nodiscard]] bool IsAlignedImage(const QImage& image);

	void UnPremultiplyLine(uchar* dst, const uchar* src, int intsCount);
	void PremultiplyLine(uchar* dst, const uchar* src, int intsCount);

	[[nodiscard]] int GetCpuCount();

	[[nodiscard]] bool GoodStorageForFrame(const QImage& storage, QSize size);

	[[nodiscard]] QImage CreateFrameStorage(QSize size);
	[[nodiscard]] void UnPremultiply(QImage& dst, const QImage& src);

	void PremultiplyInplace(QImage& image);
} // namespace FFmpeg