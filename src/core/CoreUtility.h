
#pragma once

#include <utility>
#include <xutility>

#include <variant>

#include <vector>
#include <QGuiApplication>

#include <cfenv>
#include <algorithm>

#include <iterator>

#include <ranges>
#include <gsl/gsl>

#include "Concepts.h"

using namespace std::ranges;


namespace core::utility {
	template <typename T>
	inline void accumulateMax(T& a, const T& b) {
		if (a < b) a = b; 
	}

	template <typename T>
	inline void accumulateMin(T& a, const T& b) {
		if (a > b) a = b; 
	}

	struct plus
	{
		template<typename T, typename U>
		constexpr auto operator()(T&& t, U&& u) const -> decltype((T&&)t + (U&&)u)
		{
			return (T&&)t + (U&&)u;
		}
		using is_transparent = void;
	};

	struct identity
	{
		template<typename T>
		constexpr T&& operator()(T&& t) const noexcept
		{
			return (T&&)t;
		}
		using is_transparent = void;
	};

	//template <input_range _First, forward_range... _Rest>
	//	requires (view<_First> && ... && view<_Rest>)

	struct accumulate_fn
	{


	/*	template <typename I>
		concept InputIter = requires ()
		template <I _First, typename S _Last, typename T _Init, typename Op = plus,
			typename P = identity>

		requires std::sentinel_for<S, I> AND std::input_iterator<I> AND
			std::indirectly_binary_invocable_<Op, T*, std::projected<I, P>> AND
			std::assignable_from<T&, std::indirect_result_t<Op&, T*, std::projected<I, P>>>) */
		template<typename I, typename S, typename T, typename Op = plus,
			typename P = identity>
		T operator()(I first, S last, T init, Op op = Op{},
			P proj = P{}) const
		{
			for (; first != last; ++first)
				init = invoke(op, init, invoke(proj, *first));
			return init;
		}

		template<typename Rng, typename T, typename Op = plus, typename P = identity>
		
		//concept Cn = (requires input_range<Rng> AND
		//	indirectly_binary_invocable_<Op, T*, projected<iterator_t<Rng>, P>> AND
		//	assignable_from<
		//	T&, indirect_result_t<Op&, T*, projected<iterator_t<Rng>, P>>>);

			T operator()(Rng&& rng, T init, Op op = Op{}, P proj = P{}) const
		{
			return (*this)(
				begin(rng), end(rng), std::move(init), std::move(op), std::move(proj));
		}
	};

	inline constexpr accumulate_fn accumulate{};


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