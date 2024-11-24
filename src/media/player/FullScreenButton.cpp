#include "FullScreenButton.h"

#include <QDir>
#include <QApplication>
#include <QPainter>


FullScreenButton::FullScreenButton(QWidget* parent) :
	QPushButton(parent)
	, _state (State::FullScreenTo)
{
	QString currentPath = QCoreApplication::applicationDirPath();
	QDir assetsDir(currentPath + "/../../../assets/images");

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
	painter.setRenderHint(QPainter::Antialiasing);

	painter.setBrush(Qt::NoBrush);
	painter.setPen(Qt::NoPen);

	_currentPixmap.load(_state == State::FullScreenTo 
								? _fullScreenToImagePath 
								: _fullScreenFromImagePath);

	if (_currentPixmap.size() != size())
		_currentPixmap = _currentPixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

	painter.drawPixmap(0, 0, _currentPixmap);
}

void FullScreenButton::resizeEvent(QResizeEvent* event) {
	QPushButton::resizeEvent(event);
}

void FullScreenButton::mousePressEvent(QMouseEvent* event) {
	QPushButton::mousePressEvent(event);

	if (event->button() == Qt::LeftButton)
		_state = _state == State::FullScreenTo
						 ? State::FullScreenFrom
						 : State::FullScreenTo;
}