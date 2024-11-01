#pragma once

#include <QWidget>
#include <QGridLayout>

#include "../media/player/MediaPlayer.h"
#include "../media/ClickableLabel.h"

#include "MessageWidget.h"
#include <QResizeEvent>
#include <QCloseEvent>
#include <QPainter>
#include <memory>
#include "History.h"

class NavigationButton;
class MessageTextView;
class WidgetsHider;


class MessageMediaViewer: public QWidget {
	Q_OBJECT
private:
	QGridLayout* _grid = nullptr;

	std::unique_ptr<MediaPlayer> _mediaPlayer = nullptr;
	std::unique_ptr<WidgetsHider> _widgetsHider = nullptr;

	NavigationButton* _nextAttachment = nullptr;
	NavigationButton* _previousAttachment = nullptr;

	MessageWidget* _currentMessage = nullptr;
	History* _messagesHistory;
	MessageTextView* _messageTextView = nullptr;

	int _currentMessageAttachmentIndex = 0;
public:
	MessageMediaViewer(History* messagesHistory, QWidget* parent = nullptr);

	void updateMediaNavigationButtons();
	void updateMessageTextView();
	void openMessageAttachment(MessageWidget* messageWidget, int triggeredAttachmentIndex);

	[[nodiscard]] int nextMessageWithAttachmentsIndex(int currentIndex) const noexcept;
	[[nodiscard]] int previousMessageWithAttachmentsIndex(int currentIndex) const noexcept;

	void goToPreviousMessage();
	void goToNextMessage();
Q_SIGNALS:
	void escaped();
public Q_SLOTS:
	void nextAttachmentButton_clicked();
	void previousAttachmentButton_clicked();
protected:
	void resizeEvent(QResizeEvent* event) override;
	void closeEvent(QCloseEvent* event) override;
};