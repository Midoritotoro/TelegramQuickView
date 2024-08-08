#pragma once

#include <QWidget>
#include <QGridLayout>

#include "MessageAttachment.h"
#include "../media/player/MediaPlayer.h"

class MessageMediaViewer: public QWidget {
private:
	Q_OBJECT
	QGridLayout* _grid = nullptr;
	MediaPlayer* _mediaPlayer = nullptr;
public:
	MessageMediaViewer(QWidget* parent = nullptr);

	void openMessageAttachment(MessageAttachment* messageAttachment);
	void toNext();
	void toPrevious();
};