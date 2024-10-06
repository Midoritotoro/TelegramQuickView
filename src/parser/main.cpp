#include <QApplication>
#include "AuthenticationDialog.h"


int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	AuthenticationDialog dialog;

	dialog.show();
	return app.exec();
}