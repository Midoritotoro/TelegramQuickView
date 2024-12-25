#pragma once 

#include <QString>
#include "FrameGenerator.h"


namespace FFmpeg {
	class ThumbnailGenerator {
	public:
		ThumbnailGenerator(const QString& path)

		[[nodiscard]] QImage generate();
	private:
		FrameGenerator generator;
	};
} // namespace FFmpeg