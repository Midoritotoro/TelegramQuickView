#include "SpeedButton.h"

#include "../../core/StyleCore.h"

#include <QDir>
#include <QCoreApplication>
#include <QPainter>

SpeedButton::SpeedButton(QWidget* parent) :
	QWidget(parent)
{
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