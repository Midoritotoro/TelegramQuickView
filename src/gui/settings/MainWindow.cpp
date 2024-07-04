#include "MainWindow.h"

#include <QPushButton>
#include <QRegularExpression>
#include <QJsonArray>
#include <QDir>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget* parent) :
	QWidget(parent)
{
	const QString fileName = getUserSettingsPath();
	jsonFile.setFileName(fileName);
	jsonFile.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);

	saveLastPostsCountForChannels(3);

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

const QString MainWindow::getUserSettingsPath()
{
	const QDir writeDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	if (!writeDir.mkpath("."))
		return "";
	const QString fileName = writeDir.absolutePath() + "\\userData.json";
	return fileName;
}

QJsonDocument MainWindow::getJsonDocument() {
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
	jsonDocument.setObject(jsonObject);

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

	jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
	jsonFile.write(jsonDocument.toJson());
	jsonFile.close();
}

void MainWindow::saveTargetChannels(QStringList channels) {
	QJsonArray jsonArray;
	QJsonDocument jsonDocument = getJsonDocument();
	QJsonObject currentDocumentObject = jsonDocument.object();

	if (!currentDocumentObject.value("channels").toArray().isEmpty())
		jsonArray = currentDocumentObject.value("channels").toArray();

	clearChannelsJsonArray();

	foreach(const QString& channel, channels)
		jsonArray.append(channel);

	currentDocumentObject.insert("channels", jsonArray);
	jsonDocument.setObject(currentDocumentObject);

	jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
	jsonFile.write(jsonDocument.toJson());
	jsonFile.close();
}

void MainWindow::saveLastPostsCountForChannels(int lastPostsCount) {
	QJsonObject jsonObject;
	QJsonDocument jsonDocument = getJsonDocument();

	jsonObject = jsonDocument.object();

	jsonObject.insert("lastPostsCount", lastPostsCount);
	jsonDocument.setObject(jsonObject);

	jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
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
	QJsonDocument jsonDocument = getJsonDocument();
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