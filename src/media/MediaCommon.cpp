#include "MediaCommon.h"

#include "images/ImagesPrepare.h"
#include "ffmpeg/video/FrameGenerator.h"

#include <QMimeDataBase>
#include <QFile>

#include "../core/CoreUtility.h"
#include "../ui/style/StyleScale.h"


namespace Media {
	namespace {
		inline constexpr auto kPreviewPrefix = "_p";
	} // namespace 

	Type detectMediaType(const QString& filePath) {
		const auto mimeType = QMimeDatabase().mimeTypeForFile(filePath).name();

		if (mimeType.contains("video"))
			return Media::Type::Video;
		else if (mimeType.contains("image"))
			return Media::Type::Photo;
		else if (mimeType.contains("audio"))
			return Media::Type::Audio;

		return Media::Type::Unknown;
	}

	QPixmap FindPreviewInCache(const QString& key) {
		auto temp = QPixmap();
		QPixmapCache::find(key, &temp);

		return temp;
	}

	QSize MediaResolution(
		const QString& path,
		Type type)
	{
		const auto mediaType = type == Type::Unknown
			? detectMediaType(path)
			: type;

		switch (mediaType) {
		case Type::Photo:
			return QPixmap(path).size();

		case Type::Video: {
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

		switch (detectMediaType(path)) {
		case Type::Photo:
			preview.loadFromData(mediaData);
			break;

		case Type::Video:
			preview = images::PixmapFast(std::move(FFmpeg::FrameGenerator(mediaData)
				.renderNext(QSize(), Qt::IgnoreAspectRatio, false).image));
			break;

		case Type::Audio:
			return QPixmap();

		case Type::Unknown:
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
		if (targetSize.isNull())
			return QPixmap();

		auto thumbnail = QPixmap();

		if (QPixmapCache::find(path, &thumbnail) &&
			core::utility::PartiallyEqual(thumbnail.size(), targetSize, 1)) {
			auto mediaPreview = QPixmap();

			if (QPixmapCache::find(kPreviewPrefix + key, &mediaPreview))
				QPixmapCache::remove(kPreviewPrefix + key);

			return thumbnail;
		}

		auto thumbnailImage = MediaPreview(path).toImage();
		if (thumbnailImage.isNull())
			return QPixmap();

		thumbnailImage = images::Prepare(std::move(thumbnailImage),
			core::utility::getMinimumSizeWithAspectRatio(thumbnailImage.size(), targetSize.width()));

		thumbnailImage = std::move(thumbnailImage).scaled(
			thumbnailImage.width() * style::DevicePixelRatio(),
			thumbnailImage.height() * style::DevicePixelRatio(),
			Qt::IgnoreAspectRatio,
			Qt::SmoothTransformation);

		thumbnailImage = images::Opaque(std::move(thumbnailImage));
		thumbnail = images::PixmapFast(std::move(thumbnailImage));

		if (QPixmapCache::cacheLimit() > 0)
			QPixmapCache::insert(path, thumbnail);

		return thumbnail;
	}
} // namespace Media