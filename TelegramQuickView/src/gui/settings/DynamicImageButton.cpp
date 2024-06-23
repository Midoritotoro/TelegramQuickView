#include "DynamicImageButton.h"

DynamicImageButton::DynamicImageButton(const QImage& image, QWidget* parent, bool rounded):
	ImagePushButton(image, parent, rounded)
{
	setFixedSize(50, 50);
}
