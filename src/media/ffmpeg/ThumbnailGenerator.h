#pragma once

#include "FrameGenerator.h"
#include <QImage>

class ThumbnailGenerator {
public:
	static [[nodiscard]] QImage Generate(const QString& path);
};