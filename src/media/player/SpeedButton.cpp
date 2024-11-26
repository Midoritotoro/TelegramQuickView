#include "SpeedButton.h"

#include "../../core/StyleCore.h"

#include <QDir>
#include <QCoreApplication>
#include <QPainter>

#include <QMouseEvent>

SpeedButtonOverlay::SpeedButtonOverlay(QWidget* parent):
	QWidget(parent)
{
	_speedController = new EnhancedSlider(this);
}

void SpeedButtonOverlay::paintEvent(QPaintEvent* event) {
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::black);

	painter.drawRect(rect());

	QFont font("Arial", 16);
	const auto speedTextSize = style::textSize(QString::number(_speed), font);

	QRect speedTextRect(QPoint(), speedTextSize);
	speedTextRect.moveCenter(rect().center());

	painter.setPen(Qt::white);
	painter.setFont(font);

	painter.drawText(speedTextRect, Qt::AlignCenter, QString::number(_speed));
}

SpeedButton::SpeedButton(QWidget* parent) :
	QWidget(parent)
{
	_overlay = new SpeedButtonOverlay(parent);
	_overlay->resize(100, 30);

	setMouseTracking(true);

	QString currentPath = QCoreApplication::applicationDirPath();
	QDir assetsDir(currentPath + "/../../assets/images");

	_speedButtonImagePath = assetsDir.absolutePath() + "/speed_button.png";

	setAttribute(Qt::WA_NoSystemBackground);
	setCursor(Qt::PointingHandCursor);
}

void SpeedButton::paintEvent(QPaintEvent* event) {
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::NoBrush);

	if (_currentPixmap.size() != size()) {
		auto image = QImage(_speedButtonImagePath);
		image = style::Prepare(image, size());

		image = std::move(image).scaled(
			image.width() * style::DevicePixelRatio(),
			image.height() * style::DevicePixelRatio(),
			Qt::IgnoreAspectRatio,
			Qt::SmoothTransformation);

		_currentPixmap = QPixmap::fromImage(std::move(image), Qt::ColorOnly);
		_currentPixmap.setDevicePixelRatio(style::DevicePixelRatio());
	}

	painter.drawPixmap(0, 0, _currentPixmap);
}

void SpeedButton::mouseMoveEvent(QMouseEvent* event) {
	if (!rect().contains(event->pos())) {
		_overlay->hide();
		return;
	}

	_overlay->show();

	const auto overlayX = x() + width() - _overlay->width();
	const auto overlayY = y() + height() - _overlay->height();

	_overlay->move(overlayX, overlayY);
}

void SpeedButton::mousePressEvent(QMouseEvent* event) {
	if (!_overlay->rect().contains(event->pos())) {
		_overlay->hide();
		return;
	}

	_overlay->show();

	const auto overlayX = x() + width() - _overlay->width();
	const auto overlayY = y() + height() - _overlay->height();

	_overlay->move(overlayX, overlayY);
}