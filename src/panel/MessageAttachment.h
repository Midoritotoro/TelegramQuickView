#pragma once

#include "../media/ClickableLabel.h"

#include <QPaintEvent>
#include <QResizeEvent>

class QUrl;
typedef QList<QUrl> QUrlList;

class MessageWidget;


class MessageAttachment: public ClickableLabel {
private:
	Q_OBJECT
	QString _attachmentPath;
	QString _attachmentType;
	MessageWidget* _parentMessage = nullptr;
	QImage _attachmentPreview;
	int _attachmentWidth = 0;
public:
	MessageAttachment(QString attachmentPath, int attachmentWidth, MessageWidget* parentMessage, QWidget* parent = nullptr);

	[[nodiscard]] static QSize getMinimumSizeWithAspectRatio(const QSize& imageSize, const int parentWidth);

	void setParentMessage(MessageWidget* parentMessage);

	[[nodiscard]] QString attachmentPath() const noexcept;
	[[nodiscard]] QString attachmentType() const noexcept;

	[[nodiscard]] MessageWidget* parentMessage() const noexcept;
	[[nodiscard]] static QString detectMediaType(const QString& filePath);
protected:
	void paintEvent(QPaintEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
private:
	void updatePreviewSize();
};
