#include "SpeedButton.h"

#include "../../core/StyleCore.h"

#include <QDir>
#include <QCoreApplication>
#include <QPainter>

#include <QMouseEvent>

SpeedButtonOverlay::SpeedButtonOverlay(QWidget* parent):
	QWidget(parent)
{
	_speedSlider = new EnhancedSlider(this);
	_speedSlider->setStyleSheet(style::SliderStyle());
}

void SpeedButtonOverlay::paintEvent(QPaintEvent* event) {
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::black);

	painter.drawRect(rect());

	/*QFont font("Arial", 16);
	const auto speedTextSize = style::textSize(QString::number(_speed), font);

	QRect speedTextRect(QPoint(), speedTextSize);
	speedTextRect.moveCenter(rect().center());

	painter.setPen(Qt::white);
	painter.setFont(font);

	painter.drawText(speedTextRect, Qt::AlignCenter, QString::number(_speed));*/
}

void SpeedButtonOverlay::resizeEvent(QResizeEvent* event) {
	_speedSlider->setGeometry(
		(width() - _speedSlider->width()) / 2.,
		(height() - _speedSlider->height()) / 2.,
		width(),
		height()
	);
}

SpeedController::SpeedController(QWidget* parent):
	QPushButton(parent)
{
	_overlay = new SpeedButtonOverlay(parent);
	_overlay->resize(100, 30);

	_overlay->hide();

	QString currentPath = QCoreApplication::applicationDirPath();
	QDir assetsDir(currentPath + "/../../assets/images");

	_speedButtonImagePath = assetsDir.absolutePath() + "/speed_button.png";

	setAttribute(Qt::WA_NoSystemBackground);
	setCursor(Qt::PointingHandCursor);
}

void SpeedController::paintEvent(QPaintEvent* event) {
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

void SpeedController::mousePressEvent(QMouseEvent* event) {
	_overlay->isHidden()
		? _overlay->show()
		: _overlay->hide();

	_overlay->move(
		width() - _overlay->width() + x(),
		height() - _overlay->height() + y() - 
		style::mediaPlayerPanelMargins.top() * 2. -
		style::mediaPlayerPanelMargins.bottom() * 2.
	);

	_overlay->raise();
}

bool SpeedController::eventFilter(QObject* sender, QEvent* event) {
	qDebug() << "SpeedButton::eventFilter";
	switch (event->type()) {
		case QEvent::Enter:
			_overlay->show();

			_overlay->move(
				width() - _overlay->width() + x(),
				height() - _overlay->height() + y() - 
				style::mediaPlayerPanelMargins.top() * 2. - 
				style::mediaPlayerPanelMargins.bottom() * 2.
			);
			return true;

		case QEvent::Leave:
			_overlay->hide();
			return true;
		}

	return QPushButton::eventFilter(sender, event);
}
