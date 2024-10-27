#pragma once

#include <QScrollArea>
#include <functional>

class ScrollArea: public QScrollArea {
public:
	ScrollArea(QWidget* parent);

	int scrollWidth() const;
	int scrollHeight() const;
	int scrollLeftMax() const;
	int scrollTopMax() const;
	int scrollLeft() const;
	int scrollTop() const;


	bool focusNextPrevChild(bool next) override;
	void setMovingByScrollBar(bool movingByScrollBar);

	bool viewportEvent(QEvent* e) override;
	void keyPressEvent(QKeyEvent* e) override;

	[[nodiscard]] int computeScrollTo(int toTop, int toBottom);

	void scrollToY(int toTop, int toBottom = -1);

protected:

	bool eventFilter(QObject* obj, QEvent* e) override;
private:
	bool _disabled = false;
	bool _movingByScrollBar = false;

	int _horizontalValue, _verticalValue;

	bool _touchScroll = false;
	bool _touchPress = false;
	bool _touchRightButton = false;
	QPoint _touchStart, _touchPrevPos, _touchPos;

	bool _touchPrevPosValid = false;
	bool _touchWaitingAcceleration = false;

	std::function<bool(QWheelEvent*)> _customWheelProcess;

	bool _widgetAcceptsTouch = false;
};