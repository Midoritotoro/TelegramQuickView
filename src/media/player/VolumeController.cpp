#include "VolumeController.h"

#include <QCoreApplication>
#include <QDir>
#include <QPainter>


VolumeController::VolumeController(QWidget* parent) :
    ClickableLabel(parent)
{
	setAttribute(Qt::WA_NoSystemBackground);
	setCursor(Qt::PointingHandCursor);
}

void VolumeController::paintEvent(QPaintEvent* event) {
	QString currentPath = QCoreApplication::applicationDirPath();
	QDir cssDir(currentPath + "/../../assets/images");

	QString speakerImagePath = cssDir.absolutePath() + "/speaker_white.png";

	QPixmap pixmap(speakerImagePath);

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	if (pixmap.size() != size())
		pixmap = pixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::NoBrush);
	painter.drawPixmap(0, 0, pixmap);
}