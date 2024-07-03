#include "MainWindow.h"

#include <QPushButton>
#include <QRegularExpression>
#include <QJsonArray>
#include <QDir>
#include <QStandardPaths>

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
	GetChannelsFromFileButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed); // dwad/ddas. , dwad/ddas., warthudnder efesf sefs Ffef 432Q Q-8 , rr3, tg, saef@$78

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

	const QDir writeDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

	if (!writeDir.mkpath("."))
		return;

	const QString fileName = writeDir.absolutePath() + "\\userData.json";

	jsonFile.setFileName(fileName);
	jsonFile.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);
}

QJsonDocument& MainWindow::getJsonDocument() {
	QJsonDocument jsonDocument;
	QByteArray jsonData;

	jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
	jsonData = jsonFile.readAll();
	jsonFile.close();
	
	jsonDocument = QJsonDocument::fromJson(jsonData);
	return jsonDocument;
}

void MainWindow::clearChannelsJsonArray() {
	QJsonObject jsonObject;
	QJsonArray jsonArray;
	QJsonDocument jsonDocument = getJsonDocument();

	jsonObject = jsonDocument.object();

	if (jsonObject.value("channels").isNull())
		return;

	jsonObject.remove("channels");

	jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
	jsonFile.write(jsonDocument.toJson());
	jsonFile.close();
}

void MainWindow::saveUserData(QString& apiHash, QString& phoneNumber, QString& apiId) {
	QJsonObject jsonObject;

	QJsonDocument jsonDocument = getJsonDocument();

	jsonObject.insert("apiHash", apiHash);
	jsonObject.insert("phoneNumber", phoneNumber);
	jsonObject.insert("apiId", apiId);

	jsonDocument.setObject(jsonObject);

	jsonFile.open(QIODevice::WriteOnly| QIODevice::Text);
	jsonFile.write(jsonDocument.toJson());
	jsonFile.close();
}

void MainWindow::saveTargetChannels(QStringList channels) {
	QJsonObject jsonObject;
	QJsonArray jsonArray;

	QJsonDocument jsonDocument = getJsonDocument();

	QJsonArray currentDocumentArray = jsonDocument.array();

	foreach(const QJsonValue & channel, currentDocumentArray)
		jsonArray.append(channel);

	clearChannelsJsonArray();
	
	foreach (const QString& channel, channels)
		jsonArray.append(channel);

	jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
	jsonObject.insert("channels", jsonArray);
	jsonDocument.setObject(jsonObject);
	jsonFile.write(jsonDocument.toJson());
	jsonFile.close();
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

	clearChannelsJsonArray();
	saveTargetChannels(TelegramChannelsList);
	
	TelegramParserTargetLineEdit->clear();
}

void MainWindow::on_GetChannelsFromFileButton_click() {

	
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
