#pragma once 

#include <QString>
#include "FrameGenerator.h"


namespace FFmpeg {
	class ThumbnailGenerator {
	public:
		[[nodiscard]] static QImage generate(const QString& path);
		[[nodiscard]] static QSize resolution(const QString& path);
	private:
	};
} // namespace FFmpeg