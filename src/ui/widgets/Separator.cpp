#include "Separator.h"

#include <QPainter>


Separator::Separator(QWidget* parent) :
	QWidget(parent)
{
	auto painter = QPainter(this);

	p.fillRect(0, 0, width(), _height, _bg);
	p.fillRect(
		_padding.left(),
		_padding.top(),
		width() - _padding.left() - _padding.right(),
		_lineWidth,
		_fg);
}