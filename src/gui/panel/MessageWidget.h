#pragma once

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPainter>
#include "../media/ClickableLabel.h"
#include "../media/player/MediaPlayer.h"

typedef QList<QUrl> QUrlList;

class MessageAttachment {
private:
	QString _attachmentPath;
	QString _attachmentType;
public:
	MessageAttachment(QString attachmentPath);
	QString attachmentPath() const;
	QString attachmentType() const;
};

class MessageAttachmentImage : public ClickableLabel, public MessageAttachment {
private:
	Q_OBJECT
public:
	MessageAttachmentImage(QString attachmentPath, QWidget* parent = nullptr);
};

class MessageAttachmentVideo: public ClickableLabel, public MessageAttachment {
private:
	Q_OBJECT
public:
	MessageAttachmentVideo(QString attachmentPath, QWidget* parent = nullptr);
};

class MessageWidget : public QWidget {
private:
	Q_OBJECT
	ClickableLabel* imageLabel;
	int panelWidth;
	QImage image;
	QWidget* messageWidget;
	MediaPlayer* _mediaPlayer;
	QGridLayout* gridLayout = nullptr;
public:
	MessageWidget(QWidget* parent = nullptr);
	void setSource(const QString& messageText, const QUrlList& attachmentsPaths = { QUrl() });
	void setSource(const QString& messageText, const QUrl& attachmentPath);
	[[nodiscard]] static QSize getMinimumSizeWithAspectRatio(const QSize& imageSize, const int parentWidth);
protected:
	void resizeEvent(QResizeEvent* event) override;
public Q_SLOTS:
	void attachment_cliked();
};