#include "MessageTextView.h"

#include <QGridLayout>
#include <QApplication>
#include <QScreen>
#include <QPainterPath>
#include <QPainter>


namespace {
	constexpr int messageTextViewBorderRadius = 10;
}


MessageTextView::MessageTextView(QWidget* parent):
	QWidget(parent)
{
	setStyleSheet("QWidget {\n"
		//"background-color: rgba(41, 41, 54, 99);\n"
		"border: none;\n"
		"font-size: 13px;\n"
	"}");

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setContentsMargins(0, 0, 0, 0);

	_textLabel = new QLabel(this);

	_textLabel->setWordWrap(true);
	_textLabel->setAlignment(Qt::AlignLeft);
	_textLabel->setContentsMargins(8, 5, 8, 8);

	_textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	
	QGridLayout* gridLayout = new QGridLayout(this);
	gridLayout->setContentsMargins(0, 0, 0, 0);
	gridLayout->setSpacing(0);

	gridLayout->addWidget(_textLabel, 0, 0, 1, 1);
}

void MessageTextView::setText(const QString& text) {
	_textLabel->setText(text);

	_textLabel->adjustSize();
	adjustSize();

	const auto screenHeight = QApplication::primaryScreen()->availableGeometry().height();
	const auto screenWidth = QApplication::primaryScreen()->availableGeometry().width();

	if (height() >= screenHeight / 5) {
		_textLabel->setMinimumWidth(screenWidth * 0.8);
		setMinimumWidth(screenWidth * 0.8);
	}
	else { 
		_textLabel->setMinimumWidth(0);
		setMinimumWidth(0);
	}

	_textLabel->adjustSize();
	adjustSize();
}

void MessageTextView::paintEvent(QPaintEvent* event) {
	QWidget::paintEvent(event);

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	painter.setBrush(Qt::black);
	painter.setPen(Qt::NoPen);

	drawRoundedCorners(painter, messageTextViewBorderRadius);
}

void MessageTextView::drawRoundedCorners(QPainter& painter, int borderRadius) {
	QPainterPath path;

	path.moveTo(borderRadius, 0);

	path.lineTo(width() - borderRadius, 0);
	path.quadTo(width(), 0, width(), borderRadius);

	path.lineTo(width(), height() - borderRadius);
	path.quadTo(width(), height(), width() - borderRadius, height());

	path.lineTo(borderRadius, height());
	path.quadTo(0, height(), 0, height() - borderRadius);

	path.lineTo(0, borderRadius);
	path.quadTo(0, 0, borderRadius, 0);

	painter.drawPath(path);

	QTransform mirror(-1, 0, 0, 0, 1, 0, 0, 0, 1);

	painter.setTransform(mirror);
	painter.translate(-width(), 0);

	painter.drawPath(path);
}