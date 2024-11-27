#include "StyleCore.h"

#include <QPainterPath>
#include <QPainter>

#include <QFile>
#include <QDir>

#include <QCoreApplication>

namespace style {

	namespace {

		int devicePixelRatioValue = 1;
		int scaleValue = kScaleDefault;

		QString _sliderStyle = "";
		QString _scrollAreaStyle = "";

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

		void loadStyles() {
			const auto currentPath = QCoreApplication::applicationDirPath();
			const auto cssDir = QDir(currentPath + "/../../src/css");

			const auto sliderStyle = cssDir.absolutePath() + "/SliderStyle.css";
			const auto scrollAreaStyle = cssDir.absolutePath() + "/ScrollAreaStyle.css";

			auto file = QFile(sliderStyle);
		
			if (file.open(QFile::ReadOnly))
				_sliderStyle = file.readAll();

			file.setFileName(scrollAreaStyle);

			if (file.open(QFile::ReadOnly))
				_scrollAreaStyle = file.readAll();
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

	QSize TextSize(const QString& text, const QFontMetrics& metrics) {
		return metrics.size(0, text);
	}

	QSize TextSize(const QString& text, const QFont& font) {
		return text.isEmpty() ? QSize{} : TextSize(text, QFontMetrics(font));
	}

	QString SliderStyle() {
		if (_sliderStyle.isEmpty())
			loadStyles();
		return _sliderStyle;
	}

	QString ScrollAreaStyle() {
		if (_scrollAreaStyle.isEmpty())
			loadStyles();
		return _scrollAreaStyle;
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

	QSize getMinimumSizeWithAspectRatio(const QSize& imageSize, const int targetWidth) {
		return QSize(targetWidth, targetWidth * imageSize.height() / imageSize.width());
	}

	void drawRoundedCorners(QPainter& painter, const QSize& widgetSize, int borderRadius) {
		auto path = QPainterPath();

		path.moveTo(borderRadius, 0);

		path.lineTo(widgetSize.width() - borderRadius, 0);
		path.quadTo(widgetSize.width(), 0, widgetSize.width(), borderRadius);

		path.lineTo(widgetSize.width(), widgetSize.height() - borderRadius);
		path.quadTo(widgetSize.width(), widgetSize.height(), 
			widgetSize.width() - borderRadius, widgetSize.height());

		path.lineTo(borderRadius, widgetSize.height());
		path.quadTo(0, widgetSize.height(), 0,
			widgetSize.height() - borderRadius);

		path.lineTo(0, borderRadius);
		path.quadTo(0, 0, borderRadius, 0);

		painter.drawPath(path);
	}

	QImage Prepare(
		QImage image,
		const QSize& _outer)
	{
		const auto imageWidth = image.width();
		const auto imageHeight = image.height();

		if (_outer.width() <= 0 || (_outer.width() == imageWidth
			&& (_outer.height() <= 0 || _outer.height() == imageHeight)))
			;
		else if (imageHeight <= 0)
			image = image.scaledToWidth(
				_outer.width(),
				Qt::SmoothTransformation);
		else
			image = image.scaled(
				_outer.width(),
				_outer.height(),
				Qt::IgnoreAspectRatio,
				Qt::SmoothTransformation);

		auto outer = _outer;

		if (!outer.isEmpty()) {
			const auto ratio = style::DevicePixelRatio();
			outer *= ratio;
			if (outer != QSize(_outer.width(), _outer.height())) {
				image.setDevicePixelRatio(ratio);

				auto result = QImage(outer, QImage::Format_ARGB32_Premultiplied);
				result.setDevicePixelRatio(ratio);

				QPainter painter(&result);

				if (_outer.width() < outer.width() || _outer.height() < outer.height())
					painter.fillRect(
						QRect({}, result.size() / ratio),
						Qt::black);
				painter.drawImage(
					(result.width() - imageWidth) / (2 * ratio),
					(result.height() - imageWidth) / (2 * ratio),
					image);

				image = std::move(result);
			}
		}

		image.setDevicePixelRatio(style::DevicePixelRatio());
		return image;
	}

} // namespace style