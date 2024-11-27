#include "ScrollArea.h"

#include <QScrollBar>
#include <QEvent>
#include <QWheelEvent>
#include <QPainter>

#include "../core/StyleCore.h"


ScrollArea::ScrollArea(QWidget* parent):
	QScrollArea(parent)
{
	setLayoutDirection(Qt::LeftToRight);
	setOpacity(1);

	verticalScrollBar()->setSingleStep(style::ConvertScale(verticalScrollBar()->singleStep()));
	
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	setFrameStyle(int(QFrame::NoFrame) | QFrame::Plain);
	viewport()->setAutoFillBackground(false);

	connect(verticalScrollBar(), &QAbstractSlider::valueChanged, this, &ScrollArea::scrolled);
	connect(verticalScrollBar(), &QAbstractSlider::rangeChanged, this, &ScrollArea::scrolled);

	setStyleSheet(style::ScrollAreaStyle());

	_verticalValue = verticalScrollBar()->value();
}

void ScrollArea::scrolled() {
	const int verticalValue = verticalScrollBar()->value();

	if (_verticalValue != verticalValue)
		if (_disabled) 
			verticalScrollBar()->setValue(_verticalValue);
		else
			_verticalValue = verticalValue;
}

int ScrollArea::scrollHeight() const {
	QWidget* w(widget());
	return w ? qMax(w->height(), height()) : height();
}

int ScrollArea::scrollTopMax() const {
	return scrollHeight() - height();
}

int ScrollArea::scrollTop() const {
	return _verticalValue;
}

bool ScrollArea::viewportEvent(QEvent* event) {
	if (event->type() == QEvent::Wheel) {
		const auto _wheelEvent = static_cast<QWheelEvent*>(event);

		_wheelEvent->angleDelta().y() > 0
			? verticalScrollBar()->setValue(verticalScrollBar()->value() - verticalScrollBar()->singleStep())
			: verticalScrollBar()->setValue(verticalScrollBar()->value() + verticalScrollBar()->singleStep());
		return true;
	}

	return QScrollArea::viewportEvent(event);
}

void ScrollArea::keyPressEvent(QKeyEvent* event) {
	if ((event->key() == Qt::Key_Up || event->key() == Qt::Key_Down)
		&& (event->modifiers().testFlag(Qt::AltModifier)
			|| event->modifiers().testFlag(Qt::ControlModifier))
			|| event->key() == Qt::Key_Left || event->key() == Qt::Key_Right)
		event->ignore();
	else if (event->key() == Qt::Key_Escape || event->key() == Qt::Key_Back)
		((QObject*)widget())->event(event);
	else
		QScrollArea::keyPressEvent(event);
}

void ScrollArea::scrollToWidget(QWidget* widget) {
	if (const auto local = this->widget()) {
		const auto globalPosition = widget->mapToGlobal(QPoint(0, 0));
		const auto localPosition = local->mapFromGlobal(globalPosition);

		const auto localTop = localPosition.y();
		const auto localBottom = localTop + widget->height();

		scrollToY(localTop, localBottom);
	}
}

int ScrollArea::computeScrollTo(int toTop, int toBottom) {
	const auto toMin = 0;
	const auto toMax = scrollTopMax();

	if (toTop < toMin)
		toTop = toMin;
	else if (toTop > toMax)
		toTop = toMax;
	
	const auto exact = (toBottom < 0);

	const auto curTop = scrollTop();
	const auto curHeight = height();
	const auto curBottom = curTop + curHeight;

	auto scToTop = toTop;

	if (!exact && toTop >= curTop) {
		if (toBottom < toTop)
			toBottom = toTop;

		if (toBottom <= curBottom) 
			return curTop;

		scToTop = toBottom - curHeight;
		if (scToTop > toTop)

			scToTop = toTop;
		if (scToTop == curTop)

			return curTop;
	}
	else {
		scToTop = toTop;
	}
	return scToTop;
}

void ScrollArea::scrollToY(int toTop, int toBottom) {
	verticalScrollBar()->setValue(computeScrollTo(toTop, toBottom));
}

void ScrollArea::disableScroll(bool dis) {
	_disabled = dis;
}

void ScrollArea::setOpacity(double opacity) {
	_opacity = opacity;
	update();
}

void ScrollArea::paintEvent(QPaintEvent* event) {
	QPainter painter(viewport());

	painter.setPen(Qt::black);
	painter.setBrush(Qt::black);
	
	painter.setOpacity(_opacity);
	painter.drawRect(rect());
}

bool ScrollArea::focusNextPrevChild(bool next) {
	if (QWidget::focusNextPrevChild(next))
		return true;
	return false;
}