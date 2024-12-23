#pragma once

#include "../../core/Types.h"

#include <QScrollArea>
#include <QGridLayout>

#include "../images/ImagesBlur.h"


class InnerWidget : public QWidget {
	Q_OBJECT
public:
	InnerWidget(QWidget* parent = nullptr);

	void setOpacity(double opacity);
	[[nodiscard]] double opacity() const noexcept;

	void setBackgroundColor(const QColor& color);
	[[nodiscard]] QColor backgroundColor() const noexcept;
protected:
	void paintEvent(QPaintEvent* event) override;
private:
	double _opacity = 1.;
	QColor _backgroundColor = Qt::black;
};

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

	void addItem(QWidget* item, Qt::Alignment align = Qt::AlignLeft | Qt::AlignTop);
	[[nodiscard]] InnerWidget* widget() const noexcept;
protected:
	bool viewportEvent(QEvent* event) override;
	void keyPressEvent(QKeyEvent* event) override;

	void paintEvent(QPaintEvent* event) override;
private:
	bool _disabled = false;

	int _verticalValue;
	double _opacity;
	
	QGridLayout* _scrollLayout = nullptr;
};