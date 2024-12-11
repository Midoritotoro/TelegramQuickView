#include "FullScreenButton.h"

#include "../../core/StyleCore.h"

#include <QDir>
#include <QApplication>
#include <QPainter>


FullScreenButton::FullScreenButton(QWidget* parent) :
	QPushButton(parent)
	, _state (State::FullScreenFrom)
{
	const auto currentPath = QCoreApplication::applicationDirPath();
	const auto assetsDir = QDir(currentPath + "/../../assets/images");

	_fullScreenToPixmap = QPixmap(assetsDir.absolutePath() + "/player_fullscreen.png");
	_fullScreenFromPixmap = QPixmap(assetsDir.absolutePath() + "/player_minimize.png");

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

	if (_currentPixmap.size() != size()) {
		auto& image = _state == State::FullScreenTo
			? _fullScreenToPixmap
			: _fullScreenFromPixmap;
		image = style::Prepare(image, size());

		image = std::move(image).scaled(
			image.width() * style::DevicePixelRatio(),
			image.height() * style::DevicePixelRatio(),
			Qt::IgnoreAspectRatio,
			Qt::SmoothTransformation);

		image.setDevicePixelRatio(style::DevicePixelRatio());
	}

	painter.drawPixmap(0, 0, image);
}

void FullScreenButton::mousePressEvent(QMouseEvent* event) {
	QPushButton::mousePressEvent(event);

	if (event->button() == Qt::LeftButton)
		_state = _state == State::FullScreenTo
			? State::FullScreenFrom
			: State::FullScreenTo;
}