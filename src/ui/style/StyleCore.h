#pragma once

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

	inline constexpr auto maximumMessageWidth = 430;
	inline constexpr auto minimumMessageWidth = 100;

	inline constexpr auto minimumMessageHeight = 100;

	inline constexpr auto maximumTextHeight = 1080 * 5;
	inline constexpr auto maximumTextWidth = 430;

	inline constexpr auto sliderHeight = 20;

	inline constexpr QMargins messageTextViewMargins = { 8, 5, 8, 8 };
	inline constexpr auto messageTextViewBorderRadius = 10;

	inline constexpr auto panelWidth = 548;

	inline constexpr auto kScaleMin = 50;
	inline constexpr auto kScaleMax = 300;
	inline constexpr auto kScaleDefault = 100;

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

	[[nodiscard]] int DevicePixelRatio();
	void SetDevicePixelRatio(int ratio);

	[[nodiscard]] int Scale();
	void SetScale(int scale);

	[[nodiscard]] QSize TextSize(
		const QString& text,
		const QFontMetrics& metrics);
	[[nodiscard]] QSize TextSize(
		const QString& text,
		const QFont& font);

	[[nodiscard]] int FontHeight(const QFont& font);

	[[nodiscard]] QString SliderStyle();
	[[nodiscard]] QString ScrollAreaStyle();

	[[nodiscard]] QPixmap PixmapFast(QImage&& image);

	[[nodiscard]] QImage Opaque(QImage&& image);
	[[nodiscard]] QSize getMinimumSizeWithAspectRatio(
		const QSize& imageSize,
		const int targetWidth);

	[[nodiscard]] bool PartiallyEqual(
		const QSize& first,
		const QSize& second,
		int maxDifference);

	[[nodiscard]] QPixmap FindPreviewInCache(const QString& key);
	[[nodiscard]] QSize MediaResolution(const QString& path);

	[[nodiscard]] QPixmap MediaPreview(const QString& path);
	[[nodiscard]] QPixmap GenerateThumbnail(
		const QString& path, 
		const QSize& targetSize = QSize());

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

	void Start();
	void Stop();

	void RegisterStyles();
} // namespace style