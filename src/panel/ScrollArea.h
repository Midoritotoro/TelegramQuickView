#pragma once

#include <QScrollArea>


class ScrollArea: public QScrollArea {
public:
	ScrollArea(QWidget* parent = nullptr);

	int scrollHeight() const;
	int scrollTopMax() const;
	int scrollTop() const;

	bool focusNextPrevChild(bool next) override;
	void setOpacity(double opacity);

	[[nodiscard]] int computeScrollTo(int toTop, int toBottom);

	void scrollToY(int toTop, int toBottom = -1);
	void scrolled();

	void scrollToWidget(not_null<QWidget*> widget);

	void disableScroll(bool dis);
protected:
	bool viewportEvent(QEvent* event) override;
	void keyPressEvent(QKeyEvent* event) override;

	void paintEvent(QPaintEvent* event) override;
private:
	bool _disabled = false;

	int _verticalValue;
	double _opacity;
};