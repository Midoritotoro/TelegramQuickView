#include "VolumeClickableLabel.h"


VolumeClickableLabel::VolumeClickableLabel(const QString& imagePath, QSlider* VolumeSlider, QWidget* parent) :
    ClickableLabel(parent), m_VolumeSlider(VolumeSlider)
{
	QPixmap pixmap(imagePath);
	setBackgroundRole(QPalette::Dark);
	setScaledContents(true);
	setPixmap(pixmap);
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setStyleSheet("background-color: transparent");
	setAttribute(Qt::WA_NoSystemBackground);
	setCursor(Qt::PointingHandCursor);
}

void VolumeClickableLabel::hideEvent(QHideEvent* event) {
    Q_UNUSED(event);
    m_VolumeSlider->hide();
}
