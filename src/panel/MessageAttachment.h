#pragma once

#include "../media/ClickableLabel.h"

class MessageWidget;
class QImage;
class QPaintEvent;
class QResizeEvent;


class MessageAttachment: public ClickableLabel {
private:
	Q_OBJECT
	QString _attachmentPath;
	QString _attachmentType;
	MessageWidget* _parentMessage = nullptr;
	QImage _attachmentPreview;
	int _attachmentWidth = 0;
public:
	MessageAttachment(QString attachmentPath, int attachmentWidth, MessageWidget* parentMessage);

	[[nodiscard]] static QSize getMinimumSizeWithAspectRatio(const QSize& imageSize, const int parentWidth);

	void setParentMessage(MessageWidget* parentMessage);

	[[nodiscard]] QString attachmentPath() const noexcept;
	[[nodiscard]] QString attachmentType() const noexcept;

	[[nodiscard]] MessageWidget* parentMessage() const noexcept;
	[[nodiscard]] static QString detectMediaType(const QString& filePath) noexcept;
protected:
	void paintEvent(QPaintEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
private:
	void updatePreviewSize();
};
