#include "VolumeController.h"

#include <QCoreApplication>
#include <QDir>
#include <QPainter>
#include <QMouseEvent>


VolumeController::VolumeController(QWidget* parent) :
	QPushButton(parent)
	, _isSpeakerOn(false)
	, _isVolumeValueSmall(true)
{
	setAttribute(Qt::WA_NoSystemBackground);
	setCursor(Qt::PointingHandCursor);
}


void VolumeController::paintSpeakerOff(QPainter& painter) {
	QString currentPath = QCoreApplication::applicationDirPath();
	QDir cssDir(currentPath + "/../../assets/images");

	QString speakerImagePath = cssDir.absolutePath() + "/speaker_white_off.png";

	QPixmap pixmap(speakerImagePath);


	if (pixmap.size() != size())
		pixmap = pixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

	painter.drawPixmap(0, 0, pixmap);
}

void VolumeController::paintSpeakerOn(QPainter& painter) {
	QString speakerImagePath;

	QString currentPath = QCoreApplication::applicationDirPath();
	QDir cssDir(currentPath + "/../../assets/images");

	_isVolumeValueSmall
	? speakerImagePath = cssDir.absolutePath() + "/speaker_white_small_on.png"
	: speakerImagePath = cssDir.absolutePath() + "/speaker_white_on.png";

	QPixmap pixmap(speakerImagePath);

	if (pixmap.size() != size())
		pixmap = pixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

	painter.drawPixmap(0, 0, pixmap);
}

void VolumeController::paintEvent(QPaintEvent* event) {
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::NoBrush);

	_isSpeakerOn
	? paintSpeakerOn(painter)
	: paintSpeakerOff(painter);
}

bool VolumeController::isSpeakerOn() const noexcept {
	return _isSpeakerOn;
}

void VolumeController::setSpeakerEnabled(bool enabled) {
	_isSpeakerOn = enabled;
	repaint();
}

void VolumeController::setVolume(int volume) {
	volume > 50
	? _isVolumeValueSmall = false
	: _isVolumeValueSmall = true;
}
