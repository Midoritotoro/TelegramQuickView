#include "ScrollArea.h"

#include <QScrollBar>
#include <QEvent>
#include <QWheelEvent>


namespace style {
	template <typename T>
	[[nodiscard]] inline T ConvertScale(T value) {
		if (value < 0.) {
			return -ConvertScale(-value);
		}
		const auto result = T(std::round(
			(double(value) * 1 / 100.) - 0.01));
		return (!std::is_integral_v<T> || !value || result) ? result : 1;
	}
}


ScrollArea::ScrollArea(QWidget* parent):
	QScrollArea(parent)
{
	setFocusPolicy(Qt::NoFocus);

	qDebug() << style::ConvertScale(verticalScrollBar()->singleStep()) << verticalScrollBar()->singleStep();

	verticalScrollBar()->setSingleStep(style::ConvertScale(verticalScrollBar()->singleStep()));

	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	setFrameStyle(int(QFrame::NoFrame) | QFrame::Plain);
	viewport()->setAutoFillBackground(false);

	_horizontalValue = horizontalScrollBar()->value();
	_verticalValue = verticalScrollBar()->value();
}

int ScrollArea::scrollWidth() const {
	QWidget* w(widget());
	return w ? qMax(w->width(), width()) : width();
}

int ScrollArea::scrollHeight() const {
	QWidget* w(widget());
	return w ? qMax(w->height(), height()) : height();
}

int ScrollArea::scrollLeftMax() const {
	return scrollWidth() - width();
}

int ScrollArea::scrollTopMax() const {
	return scrollHeight() - height();
}

int ScrollArea::scrollLeft() const {
	return _horizontalValue;
}

int ScrollArea::scrollTop() const {
	return _verticalValue;
}

bool ScrollArea::eventFilter(QObject* obj, QEvent* e) {
	const auto result = QScrollArea::eventFilter(obj, e);
	return obj == widget() || result;
}

bool ScrollArea::viewportEvent(QEvent* e) {
	if (e->type() == QEvent::Wheel) {
		if (_customWheelProcess
			&& _customWheelProcess(static_cast<QWheelEvent*>(e))) {
			return true;
		}
	}
	return QScrollArea::viewportEvent(e);
}

void ScrollArea::keyPressEvent(QKeyEvent* e) {
	if ((e->key() == Qt::Key_Up || e->key() == Qt::Key_Down)
		&& (e->modifiers().testFlag(Qt::AltModifier)
			|| e->modifiers().testFlag(Qt::ControlModifier))) {
		e->ignore();
	}
	else if (e->key() == Qt::Key_Escape || e->key() == Qt::Key_Back) {
		((QObject*)widget())->event(e);
	}
	else {
		QScrollArea::keyPressEvent(e);
	}
}

int ScrollArea::computeScrollTo(int toTop, int toBottom) {
	const auto toMin = 0;
	const auto toMax = scrollTopMax();
	if (toTop < toMin) {
		toTop = toMin;
	}
	else if (toTop > toMax) {
		toTop = toMax;
	}
	const auto exact = (toBottom < 0);

	const auto curTop = scrollTop();
	const auto curHeight = height();
	const auto curBottom = curTop + curHeight;
	auto scToTop = toTop;
	if (!exact && toTop >= curTop) {
		if (toBottom < toTop) {
			toBottom = toTop;
		}
		if (toBottom <= curBottom) {
			return curTop;
		}

		scToTop = toBottom - curHeight;
		if (scToTop > toTop) {
			scToTop = toTop;
		}
		if (scToTop == curTop) {
			return curTop;
		}
	}
	else {
		scToTop = toTop;
	}
	return scToTop;
}

void ScrollArea::scrollToY(int toTop, int toBottom) {
	verticalScrollBar()->setValue(computeScrollTo(toTop, toBottom));
}


bool ScrollArea::focusNextPrevChild(bool next) {
	if (QWidget::focusNextPrevChild(next)) {
		return true;
	}
	return false;
}

void ScrollArea::setMovingByScrollBar(bool movingByScrollBar) {
	_movingByScrollBar = movingByScrollBar;
}