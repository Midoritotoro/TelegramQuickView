#pragma once

#include <QWidget>
#include <QLabel>

#include "MessageAttachment.h"


static class MessageWidget {
public:
	[[nodiscard]] static QWidget* createMessageWidget();
	[[nodiscard]] static QLabel* createMessageTextLabel();
};