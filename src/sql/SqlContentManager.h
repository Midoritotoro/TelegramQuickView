#pragma once 

#include <QList>
#include <QUrl>
#include <QString>

typedef QList<QUrl> QUrlList;

typedef struct _TelegramMessage {
	QString sender;
	QString text;
	QString date;
	QUrlList attachments;
} TelegramMessage, *LPTelegramMessage;

typedef QList<TelegramMessage> TelegramMessagesList;

class SqlContentManager {
public:
	static TelegramMessagesList getLastMessages();
	static TelegramMessage getLastMessage();
};