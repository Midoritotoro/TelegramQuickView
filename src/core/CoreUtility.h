
#pragma once

#include <utility>
#include <xutility>

#include <vector>
#include <QGuiApplication>

#include <cfenv>


namespace core::utility {
	template <typename T>
	inline void accumulateMax(T& a, const T& b) {
		if (a < b) a = b; 
	}

	template <typename T>
	inline void accumulateMin(T& a, const T& b) {
		if (a > b) a = b; 
	}

	[[nodiscard]] inline bool IsCtrlPressed() {
		return (QGuiApplication::keyboardModifiers() == Qt::ControlModifier);
	}

	[[nodiscard]] inline bool IsAltPressed() {
		return (QGuiApplication::keyboardModifiers() == Qt::AltModifier);
	}

	[[nodiscard]] inline bool IsShiftPressed() {
		return (QGuiApplication::keyboardModifiers() == Qt::ShiftModifier);
	}

	[[nodiscard]] inline double SafeRound(double value) {
		Expects(!std::isnan(value));

		if (const auto result = std::round(value); !std::isnan(result)) {
			return result;
		}
		const auto errors = std::fetestexcept(FE_ALL_EXCEPT);
		if (const auto result = std::round(value); !std::isnan(result)) {
			return result;
		}

		std::feclearexcept(FE_ALL_EXCEPT);
		if (const auto result = std::round(value); !std::isnan(result)) {
			return result;
		}
	}


} // namespace core::utility