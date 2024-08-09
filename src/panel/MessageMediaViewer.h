#pragma once

#include <QWidget>
#include <QGridLayout>

#include "MessageAttachment.h"
#include "../media/player/MediaPlayer.h"
#include "../media/ClickableLabel.h"
#include <QResizeEvent>


class MessageMediaViewer: public QWidget {
private:
	Q_OBJECT
	QGridLayout* _grid = nullptr;
	MediaPlayer* _mediaPlayer = nullptr;
	ClickableLabel* _nextAttachment = nullptr;
	ClickableLabel* _previousAttachment = nullptr;
public:
	MessageMediaViewer(QWidget* parent = nullptr);

	void openMessageAttachment(MessageAttachment* messageAttachment);
	void toNext();
	void toPrevious();
protected:
	void resizeEvent(QResizeEvent* event) override;
};