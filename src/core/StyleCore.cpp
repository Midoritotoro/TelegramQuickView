#include "StyleCore.h"

#include "../media/ffmpeg/FrameGenerator.h"
#include "../media/player/MediaPlayer.h"

#include <QMimeDataBase>

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

		enum Round {
			All,
			Top,
			Bottom
		};

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

		void RoundCorners(QPainter& painter, const QSize& widgetSize, int borderRadius, Round roundType) {
			QPainterPath path;
			path.setFillRule(Qt::WindingFill);

			switch (roundType) {
				case Round::Top:
					path.moveTo(borderRadius, 0);
					path.lineTo(widgetSize.width() - borderRadius, 0);

					path.arcTo(widgetSize.width() - 2 * borderRadius, 0, 2 * borderRadius, 2 * borderRadius, 90, -90);
					path.lineTo(widgetSize.width(), widgetSize.height());

					path.lineTo(0, widgetSize.height());
					path.lineTo(0, borderRadius);

					path.arcTo(0, 0, 2 * borderRadius, 2 * borderRadius, 180, -90);
					break;
				
				case Round::Bottom:
					path.moveTo(0, widgetSize.height() - borderRadius);
					path.lineTo(0, borderRadius);

					path.lineTo(widgetSize.width(), borderRadius);
					path.lineTo(widgetSize.width(), widgetSize.height() - borderRadius);

					path.arcTo(widgetSize.width() - 2 * borderRadius, widgetSize.height() - 2 * borderRadius, 2 * borderRadius, 2 * borderRadius, 0, -90);
					path.lineTo(borderRadius, widgetSize.height());

					path.arcTo(0, widgetSize.height() - 2 * borderRadius, 2 * borderRadius, 2 * borderRadius, 180, -90);
					break;
				
				case Round::All:
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

					break;
			}

			painter.setClipPath(path);
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
		return imageSize.width() != 0 
			? QSize(targetWidth, targetWidth * 
				imageSize.height() / imageSize.width())
			: QSize(style::maximumMessageWidth, 100);
	}

	QPixmap GenerateThumbnail(const QString& path, const QSize& targetSize) {
		auto size = targetSize;
		if (size.width() <= 0)
			size.setWidth(style::maximumMessageWidth);

		auto thumbnail = QPixmap();
		const auto key = path;

		if (QPixmapCache::find(key, &thumbnail))
			return thumbnail;

		auto file = QFile(path);
		if (!file.open(QIODevice::ReadOnly))
			return QPixmap();

		auto mediaData = file.readAll();
		if (mediaData.isNull())
			return QPixmap();

		auto thumbnailImage = QImage();

		switch (MediaPlayer::detectMediaType(path)) {
			case MediaPlayer::MediaType::Image:
				thumbnailImage.loadFromData(mediaData);
				break;

			case MediaPlayer::MediaType::Video:
				thumbnailImage = FFmpeg::FrameGenerator(mediaData)
					.renderNext(QSize(), Qt::IgnoreAspectRatio, false).image;
				break;

			case MediaPlayer::MediaType::Audio:
				return QPixmap();

			case MediaPlayer::MediaType::Unknown:
				return QPixmap();
		}

		if (thumbnailImage.isNull())
			return QPixmap();

		thumbnailImage = style::Prepare(std::move(thumbnailImage), 
			style::getMinimumSizeWithAspectRatio(thumbnailImage.size(), size.width()));

		thumbnailImage = std::move(thumbnailImage).scaled(
			thumbnailImage.width() * style::DevicePixelRatio(),
			thumbnailImage.height() * style::DevicePixelRatio(),
			Qt::IgnoreAspectRatio,
			Qt::SmoothTransformation);

		thumbnailImage = style::Opaque(std::move(thumbnailImage));

		thumbnail = QPixmap::fromImage(std::move(thumbnailImage), Qt::ColorOnly);
		thumbnail.setDevicePixelRatio(style::DevicePixelRatio());
		
		if (QPixmapCache::cacheLimit() > 0)
			QPixmapCache::insert(key, thumbnail);

		return thumbnail;
	}

	void RoundCorners(QPainter& painter, const QSize& widgetSize, int borderRadius) {
		return RoundCorners(painter, widgetSize, borderRadius, Round::All);
	}

	void RoundTopCorners(QPainter& painter, const QSize& widgetSize, int borderRadius) {
		return RoundCorners(painter, widgetSize, borderRadius, Round::Top);
	}

	void RoundBottomCorners(QPainter& painter, const QSize& widgetSize, int borderRadius) {
		return RoundCorners(painter, widgetSize, borderRadius, Round::Bottom);
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