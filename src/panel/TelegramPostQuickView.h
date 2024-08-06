#pragma once

#include <QScrollArea>

#include "MessageWidget.h"

class TelegramPostQuickView: public QWidget {
private:
	Q_OBJECT
	int panelWidth;
	QScrollArea* _chatScrollArea = nullptr;
	MediaPlayer* _mediaPlayer = nullptr;
	QGridLayout* _chatScrollAreaLayout = nullptr;
	QGridLayout* _grid = nullptr;
	QWidget* _chatScrollAreaWidget = nullptr;
public:
	TelegramPostQuickView(const QString& messageText = "", const QUrlList& attachmentsPaths = {}, QWidget* parent = nullptr);
	void addMessage(const QString& messageText, const QUrlList& attachmentsPaths = {});

public Q_SLOTS:
	void attachment_cliked();
};