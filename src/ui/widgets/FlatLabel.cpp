#include "FlatLabel.h"

#include <QMouseEvent>
#include <QDesktopServices>

#include <QPainter>
#include <QClipboard>

#include <QMenu>
#include <QApplication>

#include <QMouseEvent>
#include <QDrag>


FlatLabel::FlatLabel(QWidget* parent) :
	QWidget(parent)
	, ClickHandlerHost()
{
	setSelectable(true);
	setMouseTracking(true);

	setBackgroundColor(style::flatLabel::defaultColor);
	setCornerRoundMode(style::CornersRoundMode::All);
}

QSize FlatLabel::sizeHint() const {
	return QSize(_textWidth, _fullTextHeight);
}

void FlatLabel::setText(const QString& text) {
	_text = text;
	textUpdated();
}

QString FlatLabel::text() const noexcept {
	return _text;
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
	return true;
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

void FlatLabel::setLink(quint16 index) {

}

void FlatLabel::paintEvent(QPaintEvent* event) {
	auto painter = QPainter(this);

	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
	painter.setOpacity(_opacity);

	painter.setPen(Qt::white);

	style::RoundCorners(painter, size(), 10, _cornersRoundMode);

	const auto textWidth = _textWidth
		? _textWidth
		: (width()
			- style::flatLabel::margins.left()
			- style::flatLabel::margins.right());

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

	const auto lineHeight = QFontMetrics(font()).height();
	const auto elisionHeight = !heightExceeded
		? 0
		: style::maximumTextHeight
		? qMax(style::maximumTextHeight, lineHeight)
		: height();

	const auto _rect = QRect(
		QPoint(textLeft, style::flatLabel::margins.top()),
		QSize(textWidth, elisionHeight));

	painter.fillRect(_rect, _backgroundColor);
	painter.drawText(_rect, _text);
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

	if (((_dragAction == Selecting) || (_dragAction == NoDrag)) && _selectionType == TextSelection::Type::Letters) {
		if (state.uponSymbol) {
			_dragSymbol = state.symbol;
			_selectionType = _doubleClickSelectsParagraph ? TextSelection::Type::Paragraphs : TextSelection::Type::Words;

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

	_selection = TextSelection(0, 0);
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
	QGuiApplication::clipboard()->setText(_text);
}

void FlatLabel::copySelectedText() {
	const auto selection = _selection.empty()
		? (_contextMenu
			? _savedSelection
			: _selection)
		: _selection;

	if (!selection.empty())
		QGuiApplication::clipboard()->setText(_text);
}

FlatLabel::TextState FlatLabel::dragActionUpdate() {
	auto m = mapFromGlobal(_lastMousePos);
	auto state = getTextState(m);
	updateHover(state);

	if (_dragAction == PrepareDrag && (m - _dragStartPosition).manhattanLength() >= QApplication::startDragDistance()) {
		_dragAction = Dragging;
		core::InvokeQueued(this, [=] { executeDrag(); });
	}

	return state;
}


FlatLabel::TextState FlatLabel::dragActionStart(const QPoint& p, Qt::MouseButton button) {
	_lastMousePos = p;
	auto state = dragActionUpdate();

	if (button != Qt::LeftButton) return state;

	ClickHandler::pressed();
	_dragAction = NoDrag;
	_dragWasInactive = true;

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
			_selectionType = TextSelection::Type::Paragraphs;
			updateHover(state);
			_trippleClickTimer.callOnce(QApplication::doubleClickInterval());
			update();
		}
	}
	if (_selectionType != TextSelection::Type::Paragraphs) {
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
		else if (!_dragWasInactive) {
			if (state.afterSymbol) ++_dragSymbol;
			_selection = { _dragSymbol, _dragSymbol };
			_savedSelection = { 0, 0 };
			_dragAction = Selecting;
			update();
		}
	}
	return state;
}

FlatLabel::TextState FlatLabel::dragActionFinish(const QPoint& p, Qt::MouseButton button) {
	_lastMousePos = p;
	auto state = dragActionUpdate();

	//auto activated = ClickHandler::unpressed();
	auto activated = NoDrag;
	if (_dragAction == Dragging) {
		activated = nullptr;
	}
	else if (_dragAction == PrepareDrag) {
		_selection = { 0, 0 };
		_savedSelection = { 0, 0 };
		update();
	}
	_dragAction = NoDrag;
	_selectionType = TextSelection::Type::Letters;

	if (activated) {
		/*crl::on_main(this, [=] {
			const auto guard = window();
			if (!_clickHandlerFilter
				|| _clickHandlerFilter(activated, button)) {
				ActivateClickHandler(guard, activated, button);
			}
			});*/
	}

	if (QGuiApplication::clipboard()->supportsSelection()
		&& !_selection.empty()) {
		QGuiApplication::clipboard()->setText(
			text(),
			QClipboard::Selection);
	}

	return state;
}

void FlatLabel::updateHover(const TextState& state) {
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
			if (state.afterSymbol && _selectionType == TextSelection::Type::Letters) {
				++second;
			}
			auto selection = adjustSelection({ qMin(second, _dragSymbol), qMax(second, _dragSymbol) }, _selectionType);
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

FlatLabel::TextState FlatLabel::getTextState(const QPoint& m) const {
	TextState state;
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
		if (uponSelected) {
			return _text.toTextForMimeData(_selection);
		}
		else if (pressedHandler) {
			return TextForMimeData::Simple(pressedHandler->dragText());
		}
		return TextForMimeData();
		}();

	if (auto mimeData = TextUtilities::MimeDataFromText(selectedText)) {
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
	auto request = ContextMenuRequest();

	request.menu = _contextMenu,
		request.selection = _selectable ? _selection : TextSelection(),
		request.uponSelection = uponSelection,
		request.fullSelection = _selectable && _selection.isFullSelection(text());

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
	if (request.uponSelection && !request.fullSelection)
		request.menu->addAction(
			style::flatLabel::phraseContextCopySelected,
			[=] { copySelectedText(); });
}

int FlatLabel::countTextWidth() const noexcept {
	const auto available = _allowedWidth
		? _allowedWidth
		: style::minimumMessageWidth;
	return available;
}

int FlatLabel::countTextHeight() {
	_fullTextHeight = style::TextSize(text(), font()).height();
	return _fullTextHeight;
}


void FlatLabel::refreshSize() {
	const auto textWidth = countTextWidth();
	const auto textHeight = countTextHeight();

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