#include "VolumeController.h"

#include <QCoreApplication>
#include <QDir>
#include <QPainter>


VolumeController::VolumeController(QWidget* parent) :
	QAbstractButton(parent)
	, _isSpeakerOn(true)
{
	setAttribute(Qt::WA_NoSystemBackground);
	setCursor(Qt::PointingHandCursor);
}


void VolumeController::paintSpeakerOff(QPainter& painter) {
	QString currentPath = QCoreApplication::applicationDirPath();
	QDir cssDir(currentPath + "/../../assets/images");

	QString speakerImagePath = cssDir.absolutePath() + "/speaker_white_off.png";

	QPixmap pixmap(speakerImagePath);

	painter.setRenderHint(QPainter::Antialiasing);

	if (pixmap.size() != size())
		pixmap = pixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::NoBrush);
	painter.drawPixmap(0, 0, pixmap);
}

void VolumeController::paintSpeakerOn(QPainter& painter) {
	QString currentPath = QCoreApplication::applicationDirPath();
	QDir cssDir(currentPath + "/../../assets/images");

	QString speakerImagePath = cssDir.absolutePath() + "/speaker_white_on.png";

	QPixmap pixmap(speakerImagePath);

	painter.setRenderHint(QPainter::Antialiasing);

	if (pixmap.size() != size())
		pixmap = pixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::NoBrush);
	painter.drawPixmap(0, 0, pixmap);
}

void VolumeController::paintEvent(QPaintEvent* event) {
	QPainter painter(this);

	if (isSpeakerOn()) {
		paintSpeakerOff(painter);
		_isSpeakerOn = false;
	}
	else {
		paintSpeakerOn(painter);
		_isSpeakerOn = true;
	}
}

bool VolumeController::isSpeakerOn() const noexcept {
	return _isSpeakerOn;
}
