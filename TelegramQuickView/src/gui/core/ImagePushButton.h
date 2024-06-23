#pragma once

#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>
#include <QVBoxLayout>
#include <QWidget>
#include <qpainter.h>

class ImagePushButton : public QPushButton
{
private:
    Q_OBJECT
    QImage m_image = QImage(0, 0);
    QLabel* _imageLabel = nullptr;
    bool m_Rounded;
public:
    ImagePushButton(const QImage& image, QWidget* parent = nullptr, bool rounded = false);
    ImagePushButton(const QString imagePath, QWidget* parent = nullptr, bool rounded = false);
    QLabel* imageLabel() { return _imageLabel; };
protected:
    void paintEvent(QPaintEvent* event) override
    {
        if (m_image.width() != 0) {
            QPainter painter(this);
            painter.setRenderHint(QPainter::SmoothPixmapTransform);
            m_image = m_image.scaled(m_image.width(), m_image.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            painter.drawImage(rect(), m_image);
        }
    }
};
