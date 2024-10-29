#include "StyleCore.h"

#include <QPainter>

namespace style {

namespace {

	int devicePixelRatioValue = 1;
	int scaleValue = kScaleDefault;

} // namespace

int devicePixelRatio() {
	return devicePixelRatioValue;
}

void setDevicePixelRatio(int ratio) {
	devicePixelRatioValue = std::clamp(ratio, 1, kScaleMax / kScaleMin);
}

int scale() {
	return scaleValue;
}

void setScale(int scale) {
	scaleValue = scale;
}

QImage prepare(
	QImage image,
	const QSize& _outer)
{
	const auto width = image.width();
	const auto height = image.height();

	if (width <= 0
		|| (width == image.width() && (height <= 0 || height == image.height()))) {
	}
	else if (height <= 0) {
		image = image.scaledToWidth(
			width,
			Qt::SmoothTransformation);
	}
	else {
		image = image.scaled(
			width,
			height,
			Qt::IgnoreAspectRatio,
			Qt::SmoothTransformation);
	}
	auto outer = _outer;
	if (!outer.isEmpty()) {
		const auto ratio = style::devicePixelRatio();
		outer *= ratio;
		if (outer != QSize(width, height)) {
			image.setDevicePixelRatio(ratio);
			auto result = QImage(outer, QImage::Format_ARGB32_Premultiplied);
			result.setDevicePixelRatio(ratio);
			
			QPainter p(&result);
			p.drawImage(
				(result.width() - image.width()) / (2 * ratio),
				(result.height() - image.height()) / (2 * ratio),
				image);
			
			image = std::move(result);
		}
	}

	image.setDevicePixelRatio(style::devicePixelRatio());
	return image;
}

} // namespace style