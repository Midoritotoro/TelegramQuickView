#include "MessageTextView.h"

#include <QGridLayout>
#include <QApplication>
#include <QScreen>


MessageTextView::MessageTextView(QWidget* parent):
	QWidget(parent)
{

	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	setStyleSheet("QWidget {\n"
		"background-color: rgba(41, 41, 54, 99);\n"
		"border: 7px;\n"
		"border-radius: 5px;\n"
	"}");
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setContentsMargins(0, 0, 0, 0);

	_textLabel = new QLabel(this);
	_textLabel->setWordWrap(true);
	_textLabel->setAlignment(Qt::AlignLeft);
	_textLabel->setContentsMargins(8, 5, 8, 8);
	_textLabel->setStyleSheet("QLabel{\n"
		"font-size: 13px;\n"
	"}");

	_textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	
	QGridLayout* gridLayout = new QGridLayout(this);
	gridLayout->setContentsMargins(0, 0, 0, 0);
	gridLayout->setSpacing(0);

	gridLayout->addWidget(_textLabel, 0, 0, 1, 1, Qt::AlignCenter);
}

void MessageTextView::setText(const QString& text) {
	_textLabel->setText(text);
	_textLabel->adjustSize();

	const int screenHeight = QApplication::primaryScreen()->availableGeometry().height();
	const int screenWidth = QApplication::primaryScreen()->availableGeometry().width();

	if (_textLabel->height() >= screenHeight / 4) {
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