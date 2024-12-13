#pragma once

#include <utility>
#include <xutility>
#include <vector>


namespace core::utility {
	template <typename T>
	inline void accumulateMax(T& a, const T& b) {
		if (a < b) a = b; 
	}

	template <typename T>
	inline void accumulateMin(T& a, const T& b) {
		if (a > b) a = b; 
	}
} // namespace core::utility