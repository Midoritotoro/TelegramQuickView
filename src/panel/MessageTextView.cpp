#include "MessageTextView.h"

#include <QGridLayout>
#include <QApplication>
#include <QScreen>
#include <QPainterPath>
#include <QPainter>


namespace {
	constexpr QMargins textLabelMargins = { 8, 5, 8, 8 };
	constexpr int messageTextViewBorderRadius = 10;
}


MessageTextView::MessageTextView(QWidget* parent):
	QWidget(parent)
{
	setContentsMargins(0, 0, 0, 0);
	setAttribute(Qt::WA_TransparentForMouseEvents);
	setAttribute(Qt::WA_NoSystemBackground);
	setStyleSheet("QWidget {\n"
		"font-size: 13px;\n"
	"}");

	_textLabel = new QLabel(this);

	_textLabel->setWordWrap(true);
	_textLabel->setAlignment(Qt::AlignLeft);
	_textLabel->setContentsMargins(textLabelMargins);

	_textLabel->setAttribute(Qt::WA_NoSystemBackground);
	_textLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
	
	QGridLayout* gridLayout = new QGridLayout(this);
	gridLayout->setContentsMargins(0, 0, 0, 0);
	gridLayout->setSpacing(0);

	gridLayout->addWidget(_textLabel, 0, 0, 1, 1);
}

void MessageTextView::setText(const QString& text) {
	_textLabel->setText(text);

	_textLabel->adjustSize();
	adjustSize();
	_textLabel->resize(size());

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
	_textLabel->resize(size());
}

void MessageTextView::paintEvent(QPaintEvent* event) {
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

	painter.setBrush(Qt::black);
	painter.setPen(Qt::NoPen);
	painter.setOpacity(0.65);

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
}