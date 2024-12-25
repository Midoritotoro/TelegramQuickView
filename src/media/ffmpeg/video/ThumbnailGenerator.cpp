#include "ThumbnailGenerator.h"


namespace FFmpeg {
	ThumbnailGenerator::ThumbnailGenerator(const QString& path) {
		generator = std::make_unique<FrameGenerator>(path);
	}

	QImage ThumbnailGenerator::generate() {
		return generator->renderNext(QSize(), Qt::IgnoreAspectRatio, false).image;
	}

	QSize ThumbnailGenerator::resolution() const {
		return generator->resolution();
	}
} // namespace FFmpeg