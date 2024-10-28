#include "StyleCore.h"


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
	int w,
	int h,
	const QSize& _outer)
{
	if (w <= 0
		|| (w == image.width() && (h <= 0 || h == image.height()))) {
	}
	else if (h <= 0) {
		image = image.scaledToWidth(
			w,
			Qt::SmoothTransformation);
	}
	else {
		image = image.scaled(
			w,
			h,
			Qt::IgnoreAspectRatio,
			(Qt::SmoothTransformation));
	}
	auto outer = _outer;
	if (!outer.isEmpty()) {
		const auto ratio = style::devicePixelRatio;
		outer *= ratio;
		if (outer != QSize(w, h)) {
			image.setDevicePixelRatio(ratio);
			auto result = QImage(outer, QImage::Format_ARGB32_Premultiplied);
			result.setDevicePixelRatio(ratio);
			if (args.options & Images::Option::TransparentBackground) {
				result.fill(Qt::transparent);
			}
			{
				QPainter p(&result);
				if (!(args.options & Images::Option::TransparentBackground)) {
					if (w < outer.width() || h < outer.height()) {
						p.fillRect(
							QRect({}, result.size() / ratio),
							Qt::black);
					}
				}
				p.drawImage(
					(result.width() - image.width()) / (2 * ratio),
					(result.height() - image.height()) / (2 * ratio),
					image);
			}
			image = std::move(result);
		}
	}

	if (args.options
		& (Option::RoundCircle | Option::RoundLarge | Option::RoundSmall)) {
		image = Round(std::move(image), args.options);
		Assert(!image.isNull());
	}
	if (args.colored) {
		image = Colored(std::move(image), *args.colored);
	}
	image.setDevicePixelRatio(style::devicePixelRatio());
	return image;
}

} // namespace style