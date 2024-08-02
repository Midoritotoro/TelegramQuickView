#pragma once

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPainter>
#include <QScrollArea>

#include "../media/ClickableLabel.h"
#include "../media/player/MediaPlayer.h"


typedef QList<QUrl> QUrlList;

class MessageAttachment: public ClickableLabel {
private:
	Q_OBJECT
	QString _attachmentPath;
	QString _attachmentType;
public:
	MessageAttachment(QString attachmentPath, int attachmentWidth, QWidget* parent = nullptr);
	[[nodiscard]] static QSize getMinimumSizeWithAspectRatio(const QSize& imageSize, const int parentWidth);

	QString attachmentPath() const;
	QString attachmentType() const;
};

class MessageWidget : public QWidget {
private:
	Q_OBJECT
	int panelWidth;
	QScrollArea* _chatScrollArea = nullptr;
	MediaPlayer* _mediaPlayer = nullptr;
	QGridLayout *_chatScrollAreaLayout = nullptr, *_grid = nullptr;
	QWidget* _chatScrollAreaWidget = nullptr;
public:
	MessageWidget(const QString& messageText, const QUrlList& attachmentsPaths = { QUrl() }, QWidget* parent = nullptr);
	void setSource(const QString& messageText, const QUrlList& attachmentsPaths = { QUrl() });
	QWidget* createMessageWidget();
	QLabel* createMessageTextLabel();
public Q_SLOTS:
	void attachment_cliked();
};