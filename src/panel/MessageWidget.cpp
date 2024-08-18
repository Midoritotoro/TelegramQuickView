#include "MessageWidget.h"
#include "MessageAttachment.h"

#include <QElapsedTimer>


MessageWidget::MessageWidget(
	const QString& author,
	QWidget* parent
):
QWidget(parent)
, _mediaDisplayMode(MessageMediaDisplayMode::Stack)
{
	_telegramMessage = new TelegramMessage();
	_telegramMessage->author = author;

	_messageLayout = new QGridLayout(this);
	setContentsMargins(0, 0, 0, 0);
	_messageLayout->setSpacing(0);
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

	textLabel->setText(text);
	_telegramMessage->text = text;

	textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	textLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

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
	if (attachmentsPaths.length() < 1)
		return;

	QElapsedTimer timer;
	switch (_mediaDisplayMode) {
	case MessageMediaDisplayMode::PreviewWithCount:
		timer.start();

		foreach(const QUrl & url, attachmentsPaths) {
			QString sourcePath;

			if (url.path().at(0) == "/"[0])
				sourcePath = url.path().remove(0, 1);
			else
				sourcePath = url.path();
			QElapsedTimer timer2;
			timer2.start();

			MessageAttachment* messageAttachment = new MessageAttachment(sourcePath, maximumMessageWidth, this);
			qDebug() << "messageAttachment init: " << static_cast<double>(timer2.elapsed()) / 1000 << " s";
			if (_messageLayout->rowCount() <= 1)
				_messageLayout->addWidget(messageAttachment, _messageLayout->rowCount(), 0, 1, 1);
			_telegramMessage->attachments.append(messageAttachment);

		}
		qDebug() << "PreviewWithCount: " << static_cast<double>(timer.elapsed()) / 1000 << " s";
		break;
	case MessageMediaDisplayMode::Stack:
		timer.start();

		foreach(const QUrl & url, attachmentsPaths) {
			QString sourcePath;

			if (url.path().at(0) == "/"[0])
				sourcePath = url.path().remove(0, 1);
			else
				sourcePath = url.path();

			MessageAttachment* messageAttachment = new MessageAttachment(sourcePath, maximumMessageWidth, this);
			_messageLayout->addWidget(messageAttachment);

			_telegramMessage->attachments.append(messageAttachment);
		}
		qDebug() << "PreviewWithCount: " << static_cast<double>(timer.elapsed()) / 1000 << " s";
		break;
	}
}

void MessageWidget::setMessageMediaDisplayMode(MessageMediaDisplayMode displayMode) {
	_mediaDisplayMode = displayMode;
}

MessageWidget::MessageMediaDisplayMode MessageWidget::messsageMediaDisplayMode() const noexcept {
	return _mediaDisplayMode;
}

QString MessageWidget::messageText() const noexcept {
	return _telegramMessage->text;	
}

const MessageAttachmentsList& MessageWidget::messageAttachments() const noexcept {
	return _telegramMessage->attachments;
}

int MessageWidget::indexOfAttachment(MessageAttachment* messageAttachment) const noexcept {
	return _telegramMessage->attachments.indexOf(messageAttachment);
}

MessageAttachment* MessageWidget::attachmentAt(int index) const noexcept {
	if (index >= 0 && attachmentsLength() > index)
		return _telegramMessage->attachments.at(index);
	return nullptr;
}

int MessageWidget::attachmentsLength() const noexcept {
	return _telegramMessage->attachments.length();
}

bool MessageWidget::hasAttachments() const noexcept {
	return !_telegramMessage->attachments.isEmpty();
}

bool MessageWidget::hasText() const noexcept {
	return !_telegramMessage->text.isEmpty();
}