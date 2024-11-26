#include "VolumeController.h"

#include "../../core/StyleCore.h"

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

	QString currentPath = QCoreApplication::applicationDirPath();
	QDir cssDir(currentPath + "/../../assets/images");

	_speakerOffImagePath = cssDir.absolutePath() + "/speaker_white_off.png";
	_speakerOnImagePath = cssDir.absolutePath() + "/speaker_white_on.png";
	_speakerSmallOnImagePath = cssDir.absolutePath() + "/speaker_white_small_on.png";
}


void VolumeController::paintSpeakerOff(QPainter& painter) {
	auto image = QImage(_speakerOffImagePath);
	image = style::Prepare(image, size());

	image = std::move(image).scaled(
		image.width() * style::DevicePixelRatio(),
		image.height() * style::DevicePixelRatio(),
		Qt::IgnoreAspectRatio,
		Qt::SmoothTransformation);

	_currentPixmap = QPixmap::fromImage(std::move(image), Qt::ColorOnly);

	_currentPixmap.setDevicePixelRatio(style::DevicePixelRatio());
	painter.drawPixmap(0, 0, _currentPixmap);
}

void VolumeController::paintSpeakerOn(QPainter& painter) {
	auto image = QImage(_isVolumeValueSmall
		? _speakerSmallOnImagePath
		: _speakerOnImagePath);
	image = style::Prepare(image, size());

	image = std::move(image).scaled(
		image.width() * style::DevicePixelRatio(),
		image.height() * style::DevicePixelRatio(),
		Qt::IgnoreAspectRatio,
		Qt::SmoothTransformation);

	_currentPixmap = QPixmap::fromImage(std::move(image), Qt::ColorOnly);

	_currentPixmap.setDevicePixelRatio(style::DevicePixelRatio());
	painter.drawPixmap(0, 0, _currentPixmap);
}

void VolumeController::paintEvent(QPaintEvent* event) {
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

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
	update();
}

void VolumeController::setVolume(int volume) {
	volume > 50
	? _isVolumeValueSmall = false
	: _isVolumeValueSmall = true;
}
