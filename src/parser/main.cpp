#include <QApplication>
#include "TelegramParser.h"


int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	TelegramParser parser;

	/*AuthenticationDialog dialog;

	dialog.show();*/

	return app.exec();
}