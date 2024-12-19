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

	inline const FlatLabel* defaultFlatLabelStyle = nullptr;
}