#include "MessageWidget.h"

#include "../media/ClickableLabel.h"

MessageWidget::MessageWidget(QWidget* parent) :
	QWidget(parent)
{
	setContentsMargins(0, 0, 0, 0);
	QGridLayout* gridLayout = new QGridLayout(this);
	QLabel* messageTextLabel = new QLabel("fkeoisfkeiofr je fjeoifj seoifjseif jsiefjseiofesif jhsoeif sief isfiesjf ijsfoisejf iosfi sje fjsieofjioe foehopuwhwqf");
	ClickableLabel* clickableLabel = new ClickableLabel();
	QString imagePath = "D:\\Media\\chatgpt3\\1\\Изображения\\photo_2024-07-12_09-49-17.jpg";
	QPixmap pixmap(imagePath);
	clickableLabel->setBackgroundRole(QPalette::Dark);
	clickableLabel->setScaledContents(true);
	clickableLabel->setPixmap(pixmap);
	clickableLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setStyleSheet("background-color: transparent");
	setAttribute(Qt::WA_NoSystemBackground);
	setCursor(Qt::PointingHandCursor);

	gridLayout->setContentsMargins(0, 0, 0, 0);
	gridLayout->setSpacing(0);
	gridLayout->setAlignment(Qt::AlignBottom | Qt::AlignVCenter);
	messageTextLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	messageTextLabel->setWordWrap(true);

	setStyleSheet(QString::fromUtf8("QWidget{ \n"
		"background: #333;\n"
		"border-radius: 15px\n"
	"}"
	"*{\n"
		"font-family: centry gothic;\n"
		"font-size: 24px;\n"
	"}"));

	messageTextLabel->setStyleSheet("QLabel{ \n"
		"background: Wheat;\n"
		"border-radius: 5px;\n"
		"color: black;\n"
	"}");

	gridLayout->addWidget(clickableLabel, 0, 0, 1, 1, Qt::AlignTop | Qt::AlignHCenter);
	gridLayout->addWidget(messageTextLabel, 1, 0, 1, 1, Qt::AlignBottom | Qt::AlignHCenter);
}