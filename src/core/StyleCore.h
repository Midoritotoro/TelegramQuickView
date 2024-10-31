#pragma once

#include <QImage>
#include <QSize>

#include <algorithm>
#include <cmath>


namespace style {

inline constexpr auto kScaleMin = 50;
inline constexpr auto kScaleMax = 300;
inline constexpr auto kScaleDefault = 100;

[[nodiscard]] int DevicePixelRatio();
void SetDevicePixelRatio(int ratio);

[[nodiscard]] int Scale();
void SetScale(int scale);

[[nodiscard]] QImage Opaque(QImage&& image);

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

} // namespace style