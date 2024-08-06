#pragma once

#include <QUrl>

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

	[[nodiscard]] QString attachmentPath() const;
	[[nodiscard]] QString attachmentType() const;
};