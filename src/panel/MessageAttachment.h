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

	inline void setParentMessage(MessageWidget* parentMessage) noexcept { _parentMessage = parentMessage; }

	[[nodiscard]] inline QString attachmentPath() const noexcept { return _attachmentPath;  }
	[[nodiscard]] inline AttachmentType attachmentType() const noexcept { return _attachmentType; }

	[[nodiscard]] inline MessageWidget* parentMessage() const noexcept { return _parentMessage; }
	[[nodiscard]] static AttachmentType detectMediaType(const QString& filePath) noexcept;
protected:
	void paintEvent(QPaintEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
private:
	void paintAttachmentCount(QPainter& painter);
	void preparePreview();
	void updatePreviewSize();

	QString _attachmentPath;
	AttachmentType _attachmentType;

	MessageWidget* _parentMessage = nullptr;
	QSize _attachmentPreviewSize;

	int _attachmentWidth = 0;
};
