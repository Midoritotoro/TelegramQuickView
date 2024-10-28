#include "ScrollArea.h"

#include <QScrollBar>
#include <QEvent>
#include <QWheelEvent>

#include "../core/StyleCore.h"

ScrollArea::ScrollArea(QWidget* parent):
	QScrollArea(parent)
{
	setLayoutDirection(Qt::LeftToRight);

	//qDebug() << style::ConvertScale(verticalScrollBar()->singleStep()) << verticalScrollBar()->singleStep();

	verticalScrollBar()->setSingleStep(style::convertScale(verticalScrollBar()->singleStep()));
	
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	setFrameStyle(int(QFrame::NoFrame) | QFrame::Plain);
	viewport()->setAutoFillBackground(false);

	_verticalValue = verticalScrollBar()->value();

	connect(verticalScrollBar(), &QAbstractSlider::valueChanged, [=] {
		scrolled();
		});
	connect(verticalScrollBar(), &QAbstractSlider::rangeChanged, [=] {
		scrolled();
		});

}

void ScrollArea::scrolled() {
	const int verticalValue = verticalScrollBar()->value();

	if (_verticalValue != verticalValue) {
		if (_disabled) 
			verticalScrollBar()->setValue(_verticalValue);
		else
			_verticalValue = verticalValue;
	}
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

bool ScrollArea::eventFilter(QObject* obj, QEvent* e) {
	QElapsedTimer timer;
	timer.start();

	qDebug() << e->type();

	const auto result = QScrollArea::eventFilter(obj, e);

	qDebug() << "eventFilter time taken: " << static_cast<double>(timer.elapsed()) / 1000 << " s";
	return result;
}

bool ScrollArea::viewportEvent(QEvent* e) {
	if (e->type() == QEvent::Wheel) {
		return false;
		// ...
	}
	return QScrollArea::viewportEvent(e);
}

void ScrollArea::keyPressEvent(QKeyEvent* e) {
	if ((e->key() == Qt::Key_Up || e->key() == Qt::Key_Down)
		&& (e->modifiers().testFlag(Qt::AltModifier)
			|| e->modifiers().testFlag(Qt::ControlModifier)))
		e->ignore();
	else if (e->key() == Qt::Key_Escape || e->key() == Qt::Key_Back)
		((QObject*)widget())->event(e);
	else
		QScrollArea::keyPressEvent(e);
}

void ScrollArea::scrollToWidget(QWidget* widget) {
	if (auto local = this->widget()) {
		auto globalPosition = widget->mapToGlobal(QPoint(0, 0));
		auto localPosition = local->mapFromGlobal(globalPosition);
		auto localTop = localPosition.y();
		auto localBottom = localTop + widget->height();
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

bool ScrollArea::focusNextPrevChild(bool next) {
	if (QWidget::focusNextPrevChild(next)) {
		return true;
	}
	return false;
}