#include "MessageMediaViewer.h"

#include <QToolButton>
#include <QDir>
#include <QFile>
#include <QApplication>
#include <QStyle>


MessageMediaViewer::MessageMediaViewer(QWidget* parent) :
	QWidget(parent)
{
	QToolButton* minimizeWindowButton = new QToolButton();
	QToolButton* maximizeWindowButton = new QToolButton();
	QToolButton* closeWindowButton = new QToolButton();

	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	setStyleSheet("QWidget{\n "
		"background-color: rgba(35,36,37, 0.7)\n"
		"}");

	QPixmap minPix = style()->standardPixmap(QStyle::SP_TitleBarMinButton);
	QPixmap closePix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton);
	QPixmap maxPix = style()->standardPixmap(QStyle::SP_TitleBarMaxButton);

	minimizeWindowButton->setIcon(minPix);
	maximizeWindowButton->setIcon(maxPix);
	closeWindowButton->setIcon(closePix);

	minimizeWindowButton->setAttribute(Qt::WA_NoSystemBackground);
	closeWindowButton->setAttribute(Qt::WA_NoSystemBackground);
	maximizeWindowButton->setAttribute(Qt::WA_NoSystemBackground);

	minimizeWindowButton->setStyleSheet("background-color: transparent;");
	closeWindowButton->setStyleSheet("background-color: transparent;");
	maximizeWindowButton->setStyleSheet("background-color: transparent;");

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

	toolLayout->addWidget(minimizeWindowButton, 0, toolLayout->columnCount(), 1, 1, Qt::AlignRight | Qt::AlignTop);
	toolLayout->addWidget(maximizeWindowButton, 0, toolLayout->columnCount(), 1, 1, Qt::AlignRight | Qt::AlignTop);
	toolLayout->addWidget(closeWindowButton, 0, toolLayout->columnCount(), 1, 1, Qt::AlignRight | Qt::AlignTop);

	toolLayout->setAlignment(Qt::AlignRight | Qt::AlignTop);
	_grid->setSpacing(0);
	_grid->setContentsMargins(0, 0, 0, 0);
	setContentsMargins(0, 0, 0, 0);
	//_grid->addWidget(toolWidget, _grid->rowCount(), 0, 1, 1);
	_grid->addWidget(_mediaPlayer, _grid->rowCount(), 0, 1, 1);

	setContentsMargins(0, 0, 0, 0);

	_mediaPlayer->setVisible(false);

	connect(minimizeWindowButton, &QToolButton::clicked, this, &QWidget::showMinimized);
	connect(closeWindowButton, &QToolButton::clicked, this, &QWidget::close);
	connect(maximizeWindowButton, &QToolButton::clicked, this, [this]() {
		isFullScreen() ? showNormal() : showFullScreen();
		});
}

void MessageMediaViewer::openMessageAttachment(MessageAttachment* messageAttachment) {
	_mediaPlayer->setVisible(true);
	_mediaPlayer->setSource(QUrl::fromLocalFile(messageAttachment->attachmentPath()));
	showFullScreen();
	_mediaPlayer->showFullScreen();
	qDebug() << _mediaPlayer->size();
	qDebug() << size();

}

void MessageMediaViewer::toNext() {

}

void MessageMediaViewer::toPrevious() {

}