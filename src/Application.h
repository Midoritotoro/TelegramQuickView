#pragma once

#include "view/MessagesView.h"
#include "ui/MainQueueProcessor.h"

#include <iostream>
#include <QApplication>


class MediaPlayer;


class Application final: public QApplication {
private:
	std::unique_ptr<MessagesView> _messagesView = nullptr;
	std::unique_ptr<MediaPlayer> _mediaPlayer = nullptr;
	std::unique_ptr<MainQueueProcessor> _mainQueueProcessor = nullptr;
public:
	Application(int& argc, char** argv);
	~Application();

	[[nodiscard]] static std::unique_ptr<Application> Create(int argc, char* argv[]);
private:
	void init();
};