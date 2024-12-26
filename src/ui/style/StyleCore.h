#pragma once

#include <QImage>
#include <QSize>

#include <algorithm>
#include <cmath>

#include <QFontMetrics>
#include <QPixmapCache>

#include "StyleScale.h"


namespace style {
	inline constexpr QMargins mediaPlayerPanelMargins = { 
		10, 5, 10, 5 
	};

	inline constexpr auto mediaPlayerPanelBorderRadius = 10;

	inline constexpr auto maximumMessageWidth = 430;
	inline constexpr auto minimumMessageWidth = 100;

	inline constexpr auto minimumMessageHeight = 100;

	inline constexpr auto maximumTextHeight = 1080 * 5;
	inline constexpr auto maximumTextWidth = 430;

	inline constexpr auto sliderHeight = 20;

	inline constexpr QMargins messageTextViewMargins = { 8, 5, 8, 8 };
	inline constexpr auto messageTextViewBorderRadius = 10;

	inline constexpr auto panelWidth = 460;
	inline constexpr auto lineWidth = 1;

	namespace flatLabel {
		inline constexpr QMargins margins = {
			10, 10, 10, 10
		};
	} // namespace flatLabel

	enum class CornersRoundMode {
		All,
		Top,
		Bottom
	};

	[[nodiscard]] bool RightToLeft();
	void SetRightToLeft(bool _rightToLeft);

	[[nodiscard]] Qt::LayoutDirection LayoutDirection();

	void RoundCorners(
		QPainter& painter,
		const QSize& widgetSize,
		int borderRadius,
		CornersRoundMode roundType);
	void RoundCorners(
		QPainter& painter,
		const QSize& widgetSize,
		int borderRadius);

	void RoundTopCorners(
		QPainter& painter,
		const QSize& widgetSize,
		int borderRadius);
	void RoundBottomCorners(
		QPainter& painter,
		const QSize& widgetSize,
		int borderRadius);

	void Start();
	void Stop();

	void RegisterStyles();
} // namespace style