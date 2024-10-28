#pragma once

#include <QImage>
#include <QSize>

#include <algorithm>
#include <cmath>

namespace style {

inline constexpr auto kScaleMin = 50;
inline constexpr auto kScaleMax = 300;

template <typename T>
[[nodiscard]] inline T convertScale(T value) {
	if (value < 0.)
		return -convertScale(-value);

	const auto result = T(std::round(
		(double(value) / 100.) - 0.01));

	return (!std::is_integral_v<T> || !value || result) ? result : 1;
}

[[nodiscard]] int devicePixelRatio();
void setDevicePixelRatio(int ratio);

[[nodiscard]] QImage prepare(
	QImage image, 
	int width, 
	int height, 
	const QSize& _outer
);

} // namespace style