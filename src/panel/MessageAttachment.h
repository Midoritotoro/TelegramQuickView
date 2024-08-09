#pragma once

#include <QUrl>

#include "../media/ClickableLabel.h"
#include "../media/player/MediaPlayer.h"

typedef QList<QUrl> QUrlList;

class MessageWidget;

class MessageAttachment: public ClickableLabel {
private:
	Q_OBJECT
	QString _attachmentPath;
	QString _attachmentType;
	MessageWidget* _parentMessage = nullptr;
public:
	MessageAttachment(QString attachmentPath, int attachmentWidth, QWidget* parent = nullptr);

	[[nodiscard]] static QSize getMinimumSizeWithAspectRatio(const QSize& imageSize, const int parentWidth);

	void setParentMessage(MessageWidget* messageWidget);

	[[nodiscard]] QString attachmentPath() const;
	[[nodiscard]] QString attachmentType() const;
	[[nodiscard]] MessageWidget* parentMessage() const;
};