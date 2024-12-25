#include "StyleCore.h"

#include "../../media/ffmpeg/video/FrameGenerator.h"
#include <QMimeDataBase>

#include <QPainterPath>
#include <QPainter>

#include <QFile>
#include <QDir>

#include <QCoreApplication>
#include "StyleFont.h"

#include "StyleWidgets.h"
#include "../images/ImagesBlur.h"

#include "../../core/CoreUtility.h"
#include <QMediaMetaData>


namespace style {

	namespace {
		int devicePixelRatioValue = 1;
		int scaleValue = kScaleDefault;

		QString _sliderStyle = "";
		QString _scrollAreaStyle = "";

		bool rightToLeft = false;

		inline constexpr auto kPreviewPrefix = "_p";

		struct Shifted {
			Shifted() = default;
			Shifted(uint32 value) : value(value) {
			}
			Shifted(uint64 value) : value(value) {
			}
			uint64 value = 0;
		};

		inline Shifted operator+(Shifted a, Shifted b) {
			return Shifted(a.value + b.value);
		}

		inline Shifted operator*(Shifted shifted, uint64 multiplier) {
			return Shifted(shifted.value * multiplier);
		}

		inline Shifted operator*(uint64 multiplier, Shifted shifted) {
			return Shifted(shifted.value * multiplier);
		}

		Shifted reshifted(Shifted components) {
			return (components.value >> 8) & 0x00FF00FF00FF00FFULL;
		}

		inline uint32 unshifted(Shifted components) {
			return static_cast<uint32>((components.value & 0x000000000000FF00ULL) >> 8)
				| static_cast<uint32>((components.value & 0x00000000FF000000ULL) >> 16)
				| static_cast<uint32>((components.value & 0x0000FF0000000000ULL) >> 24)
				| static_cast<uint32>((components.value & 0xFF00000000000000ULL) >> 32);
		}

		inline uint32 getAlpha(Shifted components) {
			return (components.value & 0x00FF000000000000ULL) >> 48;
		}

		Shifted shifted(QColor color) {
			auto alpha = static_cast<uint64>((color.alpha() & 0xFF) + 1);
			auto components = static_cast<uint64>(color.blue() & 0xFF)
				| (static_cast<uint64>(color.green() & 0xFF) << 16)
				| (static_cast<uint64>(color.red() & 0xFF) << 32)
				| (static_cast<uint64>(255) << 48);
			return reshifted(components * alpha);
		}

		void loadStyles() {
			const auto sliderStyle = ":/src/css/SliderStyle.css";
			const auto scrollAreaStyle = ":/src/css/ScrollAreaStyle.css";

			auto file = QFile(sliderStyle);
		
			if (file.open(QFile::ReadOnly))
				_sliderStyle = file.readAll();

			file.setFileName(scrollAreaStyle);

			if (file.open(QFile::ReadOnly))
				_scrollAreaStyle = file.readAll();
		}
	} // namespace

	bool RightToLeft() {
		return rightToLeft;
	}

	void SetRightToLeft(bool _rightToLeft) {
		rightToLeft = rightToLeft;
	}

	Qt::LayoutDirection LayoutDirection() {
		return RightToLeft() ? Qt::RightToLeft : Qt::LeftToRight;
	}

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

	int FontHeight(const QFont& font) {
		return QFontMetrics(font).height();
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

			auto ints = reinterpret_cast<uint32*>(image.bits());
			const auto bg = shifted(QColor(Qt::white));

			const auto width = image.width();
			const auto height = image.height();

			const auto addPerLine = (image.bytesPerLine() / sizeof(uint32)) - width;

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

	QSize getMinimumSizeWithAspectRatio(const QSize& imageSize, int targetWidth) {
		return imageSize.width() != 0 
			? QSize(targetWidth, targetWidth * 
				imageSize.height() / imageSize.width())
			: QSize(style::maximumMessageWidth, 100);
	}

	QPixmap PixmapFast(QImage&& image) {
		Expects(image.format() == QImage::Format_ARGB32_Premultiplied
			|| image.format() == QImage::Format_RGB32);

		return QPixmap::fromImage(std::move(image), Qt::NoFormatConversion);
	}

	bool PartiallyEqual(
		const QSize& first,
		const QSize& second,
		int maxDifference)
	{
		const auto widthDifference = qMax(first.width(), second.width())
			- qMin(first.width(), second.width());

		const auto heightDifference = qMax(first.height(), second.height())
			- qMin(first.height(), second.height());

		return (widthDifference + heightDifference) <= maxDifference;
	}

	QPixmap FindPreviewInCache(const QString& key) {
		auto temp = QPixmap();
		QPixmapCache::find(key, &temp);

		return temp;
	}

	QSize MediaResolution(
		const QString& path,
		MediaPlayer::MediaType type)
	{
		const auto mediaType = type == MediaPlayer::MediaType::Unknown
			? MediaPlayer::detectMediaType(path)
			: type;

		switch (mediaType) {
			case MediaPlayer::MediaType::Image:
				return QPixmap(path).size();

			case MediaPlayer::MediaType::Video: {
				auto file = QFile(path);
				if (file.open(QIODevice::ReadOnly))
					return FFmpeg::FrameGenerator(file.read(40000)).resolution();
			}

		}
		return QSize();
	}

	QPixmap MediaPreview(const QString& path) {
		if (const auto _preview = FindPreviewInCache(kPreviewPrefix + path); 
			_preview.isNull() == false)
			return _preview;

		auto file = QFile(path);
		if (file.open(QIODevice::ReadOnly) == false)
			return QPixmap();

		const auto mediaData = file.readAll();
		if (mediaData.isNull())
			return QPixmap();

		auto preview = QPixmap();

		switch (MediaPlayer::detectMediaType(path)) {
			case MediaPlayer::MediaType::Image:
				preview.loadFromData(mediaData);
				break;

			case MediaPlayer::MediaType::Video:
				preview = PixmapFast(std::move(FFmpeg::FrameGenerator(mediaData)
					.renderNext(QSize(), Qt::IgnoreAspectRatio, false).image));
				break;

			case MediaPlayer::MediaType::Audio:
				return QPixmap();

			case MediaPlayer::MediaType::Unknown:
				return QPixmap();
		}
		
		if (QPixmapCache::cacheLimit() > 0)
			QPixmapCache::insert(kPreviewPrefix + path, preview);

		return preview;
	}

	QPixmap GenerateThumbnail(
		const QString& path,
		const QSize& targetSize)
	{
	//	const auto ms = Time::now();
	//	const auto timer = gsl::finally([=] { qDebug() << "style::GenerateThumbnail: " << Time::now() - ms << " ms"; });

		auto size = targetSize;
		if (size.width() <= 0)
			size.setWidth(style::maximumMessageWidth);

		auto thumbnail = QPixmap();
		const auto key = path;

		if (QPixmapCache::find(key, &thumbnail) && PartiallyEqual(thumbnail.size(), targetSize, 1)) {
			auto mediaPreview = QPixmap();

			if (QPixmapCache::find(kPreviewPrefix + key, &mediaPreview))
				QPixmapCache::remove(kPreviewPrefix + key);

			return thumbnail;
		}

		auto thumbnailImage = MediaPreview(path).toImage();
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
		// thumbnailImage = images::Blur(std::move(thumbnailImage), true);

		thumbnail = QPixmap::fromImage(std::move(thumbnailImage), Qt::ColorOnly);
	
		if (QPixmapCache::cacheLimit() > 0)
			QPixmapCache::insert(key, thumbnail);

		return thumbnail;
	}

	void RoundCorners(
		QPainter& painter,
		const QSize& widgetSize,
		int borderRadius,
		CornersRoundMode roundType) 
	{
		auto path = QPainterPath();
		path.setFillRule(Qt::WindingFill);

		switch (roundType) {
			case CornersRoundMode::Top:
				path.moveTo(borderRadius, 0);
				path.lineTo(widgetSize.width() - borderRadius, 0);

				path.arcTo(widgetSize.width() - 2 * borderRadius, 0, 
					2 * borderRadius, 2 * borderRadius, 90, -90);
				path.lineTo(widgetSize.width(), widgetSize.height());

				path.lineTo(0, widgetSize.height());
				path.lineTo(0, borderRadius);

				path.arcTo(0, 0, 2 * borderRadius, 2 * borderRadius, 180, -90);

				break;

			case CornersRoundMode::Bottom:
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

			case CornersRoundMode::All:
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

	void RoundCorners(
		QPainter& painter,
		const QSize& widgetSize,
		int borderRadius)
	{
		return RoundCorners(painter, widgetSize, borderRadius, CornersRoundMode::All);
	}

	void RoundTopCorners(
		QPainter& painter,
		const QSize& widgetSize,
		int borderRadius) 
	{
		return RoundCorners(painter, widgetSize, borderRadius, CornersRoundMode::Top);
	}

	void RoundBottomCorners(
		QPainter& painter,
		const QSize& widgetSize,
		int borderRadius) 
	{
		return RoundCorners(painter, widgetSize, borderRadius, CornersRoundMode::Bottom);
	}

	bool IsRgbNull(QRgb rgb) {
		return qRed(rgb) == 0 && qGreen(rgb) == 0 && qBlue(rgb) == 0;
	}

	QImage Prepare(
		QImage image,
		const QSize& _outer)
	{
		qDebug() << "Style::prepare: ";
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

	void Start() {
		internal::RegisterFontFamily("Open Sans");
	}

	void Stop() {
		internal::DestroyFonts();
	}

	void RegisterStyles() {
		defaultTextStyle = new TextStyle{
		   ._font = style::font(13, 0, 0),
		   .lineHeight = 14,
		   .linkUnderLine = true,
		   .blockquote = {} };

		defaultFlatLabelStyle = new FlatLabel{
			.margin = margins{
				10, 10, 10, 10
			},
			.colorBg = QColor(24, 37, 51),
			.textStyle = defaultTextStyle,
			.textPalette = &defaultTextPalette,
			.maximumWidth = style::maximumMessageWidth,
			.maximumHeight = style::maximumTextHeight,
			.minimumWidth = style::minimumMessageWidth,
			.minimumHeight = style::minimumMessageHeight
		};
	}
} // namespace style