#include "FlatLabel.h"

#include <QMouseEvent>
#include <QDesktopServices>

#include <QPainter>
#include <QClipboard>

#include <QMenu>
#include <QApplication>

#include <QMouseEvent>
#include <QDrag>

#include "../text/TextUtility.h"


FlatLabel::FlatLabel(QWidget* parent) :
	QWidget(parent)
	, ClickHandlerHost()
	, _st(new 
		style::TextStyle{
			._font = style::font(13, 0, 0),
			.lineHeight = 0,
			.linkUnderLine = true,
			.blockquote = {}})
{
	init();

	setSelectable(true);
	setMouseTracking(true);
	
	setDoubleClickSelectsParagraph(true);
	setBreakEverywhere(true);

	setBackgroundColor(style::flatLabel::defaultColor);
	setCornerRoundMode(style::CornersRoundMode::All);

	setTextAlignment(Qt::AlignLeft);
	setLinksTrusted();
}

void FlatLabel::init() {
	_contextCopyText = style::flatLabel::phraseContextCopySelected;
}

void FlatLabel::setText(const QString& text) {
	_text.setText(_st, text);
	textUpdated();
}

QString FlatLabel::text() const noexcept {
	return _text.toString();
}

void FlatLabel::setSelectable(bool selectable) {
	_selectable = selectable;
}

bool FlatLabel::selectable() const noexcept {
	return _selectable;
}

void FlatLabel::setDoubleClickSelectsParagraph(bool doubleClickSelectsParagraph) {
	_doubleClickSelectsParagraph = doubleClickSelectsParagraph;
}

bool FlatLabel::doubleClickSelectsParagraph() const noexcept {
	return _doubleClickSelectsParagraph;
}

void FlatLabel::setBreakEverywhere(bool breakEverywhere) {
	_breakEverywhere = breakEverywhere;
}

bool FlatLabel::breakEverywhere() const noexcept {
	return _breakEverywhere;
}

void FlatLabel::setOpacity(float opacity) {
	if (_opacity == opacity)
		return;

	_opacity = opacity;
	update();
}

float FlatLabel::opacity() const noexcept {
	return _opacity;
}

int FlatLabel::textMaxWidth() const noexcept {
	return style::maximumMessageWidth
		- style::flatLabel::margins.left()
		- style::flatLabel::margins.right();
}

bool FlatLabel::hasLinks() const noexcept {
	return _text.hasLinks();
}

void FlatLabel::setTextAlignment(Qt::Alignment alignment) {
	_alignment = alignment;
}

Qt::Alignment FlatLabel::alignment() const noexcept {
	return _alignment;
}

void FlatLabel::setContextMenu(not_null<QMenu*> menu) {
	_contextMenu = menu;
}

QMenu* FlatLabel::contextMenu() const noexcept {
	return _contextMenu;
}

void FlatLabel::setBackgroundColor(const QColor& color) {
	_backgroundColor = color;
}

QColor FlatLabel::backgroundColor() const noexcept {
	return _backgroundColor;
}

void FlatLabel::setCornerRoundMode(style::CornersRoundMode cornersRoundMode) {
	_cornersRoundMode = cornersRoundMode;
}

style::CornersRoundMode FlatLabel::cornerRoundMode() const noexcept {
	return _cornersRoundMode;
}

void FlatLabel::setContextMenuHook(Fn<void(ContextMenuRequest)> hook) {
	_contextMenuHook = std::move(hook);
}

void FlatLabel::setLink(
	quint16 index,
	const ClickHandlerPtr& lnk)
{
	_text.setLink(index, lnk);
	textUpdated();
}

void FlatLabel::setLinksTrusted() {
	static const auto TrustedLinksFilter = [](
		const ClickHandlerPtr& link,
		Qt::MouseButton button) {
			if (const auto url = dynamic_cast<UrlClickHandler*>(link.get())) {
				url->UrlClickHandler::onClick({ button });
				return false;
			}
			return true;
		};
	setClickHandlerFilter(TrustedLinksFilter);
}

void FlatLabel::setClickHandlerFilter(ClickHandlerFilter&& filter) {
	_clickHandlerFilter = std::move(filter);
}

void FlatLabel::overrideLinkClickHandler(Fn<void()> handler) {
	setClickHandlerFilter([=](
		const ClickHandlerPtr& link,
		Qt::MouseButton button) {
			if (button != Qt::LeftButton) {
				return true;
			}
			handler();
			return false;
		});
}

void FlatLabel::overrideLinkClickHandler(Fn<void(QString url)> handler) {
	setClickHandlerFilter([=](
		const ClickHandlerPtr& link,
		Qt::MouseButton button) {
			if (button != Qt::LeftButton) {
				return true;
			}
			handler(link->url());
			return false;
		});
}

void FlatLabel::paintEvent(QPaintEvent* event) {
	auto painter = QPainter(this);

	//painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
	//painter.setOpacity(_opacity);

	//painter.setPen(Qt::white);

	//style::RoundCorners(painter, size(), 10, _cornersRoundMode);

	const auto textWidth = _textWidth
		? _textWidth
		: width()
			- style::flatLabel::margins.left()
			- style::flatLabel::margins.right();

	const auto textLeft = _textWidth
		? ((_alignment & Qt::AlignLeft)
			? style::flatLabel::margins.left()
			: (_alignment & Qt::AlignHCenter)
			? ((width() - _textWidth) / 2)
			: (width() - style::flatLabel::margins.right() - _textWidth))
		: style::flatLabel::margins.left();

	const auto selection = !_selection.empty()
		? _selection
		: _contextMenu
		? _savedSelection
		: _selection;

	const auto heightExceeded = style::maximumTextHeight < _fullTextHeight || textWidth < style::maximumTextWidth;
	const auto renderElided = _breakEverywhere || heightExceeded;
	const auto lineHeight = _text.style()
		? qMax(_text.style()->lineHeight, 14)
		: QFontMetrics(font()).height();
	const auto elisionHeight = !heightExceeded
		? 0
		: style::maximumTextHeight
		? qMax(style::maximumTextHeight, lineHeight)
		: height();

	_text.draw(painter, {
		.position = { textLeft, style::flatLabel::margins.top() },
		.availableWidth = textWidth,
		.align = style::alignLeft,
		.clip = event->rect(),
		.palette = new style::TextPalette(),
		.now = Time::now(),
		.selection = selection,
		.elisionHeight = elisionHeight,
		.elisionBreakEverywhere = renderElided && _breakEverywhere,
		});
}

void FlatLabel::mouseMoveEvent(QMouseEvent* event) {
	_lastMousePos = event->globalPos();
	dragActionUpdate();
}

void FlatLabel::mousePressEvent(QMouseEvent* event) {
	if (_contextMenu)
		return event->accept();

	dragActionStart(event->globalPos(), event->button());
}

void FlatLabel::mouseReleaseEvent(QMouseEvent* event) {
	dragActionFinish(event->globalPos(), event->button());

	if (!rect().contains(event->pos()))
		leaveEvent(event);
}

void FlatLabel::mouseDoubleClickEvent(QMouseEvent* event) {
	auto state = dragActionStart(event->globalPos(), event->button());

	if (((_dragAction == Selecting) || (_dragAction == NoDrag)) && _selectionType == text::TextSelection::Type::Letters) {
		if (state.uponSymbol) {
			_dragSymbol = state.symbol;
			_selectionType = _doubleClickSelectsParagraph ? text::TextSelection::Type::Paragraphs : text::TextSelection::Type::Words;

			if (_dragAction == NoDrag) {
				_dragAction = Selecting;
				_selection = { state.symbol, state.symbol };
				_savedSelection = { 0, 0 };
			}

			mouseMoveEvent(event);

			_trippleClickPoint = event->globalPos();
			_trippleClickTimer.callOnce(QApplication::doubleClickInterval());
		}
	}
}

void FlatLabel::focusOutEvent(QFocusEvent* event) {
	if (_selection.empty())
		return;

	if (_contextMenu)
		_savedSelection = _selection;

	_selection = text::TextSelection(0, 0);
	update();
}

void FlatLabel::focusInEvent(QFocusEvent* event) {
	if (!_savedSelection.empty()) {
		_selection = _savedSelection;
		_savedSelection = { 0, 0 };
		update();
	}
}

void FlatLabel::keyPressEvent(QKeyEvent* event) {
	event->ignore();
	if (event->key() == Qt::Key_Copy || (event->key() == Qt::Key_C
		&& event->modifiers().testFlag(Qt::ControlModifier)))
		if (!_selection.empty()) {
			copySelectedText();
			event->accept();
		}
}

void FlatLabel::contextMenuEvent(QContextMenuEvent* event) {
	if (!_contextMenuHook && !_selectable && !hasLinks())
		return;

	showContextMenu(event, ContextMenuReason::FromEvent);
}

void FlatLabel::copyContextText() {
	QGuiApplication::clipboard()->setText(_text.toString());
}

void FlatLabel::copySelectedText() {
	const auto selection = _selection.empty()
		? (_contextMenu
			? _savedSelection
			: _selection)
		: _selection;

	if (!selection.empty())
		QGuiApplication::clipboard()->setText(_text.toString());
}

text::TextState FlatLabel::dragActionUpdate() {
	auto m = mapFromGlobal(_lastMousePos);
	auto state = getTextState(m);
	updateHover(state);

	if (_dragAction == PrepareDrag && (m - _dragStartPosition).manhattanLength() >= QApplication::startDragDistance()) {
		_dragAction = Dragging;
		core::InvokeQueued(this, [=] { executeDrag(); });
	}

	return state;
}


text::TextState FlatLabel::dragActionStart(const QPoint& p, Qt::MouseButton button) {
	_lastMousePos = p;
	auto state = dragActionUpdate();

	if (button != Qt::LeftButton) return state;

	ClickHandler::pressed();
	_dragAction = NoDrag;

	if (ClickHandler::getPressed()) {
		_dragStartPosition = mapFromGlobal(_lastMousePos);
		_dragAction = PrepareDrag;
	}
	if (!_selectable || _dragAction != NoDrag) {
		return state;
	}

	if (_trippleClickTimer.isActive() && (_lastMousePos - _trippleClickPoint).manhattanLength() < QApplication::startDragDistance()) {
		if (state.uponSymbol) {
			_selection = { state.symbol, state.symbol };
			_savedSelection = { 0, 0 };
			_dragSymbol = state.symbol;
			_dragAction = Selecting;
			_selectionType = text::TextSelection::Type::Paragraphs;
			updateHover(state);
			_trippleClickTimer.callOnce(QApplication::doubleClickInterval());
			update();
		}
	}
	if (_selectionType != text::TextSelection::Type::Paragraphs) {
		_dragSymbol = state.symbol;
		bool uponSelected = state.uponSymbol;
		if (uponSelected) {
			if (_dragSymbol < _selection.from || _dragSymbol >= _selection.to) {
				uponSelected = false;
			}
		}
		if (uponSelected) {
			_dragStartPosition = mapFromGlobal(_lastMousePos);
			_dragAction = PrepareDrag; // start text drag
		}
		else {
			if (state.afterSymbol) ++_dragSymbol;
			_selection = { _dragSymbol, _dragSymbol };
			_savedSelection = { 0, 0 };
			_dragAction = Selecting;
			update();
		}
	}
	return state;
}

text::TextState FlatLabel::dragActionFinish(const QPoint& p, Qt::MouseButton button) {
	_lastMousePos = p;
	auto state = dragActionUpdate();

	auto activated = ClickHandler::unpressed();

	if (_dragAction == Dragging)
		activated = nullptr;
	else if (_dragAction == PrepareDrag) {
		_selection = { 0, 0 };
		_savedSelection = { 0, 0 };
		update();
	}
	_dragAction = NoDrag;
	_selectionType = text::TextSelection::Type::Letters;

	if (activated) {
		const auto guard = window();
		if (!_clickHandlerFilter
			|| _clickHandlerFilter(activated, button))
			ActivateClickHandler(guard, activated, button);
	}

	if (QGuiApplication::clipboard()->supportsSelection()
		&& !_selection.empty())
		text::SetClipboardText(
			_text.toTextForMimeData(_selection),
			QClipboard::Selection);
	
	return state;
}

void FlatLabel::updateHover(const text::TextState& state) {
	bool lnkChanged = ClickHandler::setActive(state.link, this);

	if (!_selectable) {
		refreshCursor(state.uponSymbol);
		return;
	}

	Qt::CursorShape cur = style::cursorDefault;
	if (_dragAction == NoDrag) {
		if (state.link) {
			cur = style::cursorPointer;
		}
		else if (state.uponSymbol) {
			cur = style::cursorText;
		}
	}
	else {
		if (_dragAction == Selecting) {
			uint16 second = state.symbol;
			if (state.afterSymbol && _selectionType == text::TextSelection::Type::Letters) {
				++second;
			}
			auto selection = _text.adjustSelection({ qMin(second, _dragSymbol), qMax(second, _dragSymbol) }, _selectionType);
			if (_selection != selection) {
				_selection = selection;
				_savedSelection = { 0, 0 };
				setFocus();
				update();
			}
		}

		if (ClickHandler::getPressed())
			cur = style::cursorPointer;
		else if (_dragAction == Selecting)
			cur = style::cursorText;
	}
	if (_dragAction == NoDrag && (lnkChanged || cur != _cursor)) {
		setCursor(_cursor = cur);
	}
}

text::TextState FlatLabel::getTextState(const QPoint& m) const {
	text::StateRequestElided request;
	request.align = style::alignLeft;
	if (_selectable) {
		request.flags |= text::StateRequest::StateFlag::LookupSymbol;
	}
	int textWidth = width() 
		- style::flatLabel::margins.left() 
		- style::flatLabel::margins.right();

	text::TextState state;
	bool heightExceeded = (style::maximumTextHeight < _fullTextHeight || textWidth < _text.maxWidth());
	bool renderElided = _breakEverywhere || heightExceeded;
	if (renderElided) {
		auto lineHeight = _text.style() && _text.style()->_font
			? qMax(1, _text.style()->_font->height)
			: 1;
		auto lines = qMax(style::maximumTextHeight / lineHeight, 1);
		request.lines = lines;
		if (_breakEverywhere) {
			request.flags |= text::StateRequest::StateFlag::BreakEverywhere;
		}
		state = _text.getStateElided(m - QPoint(style::flatLabel::margins.left(), style::flatLabel::margins.top()), textWidth, request);
	} else {
		state = _text.getState(m - QPoint(style::flatLabel::margins.left(), style::flatLabel::margins.top()), textWidth, request);
	}

	return state;
}

void FlatLabel::touchSelect() {
	_touchSelect = true;
	dragActionStart(_touchPos, Qt::LeftButton);
}

void FlatLabel::executeDrag() {
	if (_dragAction != Dragging) return;

	auto state = getTextState(_dragStartPosition);
	bool uponSelected = state.uponSymbol && _selection.from <= state.symbol;
	if (uponSelected) {
		if (_dragSymbol < _selection.from || _dragSymbol >= _selection.to) {
			uponSelected = false;
		}
	}

	const auto pressedHandler = ClickHandler::getPressed();
	const auto selectedText = [&] {
		if (uponSelected)
			return _text.toTextForMimeData(_selection);
		else if (pressedHandler)
			return text::TextForMimeData::Simple(pressedHandler->dragText());
		
		return text::TextForMimeData();
	}();

	if (auto mimeData = text::MimeDataFromText(selectedText)) {
		auto drag = new QDrag(window());
		drag->setMimeData(mimeData.release());
		drag->exec(Qt::CopyAction);

		ClickHandler::unpressed();
	}
}


void FlatLabel::showContextMenu(QContextMenuEvent* e, ContextMenuReason reason) {
	if (e->reason() == QContextMenuEvent::Mouse)
		_lastMousePos = e->globalPos();
	else
		_lastMousePos = QCursor::pos();

	const auto state = dragActionUpdate();
	const auto hasSelection = _selectable && !_selection.empty();
	const auto uponSelection = _selectable
		&& ((reason == ContextMenuReason::FromTouch && hasSelection)
			|| (state.uponSymbol
				&& (state.symbol >= _selection.from)
				&& (state.symbol < _selection.to)));

	_contextMenu = new QMenu(this);
	auto request = ContextMenuRequest({
		.menu = _contextMenu,
		.selection = _selectable ? _selection : text::TextSelection(),
		.uponSelection = uponSelection,
		.fullSelection = _selectable && _selection.isFullSelection(text())
	});

	if (_contextMenuHook)
		_contextMenuHook(request);
	else
		fillContextMenu(request);

	if (_contextMenu)
		_contextMenu = nullptr;
	else {
		_contextMenu->popup(e->globalPos());
		e->accept();
	}
}

void FlatLabel::fillContextMenu(ContextMenuRequest request) {
	if (request.fullSelection && !_contextCopyText.isEmpty()) {
		request.menu->addAction(
			_contextCopyText,
			[=] { copyContextText(); });
	}
	else if (request.uponSelection && !request.fullSelection)
		request.menu->addAction(
			_contextCopyText,
			[=] { copySelectedText(); });
	else if (_selectable
		&& request.selection.empty()
		&& !_contextCopyText.isEmpty()) {
		request.menu->addAction(
			_contextCopyText,
			[=] { copyContextText(); });
	}

	if (request.link) {
		const auto label = request.link->copyToClipboardContextItemText();
		if (!label.isEmpty()) {
			request.menu->addAction(
				label,
				[text = request.link->copyToClipboardText()] {
					QGuiApplication::clipboard()->setText(text);
				});
		}
	}
}

int FlatLabel::countTextWidth() const noexcept {
	const auto available = _allowedWidth
		? _allowedWidth
		: _text.maxWidth();
	if (_allowedWidth > 0
		&& _allowedWidth < _text.maxWidth()) {
		auto large = _allowedWidth;
		auto small = _allowedWidth / 2;
		const auto largeHeight = _text.countHeight(large, true);
		while (large - small > 1) {
			const auto middle = (large + small) / 2;
			if (largeHeight == _text.countHeight(middle, true)) {
				large = middle;
			}
			else {
				small = middle;
			}
		}
		return large;
	}
	return available;
}

int FlatLabel::countTextHeight(int textWidth) {
	_fullTextHeight = _text.countHeight(textWidth, true);
	return qMin(_fullTextHeight, style::maximumTextHeight);
}

void FlatLabel::refreshSize() {
	const auto textWidth = countTextWidth();
	const auto textHeight = countTextHeight(textWidth);

	const auto fullWidth = textWidth
		+ style::flatLabel::margins.right()
		+ style::flatLabel::margins.left();

	const auto fullHeight = textHeight
		+ style::flatLabel::margins.top()
		+ style::flatLabel::margins.bottom();

	resize(fullWidth, fullHeight);
}

void FlatLabel::refreshCursor(bool uponSymbol) {
	if (_dragAction != NoDrag)
		return;

	const auto needTextCursor = _selectable && uponSymbol;
	auto newCursor = needTextCursor ? style::cursorText : style::cursorDefault;

	if (ClickHandler::getActive())
		newCursor = style::cursorPointer;

	if (newCursor != _cursor) {
		_cursor = newCursor;
		setCursor(_cursor);
	}
}

void FlatLabel::textUpdated() {
	refreshSize();
	setMouseTracking(_selectable || hasLinks());
	update();
}