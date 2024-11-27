#include "SpeedButton.h"

#include "../../core/StyleCore.h"

#include <QDir>
#include <QCoreApplication>

#include <QApplication>
#include <QPainter>
#include <QScreen>

#include <QMouseEvent>

SpeedButtonOverlay::SpeedButtonOverlay(QWidget* parent):
	QWidget(parent)
{
	_speedSlider = new EnhancedSlider(this);

	_speedSlider->setMaximum(20);

	_speedSlider->setStyleSheet(style::SliderStyle());
	_speedSlider->setFixedHeight(style::sliderHeight);

	connect(_speedSlider, &QAbstractSlider::valueChanged, [this](int value) {
		_speed = value / 10.;
		_textRect = QRect(QPoint(), style::TextSize(QString::number(_speed, 'f', 1) + "x", font()));
		_textRect.moveTo(QPoint(style::mediaPlayerPanelMargins.left() * 0.5,
			(height() - _textRect.height()) / 2.));
	});

	_speedSlider->setValue(_speed * 10.);
}

float SpeedButtonOverlay::speed() const noexcept {
	return _speed;
}

void SpeedButtonOverlay::paintEvent(QPaintEvent* event) {
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

	painter.setPen(Qt::NoPen);
	painter.setBrush(QColor(41, 45, 51));

	painter.drawRect(rect());

	painter.setPen(Qt::white);
	painter.drawText(_textRect, Qt::AlignCenter, QString::number(_speed, 'f', 1) + "x");
}

void SpeedButtonOverlay::resizeEvent(QResizeEvent* event) {
	_speedSlider->setGeometry(
		_textRect.right() + style::mediaPlayerPanelMargins.left(),
		(height() - _speedSlider->height()) / 2.,
		width() - _textRect.right() - style::mediaPlayerPanelMargins.left() *
		1.5 - style::mediaPlayerPanelMargins.right(), 
		_speedSlider->height());
}

SpeedController::SpeedController(QWidget* parent):
	QPushButton(parent)
{
	_overlay = new SpeedButtonOverlay(parent);
	_overlay->hide();

	const auto screenWidth = QApplication::primaryScreen()->availableGeometry().width();

	_overlay->resize(screenWidth / 12., 30);

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
