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

	//setWindowFlag(Qt::FramelessWindowHint);
	//setAttribute(Qt::WA_TranslucentBackground);
	//setStyleSheet("color: rgba(1, 0, 0, 0)");

	QPixmap minPix = style()->standardPixmap(QStyle::SP_TitleBarMinButton);
	QPixmap closePix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton);

	minimizeWindowButton->setIcon(minPix);
	closeWindowButton->setIcon(closePix);

	minimizeWindowButton->setAttribute(Qt::WA_NoSystemBackground);
	closeWindowButton->setAttribute(Qt::WA_NoSystemBackground);

	_grid = new QGridLayout(this);
	_mediaPlayer = new MediaPlayer();
	_mediaPlayer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QWidget* toolWidget = new QWidget();
	QGridLayout* toolLayout = new QGridLayout(toolWidget);
	toolWidget->setObjectName("toolWidget");
	toolWidget->setStyleSheet("#toolWidget{\n"
		"background: transparent;\n"
		"}");
	toolWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	toolWidget->setContentsMargins(0, 0, 0, 0);
	toolLayout->setContentsMargins(0, 0, 0, 0);
	toolLayout->setSpacing(0);

	toolLayout->addWidget(minimizeWindowButton, 0, 0, 1, 1, Qt::AlignRight | Qt::AlignTop);
	toolLayout->addWidget(closeWindowButton, 0, 1, 1, 1, Qt::AlignRight | Qt::AlignTop);

	toolLayout->setAlignment(Qt::AlignRight | Qt::AlignTop);
	_grid->setVerticalSpacing(0);
	//_grid->addWidget(toolWidget, _grid->rowCount(), 0, 1, 1);
	_grid->addWidget(_mediaPlayer, _grid->rowCount(), 0, 1, 1);

	setContentsMargins(0, 0, 0, 0);
	//setWindowFlag(Qt::SplashScreen);

	_mediaPlayer->setVisible(false);

	connect(minimizeWindowButton, &QToolButton::clicked, this, &QWidget::showMinimized);
	connect(closeWindowButton, &QToolButton::clicked, this, &QWidget::close);
}

void MessageMediaViewer::openMessageAttachment(MessageAttachment* messageAttachment) {
	_mediaPlayer->setVisible(true);
	_mediaPlayer->setSource(QUrl::fromLocalFile(messageAttachment->attachmentPath()));
	showFullScreen();
}

void MessageMediaViewer::toNext() {

}

void MessageMediaViewer::toPrevious() {
	
}
