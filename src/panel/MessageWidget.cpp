﻿#include "MessageWidget.h"
#include "MessageMediaViewer.h"


MessageWidget::MessageWidget(QWidget* parent) :
	QWidget(parent)
{
	_messageLayout = new QGridLayout(this);
	setContentsMargins(0, 0, 0, 0);
	_messageLayout->setVerticalSpacing(0);
	_messageLayout->setContentsMargins(0, 0, 0, 0);

	setStyleSheet("QWidget{\n"
		"background: rgb(24, 37, 51);\n"
		"border: 5px;\n"
	"}");

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setMouseTracking(true);
}

void MessageWidget::addMessageText(const QString& text) {
	if (text.length() == 0)
		return;

	QLabel* textLabel = new QLabel();

	textLabel->setWordWrap(true);
	textLabel->setAlignment(Qt::AlignLeft);
	textLabel->setContentsMargins(8, 5, 20, 8);

	textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	textLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
	textLabel->setText(text);
	_messageText = text;

	if (_messageLayout->rowCount() > 1) // У сообщения есть вложение
		textLabel->setStyleSheet("QLabel{\n"
			"color: white;\n"
			"border-bottom-left-radius: 10px;\n"
			"border-bottom-right-radius: 10px;\n"
		"}");
	else
		textLabel->setStyleSheet("QLabel{\n"
			"color: white;\n"
			"border-radius: 10px;\n"
		"}");


	_messageLayout->addWidget(textLabel, _messageLayout->rowCount(), 0, 1, 1);
}

void MessageWidget::addMessageAttachments(const QUrlList& attachmentsPaths, int maximumMessageWidth) {
	foreach(const QUrl& url, attachmentsPaths) {
		QString sourcePath;

		if (url.path().at(0) == "/"[0])
			sourcePath = url.path().remove(0, 1);
		else
			sourcePath = url.path();

		MessageAttachment* messageAttachment = new MessageAttachment(sourcePath, maximumMessageWidth);
		_messageLayout->addWidget(messageAttachment, _messageLayout->rowCount(), 0, 1, 1);
		messageAttachment->setParentMessage(this);
		
		_messageAttachmentsList.append(messageAttachment);
	}
}

QString MessageWidget::getMessageText() const {
	return _messageText;	
}

const MessageAttachmentsList& MessageWidget::getMessageAttachments() const {
	return _messageAttachmentsList;
}

int MessageWidget::indexOfAttachment(MessageAttachment* messageAttachment) {
	return _messageAttachmentsList.indexOf(messageAttachment);
}

MessageAttachment* MessageWidget::attachmentAt(int index) {
	return _messageAttachmentsList.at(index);
}