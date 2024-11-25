#pragma once

#include <iostream>
#include <QApplication>

class TelegramPostQuickView;


class Application final: public QApplication {
private:
	std::unique_ptr<TelegramPostQuickView> _postsView = nullptr;
public:
	Application(int& argc, char** argv);
	~Application();

	[[nodiscard]] static std::unique_ptr<Application> Create(int argc, char* argv[]);
private:
	void init();
};