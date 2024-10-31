#include "StyleCore.h"

#include <QPainter>

namespace style {

namespace {

	int devicePixelRatioValue = 1;
	int scaleValue = kScaleDefault;

	struct Shifted {
		Shifted() = default;
		Shifted(quint32 value) : value(value) {
		}
		Shifted(quint64 value) : value(value) {
		}
		quint64 value = 0;
	};

	inline Shifted operator+(Shifted a, Shifted b) {
		return Shifted(a.value + b.value);
	}

	inline Shifted operator*(Shifted shifted, quint64 multiplier) {
		return Shifted(shifted.value * multiplier);
	}

	inline Shifted operator*(quint64 multiplier, Shifted shifted) {
		return Shifted(shifted.value * multiplier);
	}


	Shifted reshifted(Shifted components) {
		return (components.value >> 8) & 0x00FF00FF00FF00FFULL;
	}

	inline quint32 unshifted(Shifted components) {
		return static_cast<quint32>((components.value & 0x000000000000FF00ULL) >> 8)
			| static_cast<quint32>((components.value & 0x00000000FF000000ULL) >> 16)
			| static_cast<quint32>((components.value & 0x0000FF0000000000ULL) >> 24)
			| static_cast<quint32>((components.value & 0xFF00000000000000ULL) >> 32);
	}

	inline quint32 getAlpha(Shifted components) {
		return (components.value & 0x00FF000000000000ULL) >> 48;
	}

	Shifted shifted(QColor color) {
		// Make it premultiplied.
		auto alpha = static_cast<quint64>((color.alpha() & 0xFF) + 1);
		auto components = static_cast<quint64>(color.blue() & 0xFF)
			| (static_cast<quint64>(color.green() & 0xFF) << 16)
			| (static_cast<quint64>(color.red() & 0xFF) << 32)
			| (static_cast<quint64>(255) << 48);
		return reshifted(components * alpha);
	}

} // namespace

int DevicePixelRatio() {
	return devicePixelRatioValue;
}

void SetDevicePixelRatio(int ratio) {
	devicePixelRatioValue = std::clamp(ratio, 1, kScaleMax / kScaleMin);
}

int Scale() {
	return scaleValue;
}

void SetScale(int scale) {
	scaleValue = scale;
}

QImage Opaque(QImage&& image) {
	if (image.hasAlphaChannel()) {
		image = std::move(image).convertToFormat(
			QImage::Format_ARGB32_Premultiplied);

		auto ints = reinterpret_cast<quint32*>(image.bits());
		const auto bg = shifted(QColor(Qt::white));
		const auto width = image.width();
		const auto height = image.height();
		const auto addPerLine = (image.bytesPerLine() / sizeof(quint32)) - width;
		for (auto y = 0; y != height; ++y) {
			for (auto x = 0; x != width; ++x) {
				const auto components = shifted(*ints);
				*ints++ = unshifted(components * 256
					+ bg * (256 - getAlpha(components)));
			}
			ints += addPerLine;
		}
	}
	return std::move(image);
}

QImage Prepare(
	QImage image,
	const QSize& _outer)
{
	const auto width = image.width();
	const auto height = image.height();

	if (width <= 0|| (width == image.width() 
		&& (height <= 0 || height == image.height())))
		;
	else if (height <= 0)
		image = image.scaledToWidth(
			width,
			Qt::SmoothTransformation);
	else
		image = image.scaled(
			width,
			height,
			Qt::IgnoreAspectRatio,
			Qt::SmoothTransformation);

	auto outer = _outer;
	if (!outer.isEmpty()) {
		const auto ratio = style::DevicePixelRatio();
		outer *= ratio;
		if (outer != QSize(width, height)) {
			image.setDevicePixelRatio(ratio);
			auto result = QImage(outer, QImage::Format_ARGB32_Premultiplied);
			result.setDevicePixelRatio(ratio);
			
			QPainter p(&result);

			if (width < outer.width() || height < outer.height())
				p.fillRect(
					QRect({}, result.size() / ratio),
					Qt::black);
			p.drawImage(
				(result.width() - image.width()) / (2 * ratio),
				(result.height() - image.height()) / (2 * ratio),
				image);
			
			image = std::move(result);
		}
	}

	image.setDevicePixelRatio(style::DevicePixelRatio());
	return image;
}

} // namespace style