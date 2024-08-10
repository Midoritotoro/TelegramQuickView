#pragma once

#include <QWidget>
#include <QGridLayout>

#include "../media/player/MediaPlayer.h"
#include "../media/ClickableLabel.h"

#include "MessageWidget.h"
#include <QResizeEvent>

class History;


class MessageMediaViewer: public QWidget {
private:
	Q_OBJECT
	QGridLayout* _grid = nullptr;
	MediaPlayer* _mediaPlayer = nullptr;
	ClickableLabel* _nextAttachment = nullptr;
	ClickableLabel* _previousAttachment = nullptr;
	MessageWidget* _currentMessage = nullptr;
	History* _messagesHistory = nullptr;
	int _currentMessageAttachmentIndex = 0;
public:
	MessageMediaViewer(History* messagesHistory, QWidget* parent = nullptr);

	void updateMediaNavigationButtons();
	void openMessageAttachment(MessageWidget* messageWidget, int triggeredAttachmentIndex);

	[[nodiscard]] bool isNextMediaAvailable();

	[[nodiscard]] int nextMessageWithAttachmentsIndex(int currentIndex) const noexcept;
	[[nodiscard]] int previousMessageWithAttachmentsIndex(int currentIndex) const noexcept;

	void goToPreviousMessage();
	void goToNextMessage();
public Q_SLOTS:
	void toNext();
	void toPrevious();
protected:
	void resizeEvent(QResizeEvent* event) override;
};