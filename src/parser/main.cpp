#include <QApplication>
#include "AuthenticationDialog.h"


int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	AuthenticationDialog* dialog = new AuthenticationDialog();

	if (dialog->isTelegramCredentialsValid() == true && dialog->isAuthCodeAccepted() == false) {
		dialog->skipFirstAuthorizationStage();
		dialog->exec();
		dialog->show();
	}
	else if (dialog->isTelegramCredentialsValid() == false) {
		dialog->exec();
		dialog->show();
	}

	return app.exec();
}