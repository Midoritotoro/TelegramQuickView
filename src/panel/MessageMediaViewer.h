#pragma once

#include <memory>
#include "History.h"

#include "../core/Types.h"
#include <QWidget>

#include <QGridLayout>


class FlatButton;
class MessageTextView;

class WidgetsHider;
class MediaPlayer;


class MessageMediaViewer: public QWidget {
	Q_OBJECT
private:
	QGridLayout* _grid = nullptr;

	std::unique_ptr<MediaPlayer> _mediaPlayer = nullptr;
	std::unique_ptr<WidgetsHider> _widgetsHider = nullptr;

	FlatButton* _nextAttachment = nullptr;
	FlatButton* _previousAttachment = nullptr;

	MessageWidget* _currentMessage = nullptr;
	History* _messagesHistory;
	MessageTextView* _messageTextView = nullptr;

	int _currentMessageAttachmentIndex = 0;
public:
	MessageMediaViewer(
		not_null<History*> messagesHistory,
		QWidget* parent = nullptr);

	void updateMediaNavigationButtons();
	void updateMessageTextView();

	void openMessageAttachment(
		not_null<MessageWidget*> messageWidget,
		int triggeredAttachmentIndex);

	[[nodiscard]] int nextMessageWithAttachmentsIndex(int currentIndex) const noexcept;
	[[nodiscard]] int previousMessageWithAttachmentsIndex(int currentIndex) const noexcept;

	void goToPreviousMessage();
	void goToNextMessage();
Q_SIGNALS:
	void needScrollToMessage(MessageWidget* message);
	void escaped();
public Q_SLOTS:
	void nextAttachmentButton_clicked();
	void previousAttachmentButton_clicked();
protected:
	void resizeEvent(QResizeEvent* event) override;
	void closeEvent(QCloseEvent* event) override;
};