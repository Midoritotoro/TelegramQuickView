#pragma once 

#include <QString>
#include "FrameGenerator.h"


namespace FFmpeg {
	class ThumbnailGenerator {
	public:
		ThumbnailGenerator(const QString& path);

		[[nodiscard]] QImage generate();
		[[nodiscard]] QSize resolution() const;
	private:
		std::unique_ptr<FrameGenerator> generator = nullptr;
	};
} // namespace FFmpeg