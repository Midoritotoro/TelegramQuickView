#include <QApplication>
#include "TelegramParser.h"


int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	//TelegramParser parser;
	PostSqlManager p;

	TelegramMessage message;
	message.attachment = "att";
	message.date = "2024";
	message.sender = "sender";
	message.mediaAlbumId = 10;
	message.text = "Message text";

	p.writeMessageInfo(message);

	return app.exec();
}