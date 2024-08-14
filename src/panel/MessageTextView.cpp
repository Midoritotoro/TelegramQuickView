#include "MessageTextView.h"

#include <QGridLayout>


MessageTextView::MessageTextView(QWidget* parent):
	QWidget(parent)
{
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	setStyleSheet("QWidget {\n"
		"background-color: rgba(35, 36, 37, 90);\n"
		"border: 5px;\n"
		"border-radius: 5px;\n"
	"}");
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	_textLabel = new QLabel(this);
	_textLabel->setWordWrap(true);
	_textLabel->setAlignment(Qt::AlignLeft);
	_textLabel->setContentsMargins(8, 5, 8, 8);

	_textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	
	QGridLayout* gridLayout = new QGridLayout(this);
	gridLayout->addWidget(_textLabel, 0, 0, 1, 1, Qt::AlignCenter);
}

void MessageTextView::setText(const QString& text) {
	_textLabel->setText(text);
}