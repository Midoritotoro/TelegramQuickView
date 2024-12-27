#pragma once 

#include "StyleFont.h"


namespace style {
	void loadStyles();

	[[nodiscard]] QString SliderStyle();
	[[nodiscard]] QString ScrollAreaStyle();

	struct FlatLabel {
		margins margin = { 0, 0, 0, 0 };
		color colorBg;

		const TextStyle* textStyle = nullptr;
		const TextPalette* textPalette = nullptr;

		int maximumWidth = 0;
		int maximumHeight = 0;

		int minimumWidth = 0;
		int minimumHeight = 0;
	};

	struct Slider {

	};

	struct PopupMenu {
		margins margin = { 0, 0, 0, 0 };
		color colorBg;

		int borderRadius = 0;

		const TextStyle* textStyle = nullptr;
		const TextPalette* textPalette = nullptr;
	};

	inline const FlatLabel* defaultFlatLabelStyle = nullptr;
	inline const PopupMenu* defaultPopupMenuStyle = nullptr;
} // namespace style