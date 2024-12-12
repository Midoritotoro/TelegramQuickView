#pragma once

#include <iostream>
#include <QApplication>

class MessagesView;
class MediaPlayer;


class Application final: public QApplication {
private:
	std::unique_ptr<MessagesView> _messagesView = nullptr;
public:
	Application(int& argc, char** argv);
	~Application();

	[[nodiscard]] static std::unique_ptr<Application> Create(int argc, char* argv[]);
private:
	void init();
};