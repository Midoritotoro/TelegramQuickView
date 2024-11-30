#include "FullScreenButton.h"

#include "../../core/StyleCore.h"

#include <QDir>
#include <QApplication>
#include <QPainter>


FullScreenButton::FullScreenButton(QWidget* parent) :
	QPushButton(parent)
	, _state (State::FullScreenFrom)
{
	QString currentPath = QCoreApplication::applicationDirPath();
	QDir assetsDir(currentPath + "/../../assets/images");

	_fullScreenToImagePath = assetsDir.absolutePath() + "/player_fullscreen.png";
	_fullScreenFromImagePath = assetsDir.absolutePath() + "/player_minimize.png";

	setAttribute(Qt::WA_NoSystemBackground);
	setCursor(Qt::PointingHandCursor);
}

[[nodiscard]] FullScreenButton::State FullScreenButton::state() const noexcept {
	return _state;
}

void FullScreenButton::paintEvent(QPaintEvent* event) {
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

	painter.setBrush(Qt::NoBrush);
	painter.setPen(Qt::NoPen);

	if (_currentPixmap.size() != size()) {
		auto image = QImage(_state == State::FullScreenTo
			? _fullScreenToImagePath
			: _fullScreenFromImagePath);
		image = style::Prepare(image, size());

		image = std::move(image).scaled(
			image.width() * style::DevicePixelRatio(),
			image.height() * style::DevicePixelRatio(),
			Qt::IgnoreAspectRatio,
			Qt::SmoothTransformation);

		_currentPixmap = QPixmap::fromImage(std::move(image), Qt::ColorOnly);
		_currentPixmap.setDevicePixelRatio(style::DevicePixelRatio());
	}

	painter.drawPixmap(0, 0, _currentPixmap);
}

void FullScreenButton::mousePressEvent(QMouseEvent* event) {
	QPushButton::mousePressEvent(event);

	if (event->button() == Qt::LeftButton)
		_state = _state == State::FullScreenTo
			? State::FullScreenFrom
			: State::FullScreenTo;
}