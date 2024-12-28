extern "C" {
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
	#include <libavcodec/avcodec.h>
	#include <libavutil/pixfmt.h>
}

#include <QImage>

namespace FFmpeg {
	[[nodiscard]] int GetCpuCount();
} // namespace FFmpeg