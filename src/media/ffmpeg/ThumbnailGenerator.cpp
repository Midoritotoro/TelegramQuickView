#include "ThumbnailGenerator.h"

QImage ThumbnailGenerator::Generate(const QString& path) {
	auto file = QFile(path);
	if (!file.open(QIODevice::ReadOnly))
		return QImage();

	auto bytes = file.readAll();

	FFmpeg::FrameGenerator generator(bytes);
	return generator.renderNext(QSize(), Qt::IgnoreAspectRatio, false).image;
}