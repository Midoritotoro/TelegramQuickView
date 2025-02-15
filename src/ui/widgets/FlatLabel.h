﻿#pragma once

#include "../style/StyleCore.h"
#include "../../core/Timer.h"

#include "../../core/InvokeQueued.h"
#include "../BasicClickHandlers.h"

#include "../text/String.h"
#include "PopupMenu.h"


namespace {
	inline constexpr auto phraseContextCopySelected = "Копировать текст";

	inline constexpr text::TextParseOptions _labelOptions = {
		text::TextParseMultiline | text::TextParseLinks | text::TextParseHashtags
		| text::TextParseMentions | text::TextParseBotCommands | text::TextParseMarkdown,
		0,
		0,
		Qt::LayoutDirectionAuto
	};
} // namespace


class FlatLabel : 
	public QWidget, 
	public ClickHandlerHost 
{
	Q_OBJECT
public:
	struct ContextMenuRequest {
		PopupMenu* menu;
		text::TextSelection selection;
		ClickHandlerPtr link;
		bool uponSelection = false;
		bool fullSelection = false;
	};

	FlatLabel(QWidget* parent = nullptr);

	[[nodiscard]] QSize sizeHint() const override;
	[[nodiscard]] QSize minimumSizeHint() const override;

	[[nodiscard]] int textMaxWidth() const noexcept;
	[[nodiscard]] bool hasLinks() const noexcept;

	[[nodiscard]] int fullHeight() const noexcept;

	void setText(const QString& text);
	[[nodiscard]] const text::String& text() const noexcept;

	void setSelectable(bool selectable);
	[[nodiscard]] bool selectable() const noexcept;

	void setDoubleClickSelectsParagraph(bool doubleClickSelectsParagraph);
	[[nodiscard]] bool doubleClickSelectsParagraph() const noexcept;

	void setBreakEverywhere(bool breakEverywhere);
	[[nodiscard]] bool breakEverywhere() const noexcept;

	void setOpacity(float opacity);
	[[nodiscard]] float opacity() const noexcept;

	void setTextAlignment(Qt::Alignment alignment);
	[[nodiscard]] Qt::Alignment alignment() const noexcept;

	void setContextMenu(not_null<PopupMenu*> menu);
	[[nodiscard]] PopupMenu* contextMenu() const noexcept;

	void setCornerRoundMode(style::CornersRoundMode cornersRoundMode);
	[[nodiscard]] style::CornersRoundMode cornerRoundMode() const noexcept;

	void setStyle(const style::FlatLabel* style, bool repaint = true);
	[[nodiscard]] const style::FlatLabel* style() const noexcept;

	void setContextMenuHook(Fn<void(ContextMenuRequest)> hook);

	void setLink(
		quint16 index,
		const ClickHandlerPtr& lnk);
	void setLinksTrusted();

	using ClickHandlerFilter = Fn<bool(const ClickHandlerPtr&, Qt::MouseButton)>;
	void setClickHandlerFilter(ClickHandlerFilter&& filter);

	void overrideLinkClickHandler(Fn<void()> handler);
	void overrideLinkClickHandler(Fn<void(QString url)> handler);

	void init();
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

	bool event(QEvent* e) override;
	void touchEvent(QTouchEvent* e);
private:
	int resizeGetHeight(int newWidth);

	enum class ContextMenuReason {
		FromEvent,
		FromTouch,
	};

	void showContextMenu(
		QContextMenuEvent* event,
		ContextMenuReason reason);
	void fillContextMenu(ContextMenuRequest request);

	[[nodiscard]] int countTextWidth() const noexcept;
	[[nodiscard]] int countTextHeight(int textWidth);

	void refreshSize();
	void refreshCursor(bool uponSymbol);

	void textUpdated();

	void copyContextText();
	void copySelectedText();

	text::TextState dragActionUpdate();

	text::TextState dragActionStart(const QPoint& p, Qt::MouseButton button);
	text::TextState dragActionFinish(const QPoint& p, Qt::MouseButton button);

	void updateHover(const text::TextState& state);
	text::TextState getTextState(const QPoint& m) const;

	void touchSelect();
	void executeDrag();

	enum DragAction {
		NoDrag = 0x00,
		PrepareDrag = 0x01,
		Dragging = 0x02,
		Selecting = 0x04,
	};

	const style::FlatLabel* _st = nullptr;
	
	style::align _alignment = style::alignLeft;
	text::String _text;

	float _opacity = 1.;
	bool _selectable;

	int _allowedWidth = 0;
	int _textWidth = 0;

	int _fullTextHeight = 0;

	style::cursor _cursor = style::cursorDefault;

	text::TextSelection _selection, _savedSelection;
	text::TextSelection::Type _selectionType = text::TextSelection::Type::Letters;

	PopupMenu* _contextMenu = nullptr;

	Fn<void(ContextMenuRequest)> _contextMenuHook = nullptr;
	style::CornersRoundMode _cornersRoundMode;

	ClickHandlerFilter _clickHandlerFilter;
	QString _contextCopyText;

	DragAction _dragAction = NoDrag;

	QPoint _dragStartPosition;
	uint16 _dragSymbol = 0;

	bool _dragWasInactive = false;
	bool _doubleClickSelectsParagraph = true;

	bool _touchSelect = false;
	bool _breakEverywhere = false;

	QPoint _lastMousePos;
	QPoint _touchStart, _touchPrevPos, _touchPos;

	QPoint _trippleClickPoint;
	core::Timer _trippleClickTimer;

	bool _touchInProgress = false;
	core::Timer _touchSelectTimer;
};