#pragma once

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPainter>
#include "../media/ClickableLabel.h"
#include "../media/player/MediaPlayer.h"

class MessageWidget : public QWidget {
private:
	Q_OBJECT
	ClickableLabel* imageLabel;
	int panelWidth;
	QImage image;
	QWidget* messageWidget;
	MediaPlayer* _mediaPlayer;
public:
	MessageWidget(QWidget* parent = nullptr);
	[[nodiscard]] static QSize getMinimumSizeWithAspectRatio(const QSize& imageSize, const int parentWidth);
protected:
	void resizeEvent(QResizeEvent* event) override;
public Q_SLOTS:
	void attachment_cliked();
};