#pragma once 

#include "../core/Types.h"
#include <QString>

#include <QSize>
#include <QPixmap>


namespace Media {
	enum class Type : uchar {
		Photo = 0x01,
		Video = 0x02,
		Audio = 0x04,
		Unknown = 0x08
	};

	[[nodiscard]] Type detectMediaType(const QString& path);
	
	[[nodiscard]] QPixmap FindPreviewInCache(const QString& key);
	[[nodiscard]] QSize MediaResolution(
		const QString& path,
		Type type = Type::Unknown);

	[[nodiscard]] QPixmap MediaPreview(const QString& path);
	[[nodiscard]] QPixmap GenerateThumbnail(
		const QString& path,
		const QSize& targetSize);
} // namespace Media