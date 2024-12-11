#include "FlatLabel.h"

#include <QString>
#include <QMouseEvent>

#include <QDesktopServices>

#include <QPainter>
#include "../core/StyleCore.h"


FlatLabel::FlatLabel(QWidget* parent):
	QWidget(parent) 
{
	setSelectable(true);
	setMouseTracking(true);
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

void FlatLabel::setContextMenu(QMenu* menu) {
	_contextMenu = menu;
}

QMenu* FlatLabel::contextMenu() const noexcept {
	return _contextMenu;
}

void FlatLabel::setLink(quint16 index) {

}

void FlatLabel::paintEvent(QPaintEvent* e) {
	auto painter = QPainter(this);

	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	painter.setOpacity(_opacity);

	painter.setPen(Qt::white);

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

	const auto lineHeight = qMax(_st.style.lineHeight, QFontMetrics(font()).height());
	const auto elisionHeight = !renderElided
		? 0
		: _st.maxHeight
		? qMax(_st.maxHeight, lineHeight)
		: height();
	const auto paused = _animationsPausedCallback
		? _animationsPausedCallback()
		: WhichAnimationsPaused::None;
	_text.draw(p, {
		.position = { textLeft, _st.margin.top() },
		.availableWidth = textWidth,
		.align = _st.align,
		.clip = e->rect(),
		.palette = &_st.palette,
		.spoiler = Text::DefaultSpoilerCache(),
		.now = crl::now(),
		.pausedEmoji = (paused == WhichAnimationsPaused::CustomEmoji
			|| paused == WhichAnimationsPaused::All),
		.pausedSpoiler = (paused == WhichAnimationsPaused::Spoiler
			|| paused == WhichAnimationsPaused::All),
		.selection = selection,
		.elisionHeight = elisionHeight,
		.elisionBreakEverywhere = renderElided && _breakEverywhere,
		});
}

void FlatLabel::mouseMoveEvent(QMouseEvent* e) {

}

void FlatLabel::mousePressEvent(QMouseEvent* e) {

}

void FlatLabel::mouseReleaseEvent(QMouseEvent* e) {

}

void FlatLabel::mouseDoubleClickEvent(QMouseEvent* e) {

}

void FlatLabel::focusOutEvent(QFocusEvent* e) {

}

void FlatLabel::focusInEvent(QFocusEvent* e) {

}

void FlatLabel::keyPressEvent(QKeyEvent* e) {

}

void FlatLabel::contextMenuEvent(QContextMenuEvent* e) {

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