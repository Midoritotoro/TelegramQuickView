#pragma once

#include "../style/StyleCore.h"
#include "../../core/Timer.h"

#include "../../core/InvokeQueued.h"
#include "../BasicClickHandlers.h"

#include "../text/String.h"

#include <QWidget>
#include "../style/StyleFont.h"

#include "../style/StyleWidgets.h"


class PopupMenu : public QWidget {
	Q_OBJECT
public:
	PopupMenu(QWidget* parent = nullptr);

	[[nodiscard]] QSize sizeHint() const override;
	[[nodiscard]] QSize minimumSizeHint() const override;

	void setAction(const QAction& action);
	[[nodiscard]] QAction action(int index) const noexcept;

	void setOpacity(float opacity);
	[[nodiscard]] float opacity() const noexcept;

	void setStyle(
		const style::PopupMenu* style,
		bool repaint = true);
	[[nodiscard]] const style::PopupMenu* style() const noexcept;
protected:
	void paintEvent(QPaintEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;

	void mouseReleaseEvent(QMouseEvent* event) override;
	void mouseDoubleClickEvent(QMouseEvent* event) override;

	void focusOutEvent(QFocusEvent* event) override;
	void focusInEvent(QFocusEvent* event) override;

	void keyPressEvent(QKeyEvent* event) override;
	bool event(QEvent* _event) override;
private:
	const style::PopupMenu* _st = nullptr;

	
};