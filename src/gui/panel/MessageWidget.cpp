#include "MessageWidget.h"
#include "../../src/gui/media/player/MediaPlayer.h"

MessageWidget::MessageWidget(QWidget* parent) :
	QWidget(parent)
{
	setMaximumSize(632, 1080);
	setContentsMargins(0, 0, 0, 0);
	QGridLayout* gridLayout = new QGridLayout(this);
	QLabel* messageTextLabel = new QLabel("fkeoisfkeiofr je fjeoifj seoifjseif jsiefjseiofesif jhsoeif sief isfiesjf ijsfoisejf iosfi sje fjsieofjioe foehopuwhwqf");
	MediaPlayer* mediaPlayer = new MediaPlayer();
	mediaPlayer->setSource(QUrl::fromLocalFile(""));

	gridLayout->setContentsMargins(0, 0, 0, 0);
	gridLayout->setSpacing(0);
	gridLayout->setAlignment(Qt::AlignBottom | Qt::AlignVCenter);
	messageTextLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
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

	gridLayout->addWidget(mediaPlayer, 0, 0, 1, 1, Qt::AlignTop);
	//gridLayout->addWidget(messageTextLabel, 1, 0, 1, 1, Qt::AlignBottom);
}