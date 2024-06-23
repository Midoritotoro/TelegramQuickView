#include "MainWindow.h"

#include <QPushButton>
#include "../../core/winapi/FileIO.h"

MainWindow::MainWindow(QWidget* parent) :
	QWidget(parent)
{
	CreateMainWindow();
}

MainWindow::~MainWindow()
{
}

void MainWindow::CreateMainWindow()
{
	GridLayout = new QGridLayout(this);
	TelegramParserTargetLineEdit = new QLineEdit(this);
	channelOptionsMenuBar = new QMenuBar(this);
	mouseDetector = new MouseDetector();
	TelegramParserTextLabel = new QLabel("Телеграм каналы: ");

	channelOptionsMenu = channelOptionsMenuBar->addMenu("&Появление панели");
	LeftPartOfScreenAction = channelOptionsMenu->addAction("Левая часть экрана");
	RightPartOfScreenAction = channelOptionsMenu->addAction("Правая часть экрана");

	AddChannelsButton = new QPushButton("Добавить", this);
	ReplaceChannelsButton = new QPushButton("Заменить", this);
	GetChannelsFromFileButton = new QPushButton("Просмотреть каналы в списке", this);

	TelegramParserTargetLineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	AddChannelsButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	ReplaceChannelsButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	channelOptionsMenuBar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	GetChannelsFromFileButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	GridLayout->addWidget(channelOptionsMenuBar, 0,1, 1, 1);
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

void MainWindow::on_AddChannelsButton_click()
{
	DWORD Count = 0;
	std::string TelegramChannels = TelegramParserTargetLineEdit->text().toStdString();
	LPWSTR wstr = NULL;
	int wstrlength = 0;
	int wideCharCount = 0;
	WCHAR s[2] = TEXT(",");
	bool ChannelsSuccessfullyWriten;

	wideCharCount = MultiByteToWideChar(CP_ACP, 0, TelegramChannels.c_str(), -1, NULL, 0);
	wstr = new wchar_t[wideCharCount];
	MultiByteToWideChar(CP_ACP, 0, TelegramChannels.c_str(), -1, wstr, wideCharCount);
	wstrlength = lstrlen(wstr);

	if (wstr[wstrlength - 1] != *s) {
		wstr[wstrlength] = *s;
	}

	ChannelsSuccessfullyWriten = WriteChannelsToFile(wstr, false);
	delete[] wstr;
}

void MainWindow::on_ReplaceChannelsButton_click()
{
	DWORD Count = 0;
	std::string TelegramChannels = TelegramParserTargetLineEdit->text().toStdString();
	LPWSTR wstr = NULL;
	int wstrlength = 0;
	int wideCharCount = 0;
	WCHAR s[2] = TEXT(",");
	bool ChannelsSuccessfullyWriten;

	wideCharCount = MultiByteToWideChar(CP_ACP, 0, TelegramChannels.c_str(), -1, NULL, 0);
	wstr = new wchar_t[wideCharCount];
	MultiByteToWideChar(CP_ACP, 0, TelegramChannels.c_str(), -1, wstr, wideCharCount);
	wstrlength = lstrlen(wstr);

	if (wstr[wstrlength - 1] != *s) {
		wstr[wstrlength] = *s;
	}

	ChannelsSuccessfullyWriten = WriteChannelsToFile(wstr, true);
	delete[] wstr;
}

void MainWindow::on_GetChannelsFromFileButton_click()
{
	char* channels = ReadChannelsFromFile();
	ReadedChannelsTextEdit = new QTextEdit(channels);
	DialogWindow = new QDialog();
	DialogWindow->setLayout(new QVBoxLayout);
	DialogWindow->layout()->addWidget(ReadedChannelsTextEdit);
	DialogWindow->exec();

}

void MainWindow::LeftPartOfScreenAction_triggered()
{
	BOOL mouseDetectorDirection = mouseDetector->isRunning();
	if (mouseDetectorDirection) {
		mouseDetector->KillThread();
		delete mouseDetector;
		mouseDetector = new MouseDetector();
	}
	mouseDetector->TrackMouse(MouseDetector::Direction::Left);
}

void MainWindow::RightPartOfScreenAction_triggered()
{
	BOOL mouseDetectorDirection = mouseDetector->isRunning();
	if (mouseDetectorDirection) {
		mouseDetector->KillThread();
		delete mouseDetector;
		mouseDetector = new MouseDetector();
	}
	mouseDetector->TrackMouse(MouseDetector::Direction::Right);
}
