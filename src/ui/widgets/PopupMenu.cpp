#include "PopupMenu.h"

#include <QMenu>


PopupMenu::PopupMenu(QWidget* parent):
	QWidget(parent)
	, _st(style::defaultPopupMenuStyle)
{

}

QSize PopupMenu::sizeHint() const {
	return size();
}

QSize PopupMenu::minimumSizeHint() const {
	return size();
}

void PopupMenu::addAction(
	const QString& title,
	Fn<void()> callback,
	const style::icon* icon)
{
	if (callback == nullptr)
		return;

	auto action = new Action(this);

	action->setText(title);


	connect(action, QAbstractButton::clicked, callback);

	
	_actions.push_back(callback);

}

PopupMenu::Action* PopupMenu::action(int index) const {
	return _actions.at(index);
}

void PopupMenu::setOpacity(float opacity) {
	_opacity = opacity;
	update();
}

float PopupMenu::opacity() const noexcept {
	return _opacity;
}

void PopupMenu::setStyle(const style::PopupMenu* style, bool repaint) {
	_st = style;
	update();
}

const style::PopupMenu* PopupMenu::style() const noexcept {
	return _st;
}

void PopupMenu::addSeparator() {

}

void PopupMenu::popup(const QPoint& point) {
	move(point);
	recountSize();
	setVisible(true);
}

void PopupMenu::paintEvent(QPaintEvent* event) {
	auto painter = QPainter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

	painter.setOpacity(_opacity);
}

void PopupMenu::focusOutEvent(QFocusEvent* event) {

}

void PopupMenu::focusInEvent(QFocusEvent* event) {

}

bool PopupMenu::event(QEvent* _event) {
	return QWidget::event(_event);
}

void PopupMenu::recountSize() {
	const auto fullWidth = _st->margin.left() + _st->margin.right() + 
}