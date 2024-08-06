#include "MessageWidget.h"


QWidget* MessageWidget::createMessageWidget() {
	QWidget* messageWidget = new QWidget();
	QGridLayout* messageLayout = new QGridLayout(messageWidget);

	messageWidget->setContentsMargins(0, 0, 0, 0);
	messageLayout->setContentsMargins(0, 0, 0, 0);

	messageWidget->setStyleSheet("QWidget{\n"
		"background: rgb(24, 37, 51);\n"
		"border: 5px;\n"
		"border-radius: 10px;\n"
	"}");

	messageWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	messageWidget->setMouseTracking(true);

	return messageWidget;
}

QLabel* MessageWidget::createMessageTextLabel() {
	QLabel* textLabel = new QLabel();

	textLabel->setStyleSheet("QLabel{\n"
		"background: transparent;\n"
		"color: white;\n"
	"}");

	textLabel->setAttribute(Qt::WA_NoSystemBackground);
	textLabel->setWordWrap(true);
	textLabel->setAlignment(Qt::AlignLeft);
	textLabel->setContentsMargins(8, 0, 20, 8);

	textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	textLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

	return textLabel;
}
