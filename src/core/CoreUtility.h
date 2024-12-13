
#pragma once

#include <utility>
#include <xutility>

#include <vector>
#include <QGuiApplication>


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

} // namespace core::utility