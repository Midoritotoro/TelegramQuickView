#pragma once

#include "../core/StyleCore.h"
#include "../core/Timer.h"

#include "../core/Types.h"

#include <QWidget>


namespace style {
	namespace flatLabel {
		inline constexpr QMargins margins = {
				8, 5, 20, 10
		};

		inline constexpr auto defaultColor = QColor(24, 37, 51);
	}
} // namespace style


struct TextSelection {
	enum class Type {
		Letters = 0x01,
		Words = 0x02,
		Paragraphs = 0x03,
	};

	constexpr TextSelection() = default;
	constexpr TextSelection(quint16 from, quint16 to) : from(from), to(to) {
	}

	[[nodiscard]] constexpr bool empty() const noexcept {
		return from == to;
	}
	quint16 from = 0;
	quint16 to = 0;
};

inline bool operator==(TextSelection a, TextSelection b) {
	return a.from == b.from && a.to == b.to;
}

inline bool operator!=(TextSelection a, TextSelection b) {
	return !(a == b);
}

static constexpr TextSelection AllTextSelection = { 0, 0xFFFF };


class FlatLabel: public QWidget {
	Q_OBJECT
public:
	struct ContextMenuRequest {
		QMenu* menu;
		TextSelection selection;
		bool uponSelection = false;
		bool fullSelection = false;
	};

	FlatLabel(QWidget* parent = nullptr);

	[[nodiscard]] QSize sizeHint() const override;

	[[nodiscard]] int textMaxWidth() const noexcept;
	[[nodiscard]] bool hasLinks() const noexcept;

	void setText(const QString& text);
	[[nodiscard]] QString text() const noexcept;

	void setSelectable(bool selectable);
	[[nodiscard]] bool selectable() const noexcept;

	void setOpacity(float opacity);
	[[nodiscard]] float opacity() const noexcept;

	void setTextAlignment(Qt::Alignment alignment);
	[[nodiscard]] Qt::Alignment alignment() const noexcept;

	void setContextMenu(not_null<QMenu*> menu);
	[[nodiscard]] QMenu* contextMenu() const noexcept;

	void setBackgroundColor(const QColor& color);
	[[nodiscard]] QColor backgroundColor() const noexcept;

	void setCornerRoundMode(style::CornersRoundMode cornersRoundMode);
	[[nodiscard]] style::CornersRoundMode cornerRoundMode() const noexcept;

	void setContextMenuHook(Fn<void(ContextMenuRequest)> hook);

	void setLink(quint16 index);
	void fillContextMenu(ContextMenuRequest request);
protected:
	void paintEvent(QPaintEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;

	void mouseReleaseEvent(QMouseEvent* event) override;
	void mouseDoubleClickEvent(QMouseEvent* event) override;
	
	void focusOutEvent(QFocusEvent* event) override;
	void focusInEvent(QFocusEvent* event) override;

	void keyPressEvent(QKeyEvent* event) override;
	void contextMenuEvent(QContextMenuEvent* event) override;
private:
	enum class ContextMenuReason {
		FromEvent,
		FromTouch,
	};
	void showContextMenu(QContextMenuEvent* event, ContextMenuReason reason);

	[[nodiscard]] int countTextWidth() const noexcept;
	[[nodiscard]] int countTextHeight();

	void refreshSize();
	void textUpdated();

	void copySelectedText();

	enum DragAction {
		NoDrag = 0x00,
		PrepareDrag = 0x01,
		Dragging = 0x02,
		Selecting = 0x04,
	};

	QString _text = "";
	Qt::Alignment _alignment;

	QColor _backgroundColor;

	float _opacity = 1.;
	bool _selectable;

	int _allowedWidth = 0;
	int _textWidth = 0;

	int _fullTextHeight = 0;

	TextSelection _selection, _savedSelection;
	QMenu* _contextMenu = nullptr;

	Fn<void(ContextMenuRequest)> _contextMenuHook = nullptr;
	style::CornersRoundMode _cornersRoundMode;

	DragAction _dragAction = NoDrag;
	QPoint _dragStartPosition;
	quint16 _dragSymbol = 0;
	bool _dragWasInactive = false;

	QPoint _lastMousePos;

	QPoint _trippleClickPoint;
	core::Timer _trippleClickTimer;
};
