#include "ThumbnailGenerator.h"


namespace FFmpeg {
	QImage ThumbnailGenerator::generate(const QString& path) {
		const auto ms = Time::now();
		const auto timer = gsl::finally([=] { qDebug() << "ThumbnailGenerator::generate: " << Time::now() - ms << " ms"; });

		auto generator = std::make_unique<FrameGenerator>(path, false);
		return generator->renderNext(QSize(), Qt::IgnoreAspectRatio, false).image;
	}

	QSize ThumbnailGenerator::resolution(const QString& path) {
		const auto ms = Time::now();
		const auto timer = gsl::finally([=] { qDebug() << "ThumbnailGenerator::resolution: " << Time::now() - ms << " ms"; });

		auto generator = std::make_unique<FrameGenerator>(path, false, false);
		return generator->resolution();
	}
} // namespace FFmpeg