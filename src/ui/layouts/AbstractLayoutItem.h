#pragma once

#include "../BasicClickHandlers.h"

#include <QRect>


class AbstractLayoutItem: public ClickHandlerHost {
public:
	AbstractLayoutItem();
	virtual ~AbstractLayoutItem();

	AbstractLayoutItem(const AbstractLayoutItem& other) = delete;
	AbstractLayoutItem& operator=(
		const AbstractLayoutItem& other) = delete;

	[[nodiscard]] int maxWidth() const;
	[[nodiscard]] int minHeight() const;

	virtual int resizeGetHeight(int width);

	[[nodiscard]] int width() const;
	[[nodiscard]] int height() const;

	virtual void setPosition(int position);
	[[nodiscard]] int position() const;

	[[nodiscard]] bool hasPoint(QPoint point) const;
protected:
	int _width = 0;
	int _height = 0;

	int _maximumWidth = 0;
	int _minimumHeight = 0;

	int _position = 0; // < 0 - Элемент удалён
};
