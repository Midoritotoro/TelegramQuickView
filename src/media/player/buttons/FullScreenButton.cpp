#include "FullScreenButton.h"

#include "../../../ui/style/StyleScale.h"
#include "../../../media/images/ImagesPrepare.h"

#include <QPainter>


FullScreenButton::FullScreenButton(QWidget* parent) :
	QPushButton(parent)
	, _state (State::FullScreenFrom)
{
	_fullScreenTo = QImage(":/assets/images/player_fullscreen.png");
	_fullScreenFrom = QImage(":/assets/images/player_minimize.png");

	setAttribute(Qt::WA_NoSystemBackground);
	setCursor(Qt::PointingHandCursor);
}

[[nodiscard]] FullScreenButton::State FullScreenButton::state() const noexcept {
	return _state;
}

void FullScreenButton::paintEvent(QPaintEvent* event) {
	auto painter = QPainter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

	painter.setBrush(Qt::NoBrush);
	painter.setPen(Qt::NoPen);
	
	auto& image = _state == State::FullScreenTo
		? _fullScreenTo
		: _fullScreenFrom;

	if (image.size() != size()) {
		image = images::Prepare(image, size());

		image = std::move(image).scaled(
			image.width() * style::DevicePixelRatio(),
			image.height() * style::DevicePixelRatio(),
			Qt::IgnoreAspectRatio,
			Qt::SmoothTransformation);

		image.setDevicePixelRatio(style::DevicePixelRatio());
	}

	painter.drawImage(0, 0, image);
}

void FullScreenButton::mousePressEvent(QMouseEvent* event) {
	QPushButton::mousePressEvent(event);

	if (event->button() == Qt::LeftButton)
		_state = _state == State::FullScreenTo
			? State::FullScreenFrom
			: State::FullScreenTo;
}