#include "ContinuousScroll.h"

#include <QScrollBar>
#include <QWheelEvent>


void ContinuousScroll::wheelEvent(QWheelEvent* event) {
	if (!event->angleDelta().isNull()
		&& (event->angleDelta().y() < 0)
		&& (scrollTopMax() == scrollTop())) {
		emit addContentRequest();
		viewportEvent(event);
		return;
	}

	QScrollArea::wheelEvent(event);
}

void ContinuousScroll::setTrackingContent(bool value) {
	if (_tracking == value)
		return;
	
	_tracking = value;
	reconnect();
}

void ContinuousScroll::reconnect() {
	if (!_tracking) {
		_connection = QMetaObject::Connection();
		return;
	}

	const auto handleAction = [=](int action) {
		qDebug() << "handleAction: " << action;
		const auto scroll = verticalScrollBar();
		const auto step = (action == QAbstractSlider::SliderSingleStepAdd)
			? scroll->singleStep()
			: (action == QAbstractSlider::SliderPageStepAdd)
			? scroll->pageStep()
			: 0;

		if (!action)
			return;

		const auto newTop = scrollTop() + step;
		if (newTop > scrollTopMax()) {
			emit addContentRequest();
			scroll->setSliderPosition(newTop);
		}
	};
	_connection = QObject::connect(
		verticalScrollBar(),
		&QAbstractSlider::actionTriggered,
		handleAction
	);
}
