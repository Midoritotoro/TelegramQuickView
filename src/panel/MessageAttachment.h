#pragma once

#include "../media/ClickableLabel.h"
#include <QPixmap>

class MessageWidget;
class QPaintEvent;
class QResizeEvent;


class MessageAttachment final: public ClickableLabel {
	Q_OBJECT
public:
	enum class AttachmentType {
		Audio,
		Video,
		Photo,
		Unknown
	};

	MessageAttachment(QString attachmentPath, int attachmentWidth, MessageWidget* parentMessage);

	[[nodiscard]] static QSize getMinimumSizeWithAspectRatio(const QSize& imageSize, const int parentWidth);

	inline void setParentMessage(MessageWidget* parentMessage);

	[[nodiscard]] QString attachmentPath() const noexcept;
	[[nodiscard]] AttachmentType attachmentType() const noexcept;

	[[nodiscard]] MessageWidget* parentMessage() const noexcept;
	[[nodiscard]] static AttachmentType detectMediaType(const QString& filePath) noexcept;
protected:
	void paintEvent(QPaintEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
private:
	void paintAttachmentCount(QPainter& painter);

	[[nodiscard]] QPixmap preparePreview();
	void updateSize();

	QString _attachmentPath;
	AttachmentType _attachmentType;

	MessageWidget* _parentMessage = nullptr;
	QSize _attachmentPreviewSize;

	int _attachmentWidth = 0;
};

// 44.3 мЅ - 1000 сообщений без вложений
// 1376 мЅ - 1000 сообщений с вложени€ми