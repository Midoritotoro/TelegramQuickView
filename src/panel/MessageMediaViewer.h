#pragma once

#include <QWidget>
#include <QGridLayout>

#include "../media/player/MediaPlayer.h"
#include "../media/ClickableLabel.h"

#include "MessageWidget.h"
#include <QResizeEvent>
#include <QPaintEvent>
#include <QPainter>

class History;
class NavigationButton;


class MessageMediaViewer: public QWidget {
private:
	Q_OBJECT
	QGridLayout* _grid = nullptr;
	MediaPlayer* _mediaPlayer = nullptr;
	NavigationButton* _nextAttachment = nullptr;
	NavigationButton* _previousAttachment = nullptr;
	MessageWidget* _currentMessage = nullptr;
	History* _messagesHistory = nullptr;
	int _currentMessageAttachmentIndex = 0;
public:
	MessageMediaViewer(History* messagesHistory, QWidget* parent = nullptr);

	void updateMediaNavigationButtons();
	void openMessageAttachment(MessageWidget* messageWidget, int triggeredAttachmentIndex);

	[[nodiscard]] int nextMessageWithAttachmentsIndex(int currentIndex) const noexcept;
	[[nodiscard]] int previousMessageWithAttachmentsIndex(int currentIndex) const noexcept;

	void goToPreviousMessage();
	void goToNextMessage();
public Q_SLOTS:
	void nextAttachmentButton_clicked();
	void previousAttachmentButton_clicked();
protected:
	void resizeEvent(QResizeEvent* event) override;
};