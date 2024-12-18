#include "AbstractLayoutItem.h"

AbstractLayoutItem::AbstractLayoutItem() 
{}

AbstractLayoutItem::~AbstractLayoutItem() 
{}

int AbstractLayoutItem::maxWidth() const {
	return _maximumWidth;
}

int AbstractLayoutItem::minHeight() const {
	return _minimumHeight;
}

int AbstractLayoutItem::resizeGetHeight(int width) {
	_width = qMin(width, _maximumWidth);
	_height = _minimumHeight;

	return _height;
}

int AbstractLayoutItem::width() const {
	return _width;
}

int AbstractLayoutItem::height() const {
	return _height;
}

void AbstractLayoutItem::setPosition(int position) {
	_position = position;
}
int AbstractLayoutItem::position() const {
	return _position;
}

bool AbstractLayoutItem::hasPoint(QPoint point) const {
	return QRect(0, 0, width(), height()).contains(point);
}
