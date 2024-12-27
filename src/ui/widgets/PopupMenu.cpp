#include "PopupMenu.h"


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

void PopupMenu::setAction(const QAction& action) {

}

QAction PopupMenu::action(int index) const {

}

void PopupMenu::setOpacity(float opacity) {

}

float PopupMenu::opacity() const {

}

void PopupMenu::setStyle(const style::PopupMenu* style, bool repaint) {

}

const style::PopupMenu* PopupMenu::style() const {
	return _st;
}

void PopupMenu::paintEvent(QPaintEvent* event) {

}

void PopupMenu::mouseMoveEvent(QMouseEvent* event) {

}

void PopupMenu::mousePressEvent(QMouseEvent* event) {

}

void PopupMenu::mouseReleaseEvent(QMouseEvent* event) {

}

void PopupMenu::mouseDoubleClickEvent(QMouseEvent* event) {

}

void PopupMenu::focusOutEvent(QFocusEvent* event) {

}

void PopupMenu::focusInEvent(QFocusEvent* event) {

}

void PopupMenu::keyPressEvent(QKeyEvent* event) {

}

bool PopupMenu::event(QEvent* _event) {

}