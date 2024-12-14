#pragma once

#include "StyleTypes.h"
#include "StyleFont.h"

#include <QImage>
#include <QSize>

#include <algorithm>
#include <cmath>

#include <QFontMetrics>
#include <QPixmapCache>


namespace style {
	inline constexpr QMargins mediaPlayerPanelMargins = { 
		10, 5, 10, 5 
	};

	inline constexpr auto mediaPlayerPanelBorderRadius = 10;

	inline auto maximumMessageWidth = 0;
	inline auto minimumMessageWidth = 0;

	inline auto maximumTextHeight = 0;
	inline auto maximumTextWidth = 0;

	inline constexpr auto sliderHeight = 20;

	inline constexpr QMargins messageTextViewMargins = { 8, 5, 8, 8 };
	inline constexpr auto messageTextViewBorderRadius = 10;

	inline constexpr auto kScaleMin = 50;
	inline constexpr auto kScaleMax = 300;
	inline constexpr auto kScaleDefault = 100;

	enum class CornersRoundMode {
		All,
		Top,
		Bottom
	};

	[[nodiscard]] int DevicePixelRatio();
	void SetDevicePixelRatio(int ratio);

	[[nodiscard]] int Scale();
	void SetScale(int scale);

	[[nodiscard]] QSize TextSize(const QString& text, const QFontMetrics& metrics);
	[[nodiscard]] QSize TextSize(const QString& text, const QFont& font);

	[[nodiscard]] int FontHeight(const QFont& font);

	[[nodiscard]] QString SliderStyle();
	[[nodiscard]] QString ScrollAreaStyle();

	[[nodiscard]] QImage Opaque(QImage&& image);
	[[nodiscard]] QSize getMinimumSizeWithAspectRatio(const QSize& imageSize, const int targetWidth);

	[[nodiscard]] QPixmap GenerateThumbnail(const QString& path, const QSize& targetSize = QSize());

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

	[[nodiscard]] bool IsRgbNull(QRgb rgb);

	template <typename T>
	[[nodiscard]] inline T ConvertScale(T value, int scale) {
		if (value < 0.)
			return -ConvertScale(-value, scale);

		const auto result = T(std::round(
			(double(value) * scale / 100.) - 0.01));

		return (!std::is_integral_v<T> || !value || result) ? result : 1;
	}

	template <typename T>
	[[nodiscard]] inline T ConvertScale(T value) {
		return ConvertScale(value, Scale());
	}

	[[nodiscard]] QImage Prepare(
		QImage image, 
		const QSize& _outer
	);

	void StartManager() {
		internal::RegisterFontFamily("Open Sans");
	}

	void StopManager() {
		internal::DestroyFonts();
	}
} // namespace style