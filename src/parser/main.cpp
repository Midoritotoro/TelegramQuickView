#include <QApplication>
#include "AuthenticationDialog.h"

#include "TelegramParser.h"


int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	TelegramParser parser;
	
	return app.exec();
}