#include "ImagePushButton.h"

ImagePushButton::ImagePushButton(const QImage& image, QWidget* parent, bool rounded): 
    m_image(image), QPushButton(parent), m_Rounded(rounded)
{
    if (m_Rounded) {
        QRegion region(QRect(0, 0, width(), height()), QRegion::Ellipse);
        setMask(region);
    }
}

ImagePushButton::ImagePushButton(const QString imagePath, QWidget* parent, bool rounded) :
    QPushButton(parent), m_Rounded(rounded)
{
    
    setContentsMargins(0, 0, 0, 0);
    setStyleSheet("QPushButton {padding: 0px;}");
    QPixmap pixmap(imagePath);
    _imageLabel = new QLabel(this);
    _imageLabel->setBackgroundRole(QPalette::Dark);
    _imageLabel->setScaledContents(true);
    _imageLabel->setPixmap(pixmap);
    _imageLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _imageLabel->setStyleSheet("background-color: transparent");
    _imageLabel->setAttribute(Qt::WA_NoSystemBackground);
    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(_imageLabel, 0, 0, 1, 1, Qt::AlignCenter);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    if (m_Rounded) {
        QRegion region(QRect(0, 0, width(), height()), QRegion::Ellipse);
        setMask(region);
        _imageLabel->setMask(region);
    }
}