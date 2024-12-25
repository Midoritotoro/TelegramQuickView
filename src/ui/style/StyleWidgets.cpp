#include "StyleWidgets.h"

#include <QFile>
#include <QString>


namespace style {
	namespace {
		QString _sliderStyle = "";
		QString _scrollAreaStyle = "";
	} // namespace

	void loadStyles() {
		const auto sliderStyle = ":/src/css/SliderStyle.css";
		const auto scrollAreaStyle = ":/src/css/ScrollAreaStyle.css";

		auto file = QFile(sliderStyle);

		if (file.open(QFile::ReadOnly))
			_sliderStyle = file.readAll();

		file.setFileName(scrollAreaStyle);

		if (file.open(QFile::ReadOnly))
			_scrollAreaStyle = file.readAll();
	}

	QString SliderStyle() {
		return _sliderStyle;
	}

	QString ScrollAreaStyle() {
		return _scrollAreaStyle;
	}

} // namespace style