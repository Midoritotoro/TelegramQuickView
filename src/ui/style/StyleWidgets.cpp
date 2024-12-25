#pragma once 


namespace style {
	QString SliderStyle() {
		if (_sliderStyle.isEmpty())
			loadStyles();
		return _sliderStyle;
	}

	QString ScrollAreaStyle() {
		if (_scrollAreaStyle.isEmpty())
			loadStyles();
		return _scrollAreaStyle;
	}

} // namespace style