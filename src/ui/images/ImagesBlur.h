#pragma once 

#include <QImage>


namespace images {
	[[nodiscard]] QImage Blur(QImage&& image, bool ignoreAlpha = false);
}