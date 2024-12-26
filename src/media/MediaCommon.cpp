#include "MediaCommon.h"

#include "images/ImagesPrepare.h"
#include "ffmpeg/video/ThumbnailGenerator.h"

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

			case Type::Video:
				return FFmpeg::ThumbnailGenerator(path).resolution();
		}

		return QSize();
	}

	QPixmap MediaPreview(const QString& path) {
		if (const auto _preview = FindPreviewInCache(kPreviewPrefix + path);
			_preview.isNull() == false)
			return _preview;

		auto preview = QPixmap();

		switch (detectMediaType(path)) {
			case Type::Photo:
				preview = QPixmap(path);
				break;

			case Type::Video:
				preview = images::PixmapFast(
					std::move(
						FFmpeg::ThumbnailGenerator(path).generate()));
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

			if (QPixmapCache::find(kPreviewPrefix + path, &mediaPreview))
				QPixmapCache::remove(kPreviewPrefix + path);

			return thumbnail;
		}

		auto thumbnailImage = MediaPreview(path).toImage();
		if (thumbnailImage.isNull())
			return QPixmap();

		thumbnailImage = images::Prepare(std::move(thumbnailImage),
			core::utility::GetMinimumSizeWithAspectRatio(thumbnailImage.size(), targetSize.width()));

		thumbnailImage = images::Opaque(std::move(thumbnailImage));
		thumbnail = images::PixmapFast(std::move(thumbnailImage));

		if (QPixmapCache::cacheLimit() > 0)
			QPixmapCache::insert(path, thumbnail);

		return thumbnail;
	}
} // namespace Media