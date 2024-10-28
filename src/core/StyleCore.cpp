#include "StyleCore.h"

#include <QPainter>

namespace style {

namespace {

	int DevicePixelRatioValue = 1;
	int ScaleValue = kScaleDefault;

} // namespace

int devicePixelRatio() {
	return DevicePixelRatioValue;
}

void setDevicePixelRatio(int ratio) {
	DevicePixelRatioValue = std::clamp(ratio, 1, kScaleMax / kScaleMin);
}

QImage prepare(
	QImage image,
	int width,
	int height,
	const QSize& _outer)
{
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
			(Qt::SmoothTransformation));
	}
	auto outer = _outer;
	if (!outer.isEmpty()) {
		const auto ratio = style::devicePixelRatio();
		outer *= ratio;
		if (outer != QSize(width, height)) {
			image.setDevicePixelRatio(ratio);
			auto result = QImage(outer, QImage::Format_ARGB32_Premultiplied);
			result.setDevicePixelRatio(ratio);
			{
				QPainter p(&result);
				p.drawImage(
					(result.width() - image.width()) / (2 * ratio),
					(result.height() - image.height()) / (2 * ratio),
					image);
			}
			image = std::move(result);
		}
	}

	image.setDevicePixelRatio(style::devicePixelRatio());
	return image;
}

} // namespace style