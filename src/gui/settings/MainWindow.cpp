#include "MainWindow.h"

#include <QPushButton>
#include <QFile>
#include <QRegularExpression>
#include <QJsonArray>


MainWindow::MainWindow(QWidget* parent) :
	QWidget(parent) 
{
	QGridLayout* GridLayout = new QGridLayout(this);
	TelegramParserTargetLineEdit = new QLineEdit(this);
	QMenuBar* channelOptionsMenuBar = new QMenuBar(this);
	mouseDetector = new MouseDetector();
	QLabel* TelegramParserTextLabel = new QLabel("Телеграм каналы: ");

	QMenu* channelOptionsMenu = channelOptionsMenuBar->addMenu("&Появление панели");
	QAction* LeftPartOfScreenAction = channelOptionsMenu->addAction("Левая часть экрана");
	QAction* RightPartOfScreenAction = channelOptionsMenu->addAction("Правая часть экрана");

	QPushButton* AddChannelsButton = new QPushButton("Добавить", this);
	QPushButton* ReplaceChannelsButton = new QPushButton("Заменить", this);
	QPushButton* GetChannelsFromFileButton = new QPushButton("Просмотреть каналы в списке", this);
	QPushButton* AuthenticationButtton = new QPushButton("");

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

QByteArray& MainWindow::getUserData() {
	QFile jsonFile;
	jsonFile.setFileName("userData.json");
	jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
	QByteArray data = jsonFile.readAll();
	jsonFile.close();
	return data;
}

QJsonDocument& MainWindow::getJsonDocument() {
	QByteArray& jsonData = getUserData();
	QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData);
	return jsonDocument;
}

void MainWindow::clearChannelsJsonArray() {
	QJsonDocument jsonDocument = getJsonDocument();
	QJsonArray jsonArray = jsonDocument.array();

	for (int index = 0; index < jsonArray.count(); index++)
		jsonArray.removeAt(index);

	jsonDocument.setArray(jsonArray);
}

void MainWindow::saveUserData(QString& apiHash, QString& phoneNumber, QString& apiId) {
	QJsonDocument jsonDocument = getJsonDocument();
	QJsonObject jsonObject = jsonDocument.object();

	jsonObject.insert("apiHash", apiHash);
	jsonObject.insert("phoneNumber", phoneNumber);
	jsonObject.insert("apiId", apiId);

	jsonDocument.setObject(jsonObject);
}

void MainWindow::saveTargetChannels(QStringList channels) {
	QJsonDocument jsonDocument = getJsonDocument();
	QJsonObject jsonObject = jsonDocument.object();
	QJsonArray jsonArray = jsonDocument.array();

	foreach (const QString& channel, channels)
		jsonArray.append(channel);

	jsonObject.insert("channels", jsonArray);
	jsonDocument.setObject(jsonObject);
}


void MainWindow::on_AddChannelsButton_click() {
	QRegularExpression channelsSplitRegularExpression("(\\ ,|\\,)");
	QString TelegramChannels = TelegramParserTargetLineEdit->text();
	QStringList TelegramChannelsList = TelegramChannels.split(channelsSplitRegularExpression);

	saveTargetChannels(TelegramChannelsList);

	TelegramParserTargetLineEdit->clear();
}

void MainWindow::on_ReplaceChannelsButton_click() {
	QRegularExpression channelsSplitRegularExpression("(\\ ,|\\,)");
	QString TelegramChannels = TelegramParserTargetLineEdit->text();
	QStringList TelegramChannelsList = TelegramChannels.split(channelsSplitRegularExpression);

	saveTargetChannels(TelegramChannelsList);
	
	TelegramParserTargetLineEdit->clear();
}

void MainWindow::on_GetChannelsFromFileButton_click() {
	QJsonDocument jsonDocument = getJsonDocument();
	QJsonObject jsonObject = jsonDocument.object();
	QJsonValue jsonValue = jsonObject.value("channels");
	
	//QTextEdit* ReadedChannelsTextEdit = new QTextEdit(jsonArray);
	//QDialog* DialogWindow = new QDialog();
	//DialogWindow->setLayout(new QVBoxLayout);
	//DialogWindow->layout()->addWidget(ReadedChannelsTextEdit);
	//DialogWindow->exec();
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
