#pragma once 

#include "StyleFont.h"


namespace style {
	void loadStyles();

	[[nodiscard]] QString SliderStyle();
	[[nodiscard]] QString ScrollAreaStyle();

	struct FlatLabel {
		margins margin;
		color colorBg;

		const TextStyle* textStyle;
		const TextPalette* textPalette;

		int maximumWidth;
		int maximumHeight;

		int minimumWidth;
		int minimumHeight;
	};

	struct Slider {

	};

	inline const FlatLabel* defaultFlatLabelStyle = nullptr;
} // namespace style