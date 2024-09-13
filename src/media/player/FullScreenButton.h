#pragma once

#include <QPushButton>
#include <QMouseEvent>


class FullScreenButton : public QPushButton {
	Q_OBJECT
public:
	enum class State {
		FullScreenTo,
		FullScreenFrom
	};

	FullScreenButton(QWidget* parent = nullptr);
	
	[[nodiscard]] State state() const noexcept;
protected:
	void paintEvent(QPaintEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;

	void mousePressEvent(QMouseEvent* event) override;
private:
	QString _fullScreenToImagePath;
	QString _fullScreenFromImagePath;
	QPixmap _currentPixmap;
	State _state;
};

