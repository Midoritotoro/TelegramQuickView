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

	const auto currentPath = QCoreApplication::applicationDirPath();
	const auto assetsDir = QDir(currentPath + "/../../assets/images");

	_speakerOffPixmap = QPixmap(cssDir.absolutePath() + "/speaker_white_off.png");
	_speakerOnPixmap = QPixmap(cssDir.absolutePath() + "/speaker_white_on.png");
	_speakerSmallOnPixmap = QPixmap(cssDir.absolutePath() + "/speaker_white_small_on.png");
}


void VolumeController::paintSpeakerOff(QPainter& painter) {
	_speakerOffPixmap = std::move(_speakerOffPixmap).scaled(
		image.width() * style::DevicePixelRatio(),
		image.height() * style::DevicePixelRatio(),
		Qt::IgnoreAspectRatio,
		Qt::SmoothTransformation);

	_speakerOffPixmap.setDevicePixelRatio(style::DevicePixelRatio());
	painter.drawPixmap(0, 0, _speakerOffPixmap);
}

void VolumeController::paintSpeakerOn(QPainter& painter) {
	auto& image = _isVolumeValueSmall
		? _speakerSmallOnPixmap
		: _speakerOnPixmap;

	image = std::move(image).scaled(
		image.width() * style::DevicePixelRatio(),
		image.height() * style::DevicePixelRatio(),
		Qt::IgnoreAspectRatio,
		Qt::SmoothTransformation);

	image.setDevicePixelRatio(style::DevicePixelRatio());
	painter.drawPixmap(0, 0, image);
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
