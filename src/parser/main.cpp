#include <QApplication>
#include "TelegramParser.h"


int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	AbstractTelegramParser parser;

	return app.exec();
}