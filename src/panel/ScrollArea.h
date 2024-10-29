#pragma once

#include <QScrollArea>
#include <functional>

class ScrollArea: public QScrollArea {
public:
	ScrollArea(QWidget* parent);

	int scrollHeight() const;
	int scrollTopMax() const;
	int scrollTop() const;

	bool focusNextPrevChild(bool next) override;

	bool viewportEvent(QEvent* e) override;
	void keyPressEvent(QKeyEvent* e) override;

	void setOpacity(double opacity);

	[[nodiscard]] int computeScrollTo(int toTop, int toBottom);

	void scrollToY(int toTop, int toBottom = -1);
	void scrolled();

	void scrollToWidget(QWidget* widget);

	void disableScroll(bool dis);
protected:
	void paintEvent(QPaintEvent* event) override;
private:
	bool _disabled = false;

	int _verticalValue;
	double _opacity;
};