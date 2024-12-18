
#pragma once

#include <vector>
#include <QGuiApplication>

#include <cfenv>
#include <algorithm>

#include <gsl/gsl>

#include "utility/Accumulate.h"


namespace core::utility {
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

		return value;
	}
} // namespace core::utility