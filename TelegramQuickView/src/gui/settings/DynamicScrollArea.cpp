#include "DynamicScrollArea.h"

DynamicScrollArea::DynamicScrollArea(QWidget* parent) :
	QScrollArea(parent)
{
	setWidgetResizable(true);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}


DynamicScrollArea::~DynamicScrollArea()
{

}