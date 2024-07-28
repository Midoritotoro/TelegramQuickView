#pragma once

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPainter>
#include "../media/ClickableLabel.h"
#include "../media/player/MediaPlayer.h"

typedef QList<QUrl> QUrlList;

class MessageAttachment: public ClickableLabel {
private:
	Q_OBJECT
	QString _attachmentPath;
	QString _attachmentType;
public:
	MessageAttachment(QString attachmentPath, QWidget* parent = nullptr);
	QString attachmentPath() const;
	QString attachmentType() const;
};

class MessageWidget : public QWidget {
private:
	Q_OBJECT
	MessageAttachment* _messageAttachment = nullptr;
	int panelWidth;
	QWidget* messageWidget = nullptr;
	MediaPlayer* _mediaPlayer = nullptr;
	QGridLayout* gridLayout = nullptr;
	QLabel* textLabel = nullptr;
public:
	MessageWidget(const QString& messageText, const QUrlList& attachmentsPaths = { QUrl() }, QWidget* parent = nullptr);
	void setSource(const QString& messageText, const QUrlList& attachmentsPaths = { QUrl() });

	[[nodiscard]] static QSize getMinimumSizeWithAspectRatio(const QSize& imageSize, const int parentWidth);
protected:
	void resizeEvent(QResizeEvent* event) override;
public Q_SLOTS:
	void attachment_cliked();
};