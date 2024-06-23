#pragma once

#include "../core/ImagePushButton.h"

class DynamicImageButton: public ImagePushButton
{
public:
	DynamicImageButton(const QImage& image, QWidget* parent = nullptr, bool rounded = false);
};
