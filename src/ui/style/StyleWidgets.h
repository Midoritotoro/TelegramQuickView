#pragma once 

#include "StyleFont.h"


namespace style {
	struct FlatLabel {
		margins margin;
		color colorBg;
		const TextStyle* textStyle;
		const TextPalette* textPalette;
	};

	struct Slider {

	};

	const FlatLabel defaultFlatLabelStyle = FlatLabel{
		.margin = margins{
			10, 10, 10, 10
		},
		.colorBg = QColor(24, 37, 51),
		.textStyle = defaultTextStyle,
		.textPalette = &defaultTextPalette
	};
}