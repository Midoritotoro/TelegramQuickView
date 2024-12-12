#include "FlatLabel.h"

#include <QMouseEvent>
#include <QDesktopServices>

#include <QPainter>
#include <QClipboard>


FlatLabel::FlatLabel(QWidget* parent):
	QWidget(parent) 
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

}

void FlatLabel::mousePressEvent(QMouseEvent* event) {

}

void FlatLabel::mouseReleaseEvent(QMouseEvent* event) {

}

void FlatLabel::mouseDoubleClickEvent(QMouseEvent* event) {

}

void FlatLabel::focusOutEvent(QFocusEvent* event) {
	if (!_selection.empty()) {
		if (_contextMenu) {
			_savedSelection = _selection;
		}
		_selection = { 0, 0 };
		update();
	}
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

	//showContextMenu(event, ContextMenuReason::FromEvent);
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

void FlatLabel::textUpdated() {
	refreshSize();
	setMouseTracking(_selectable || hasLinks());
	update();
}
