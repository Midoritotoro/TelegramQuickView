#pragma once 

#include <concepts>

#define AND &&

template<typename... Args>
concept type = true;

template<typename T1, typename T2, typename T3, typename T4>
concept common_reference_with_4_impl_ =
	type<common_reference_t<T1, T2, T3, T4>> AND
	convertible_to<T1, common_reference_t<T1, T2, T3, T4>> AND
	convertible_to<T2, common_reference_t<T1, T2, T3, T4>> AND
	convertible_to<T3, common_reference_t<T1, T2, T3, T4>> AND
	convertible_to<T4, common_reference_t<T1, T2, T3, T4>>;

template<typename Fun, typename... Args>
concept invocable = std::is_invocable_v<Fun, Args...>;

template<typename F, typename I1, typename I2>
concept indirectly_binary_invocable_impl_concept_ =
	invocable<F&, iter_value_t<I1>&, iter_value_t<I2>&> AND
	invocable<F&, iter_value_t<I1>&, iter_reference_t<I2>> AND
	invocable<F&, iter_reference_t<I1>, iter_value_t<I2>&> AND
	invocable<F&, iter_reference_t<I1>, iter_reference_t<I2> > AND
	invocable<F&, iter_common_reference_t<I1>, iter_common_reference_t<I2>>;


template<typename F, typename I1, typename I2>
concept indirectly_binary_invocable_ =
std::indirectly_readable<I1> AND
std::indirectly_readable<I2> AND
std::copy_constructible<F>;