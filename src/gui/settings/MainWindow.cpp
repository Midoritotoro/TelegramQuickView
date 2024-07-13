#include "MainWindow.h"

#include <QDialog>
#include <QFile>
#include <QGridLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QMenuBar>
#include <QPushButton>
#include <QRegularExpression>
#include <QTextEdit>

#include "AuthenticationDialog.h"

// Добавление | Замена | Просмотр каналов, новости из которых будут парситься
// Сторона появления панели с контентом каналов
// Количество последних постов, которые нужно брать из каналов


MainWindow::MainWindow(QWidget* parent) :
	QWidget(parent)
{
	AuthenticationDialog* userAuthenticationDialog = new AuthenticationDialog();

	//Py_InitializeEx(0);

	const char* pythonFilePath = "D:\\TelegramQuickView\\src\\gui\\settings\\";
	//PyGILState_STATE state = PyGILState_Ensure();

	//PyObject* PySys = PyImport_ImportModule("sys");
	//PyObject* PyPath = PyObject_GetAttrString(PySys, "path");
	//PyList_Append(PyPath, PyUnicode_FromString(pythonFilePath));

	//DWORD result1 = UserDataManager::isTelegramCredentialsValid();
	//DWORD result2 = UserDataManager::isTelegramPhoneNumberCodeValid();
	//qDebug() << "UserDataManager::isTelegramCredentialsValid() - " << result1;
	//qDebug() << "UserDataManager::isTelegramPhoneNumberCodeValid() - " << result2;

	//if (result1 == 1 && result2 == 0) {
	//	userAuthenticationDialog->skipFirstAuthorizationStage();
	//	userAuthenticationDialog->exec();
	//}
	//else if (result1 == 0) {
	//	userAuthenticationDialog->exec();
	//}

	//Py_DECREF(PyPath);
	//Py_DECREF(PySys);

	//PyGILState_Release(state);

	//Py_FinalizeEx();
	UserDataManager::setLastPostsCountForChannels(3);

	QGridLayout* GridLayout = new QGridLayout(this);
	TelegramParserTargetLineEdit = new QLineEdit(this);
	QMenuBar* channelOptionsMenuBar = new QMenuBar(this);
	mouseDetector = new MouseDetector();
	QLabel* TelegramParserTextLabel = new QLabel("Телеграм каналы: ");

	QMenu* channelOptionsMenu = channelOptionsMenuBar->addMenu("Появление панели");
	QAction* LeftPartOfScreenAction = channelOptionsMenu->addAction("Левая часть экрана");
	QAction* RightPartOfScreenAction = channelOptionsMenu->addAction("Правая часть экрана");

	QPushButton* AddChannelsButton = new QPushButton("Добавить", this);
	QPushButton* ReplaceChannelsButton = new QPushButton("Заменить", this);
	QPushButton* GetChannelsFromFileButton = new QPushButton("Просмотреть каналы в списке", this);

	TelegramParserTargetLineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	AddChannelsButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	ReplaceChannelsButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	channelOptionsMenuBar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	GetChannelsFromFileButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	GridLayout->addWidget(channelOptionsMenuBar, 0, 1, 1, 1);
	GridLayout->addWidget(TelegramParserTextLabel, 1, 0, 1, 1);
	GridLayout->addWidget(TelegramParserTargetLineEdit, 1, 1, 1, 1);
	GridLayout->addWidget(AddChannelsButton, 1, 2, 1, 1);
	GridLayout->addWidget(ReplaceChannelsButton, 1, 3, 1, 1);
	GridLayout->addWidget(GetChannelsFromFileButton, 1, 4, 1, 1);

	TelegramParserTextLabel->setAlignment(Qt::AlignCenter);
	GridLayout->setAlignment(Qt::AlignCenter);
	connect(LeftPartOfScreenAction, SIGNAL(triggered()), this, SLOT(LeftPartOfScreenAction_triggered()));
	connect(RightPartOfScreenAction, SIGNAL(triggered()), this, SLOT(RightPartOfScreenAction_triggered()));

	connect(AddChannelsButton, SIGNAL(clicked()), this, SLOT(on_AddChannelsButton_click()));
	connect(ReplaceChannelsButton, SIGNAL(clicked()), this, SLOT(on_ReplaceChannelsButton_click()));
	connect(GetChannelsFromFileButton, SIGNAL(clicked()), this, SLOT(on_GetChannelsFromFileButton_click()));
}

void MainWindow::on_AddChannelsButton_click() {
	QRegularExpression channelsSplitRegularExpression("(\\ ,|\\,)");
	QString TelegramChannels = TelegramParserTargetLineEdit->text();
	QStringList TelegramChannelsList = TelegramChannels.split(channelsSplitRegularExpression);

	UserDataManager::setTargetChannels(TelegramChannelsList);

	TelegramParserTargetLineEdit->clear();
}

void MainWindow::on_ReplaceChannelsButton_click() {
	QRegularExpression channelsSplitRegularExpression("(\\ ,|\\,)");
	QString TelegramChannels = TelegramParserTargetLineEdit->text();
	QStringList TelegramChannelsList = TelegramChannels.split(channelsSplitRegularExpression);

	UserDataManager::clearChannelsJsonArray();
	UserDataManager::setTargetChannels(TelegramChannelsList);

	TelegramParserTargetLineEdit->clear();
}

void MainWindow::on_GetChannelsFromFileButton_click() {
	QJsonDocument jsonDocument = UserDataManager::getJsonDocument();
	QJsonObject jsonObject = jsonDocument.object();

	if (!jsonObject.value("channels").toArray().isEmpty()) {
		QTextEdit* ReadedChannelsTextEdit = new QTextEdit();
		ReadedChannelsTextEdit->setReadOnly(true);
		QString channels = QString::fromUtf8(QJsonDocument(jsonObject.value("channels").toArray()).toJson());
		ReadedChannelsTextEdit->setPlainText(channels);
		QDialog* DialogWindow = new QDialog();
		DialogWindow->setLayout(new QVBoxLayout);
		DialogWindow->layout()->addWidget(ReadedChannelsTextEdit);
		DialogWindow->exec();
	}
}

void MainWindow::LeftPartOfScreenAction_triggered() {
	BOOL mouseDetectorDirection = mouseDetector->isRunning();
	if (mouseDetectorDirection) {
		mouseDetector->KillThread();
		delete mouseDetector;
		mouseDetector = new MouseDetector();
	}
	mouseDetector->TrackMouse(MouseDetector::Direction::Left);
}

void MainWindow::RightPartOfScreenAction_triggered() {
	BOOL mouseDetectorDirection = mouseDetector->isRunning();
	if (mouseDetectorDirection) {
		mouseDetector->KillThread();
		delete mouseDetector;
		mouseDetector = new MouseDetector();
	}
	mouseDetector->TrackMouse(MouseDetector::Direction::Right);
}