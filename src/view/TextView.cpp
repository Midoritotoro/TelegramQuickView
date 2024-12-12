#include "MessageTextView.h"

#include <QGridLayout>
#include <QApplication>

#include <QScreen>
#include <QPainter>

#include "../core/StyleCore.h"


TextView::TextView(QWidget* parent):
	QWidget(parent)
{
	setContentsMargins(0, 0, 0, 0);

	setAttribute(Qt::WA_TransparentForMouseEvents);
	setAttribute(Qt::WA_NoSystemBackground);

	_textLabel = new QLabel(this);

	_textLabel->setWordWrap(true);
	_textLabel->setAlignment(Qt::AlignLeft);
	_textLabel->setContentsMargins(style::messageTextViewMargins);

	_textLabel->setAttribute(Qt::WA_NoSystemBackground);
	_textLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
	
	QGridLayout* gridLayout = new QGridLayout(this);
	gridLayout->setContentsMargins(0, 0, 0, 0);
	gridLayout->setSpacing(0);

	gridLayout->addWidget(_textLabel, 0, 0, 1, 1);
}

void TextView::setText(const QString& text) {
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

void TextView::paintEvent(QPaintEvent* event) {
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

	painter.setBrush(Qt::black);
	painter.setPen(Qt::NoPen);
	painter.setOpacity(0.65);

	style::RoundCorners(painter, size(), style::messageTextViewBorderRadius);

	painter.drawRect(rect());
}