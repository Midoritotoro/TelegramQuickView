#pragma once

#include "../media/ClickableLabel.h"


class QUrl;
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

	[[nodiscard]] QString attachmentPath() const noexcept;
	[[nodiscard]] QString attachmentType() const noexcept;
	[[nodiscard]] MessageWidget* parentMessage() const noexcept;
	[[nodiscard]] static QString detectMediaType(const QString& filePath);
};
