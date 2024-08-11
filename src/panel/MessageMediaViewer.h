﻿#pragma once

#include <QWidget>
#include <QGridLayout>

#include "../media/player/MediaPlayer.h"
#include "../media/ClickableLabel.h"

#include "MessageWidget.h"
#include <QResizeEvent>
#include <QPaintEvent>
#include <QPainter>


class History;


class MediaNavigationLabel : public ClickableLabel {
private:
	Q_OBJECT
	QPixmap _pixmap;
public:
	using ClickableLabel::ClickableLabel;

	void setPixmap(const QPixmap& pixmap) {
		_pixmap = pixmap;
		update();
	}
protected:
	void paintEvent(QPaintEvent* event) override {
		QPainter painter(this);
		painter.setRenderHint(QPainter::Antialiasing);
		painter.setPen(Qt::NoPen);
		painter.drawEllipse(rect());

		if (!_pixmap.isNull())
			painter.drawPixmap(rect(), _pixmap);
	}
	void resizeEvent(QResizeEvent* event) override {
		update();
		QLabel::resizeEvent(event);
	}
};


class MessageMediaViewer: public QWidget {
private:
	Q_OBJECT
	QGridLayout* _grid = nullptr;
	MediaPlayer* _mediaPlayer = nullptr;
	MediaNavigationLabel* _nextAttachment = nullptr;
	MediaNavigationLabel* _previousAttachment = nullptr;
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