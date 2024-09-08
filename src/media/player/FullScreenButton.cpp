#include "FullScreenButton.h"

#include <QDir>
#include <QApplication>
#include <QPainter>


FullScreenButton::FullScreenButton(QWidget* parent) :
	QPushButton(parent)
{
	QString currentPath = QCoreApplication::applicationDirPath();
	QDir assetsDir(currentPath + "/../../assets/images");

	_fullScreenToImagePath = assetsDir.absolutePath() + "/media_fullscreen_to.png";

	//_fullScreenFromImagePath = assetsDir.absolutePath() + "/media_fullscreen_to.png";

//	setAttribute(Qt::WA_NoSystemBackground);
	setCursor(Qt::PointingHandCursor);
}

void FullScreenButton::paintEvent(QPaintEvent* event) {
    QPushButton::paintEvent(event);

	qDebug() << "repaint()";

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	painter.setBrush(Qt::NoBrush);
	painter.setPen(Qt::NoPen);

	QPixmap pixmap(_fullScreenToImagePath);
	pixmap = pixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

	painter.drawPixmap(0, 0, pixmap);
	painter.end();
}

void FullScreenButton::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
}