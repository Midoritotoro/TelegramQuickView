#include "MessageMediaViewer.h"

#include <QToolButton>
#include <QDir>
#include <QFile>
#include <QApplication>
#include <QStyle>


MessageMediaViewer::MessageMediaViewer(QWidget* parent):
	QWidget(parent)
{
	QToolButton* minimizeWindowButton = new QToolButton();
	QToolButton* closeWindowButton = new QToolButton();

	QString currentPath = QCoreApplication::applicationDirPath();
	QDir cssDir(currentPath + "/../../src/css");

	QString toolButtonStylePath = cssDir.absolutePath() + "/ToolButtonStyle.css";

	QPixmap minPix = style()->standardPixmap(QStyle::SP_TitleBarMinButton);
	QPixmap closePix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton);

	minimizeWindowButton->setIcon(minPix);
	closeWindowButton->setIcon(closePix);

	minimizeWindowButton->setAttribute(Qt::WA_NoSystemBackground);
	closeWindowButton->setAttribute(Qt::WA_NoSystemBackground);

	QFile toolButtonStyleFile(toolButtonStylePath);
	if (toolButtonStyleFile.open(QFile::ReadOnly)) {

		QByteArray toolButtonStyle = toolButtonStyleFile.readAll();

		closeWindowButton->setStyleSheet(toolButtonStyle);
		minimizeWindowButton->setStyleSheet(toolButtonStyle);

		toolButtonStyleFile.close();
	}

	_grid = new QGridLayout(this);

	QWidget* toolWidget = new QWidget();
	QGridLayout* toolLayout = new QGridLayout(toolWidget);
	toolWidget->setObjectName("toolWidget");
	toolWidget->setStyleSheet("#toolWidget{\n"
		"background: rgba(36, 47, 61, 1);\n"
		"}");
	toolWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	toolWidget->setContentsMargins(0, 0, 0, 0);
	toolLayout->setContentsMargins(0, 0, 0, 0);
	toolLayout->setSpacing(0);

	toolLayout->addWidget(minimizeWindowButton, 0, 0, 1, 1, Qt::AlignRight | Qt::AlignTop);
	toolLayout->addWidget(closeWindowButton, 0, 1, 1, 1, Qt::AlignRight | Qt::AlignTop);

	toolLayout->setAlignment(Qt::AlignRight | Qt::AlignTop);
	_grid->setVerticalSpacing(0);
	_grid->addWidget(toolWidget, _grid->rowCount(), 0, 1, 1);

	setContentsMargins(0, 0, 0, 0);
	setWindowFlag(Qt::SplashScreen);
}

void MessageMediaViewer::toNext() {

}

void MessageMediaViewer::toPrevious() {
	
}