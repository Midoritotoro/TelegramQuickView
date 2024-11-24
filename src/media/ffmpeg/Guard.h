#pragma once

#include <utility>
#include <type_traits>

namespace Guard {
	template <class F>
	class FinalAction
	{
	public:
		static_assert(!std::is_reference<F>::value && !std::is_const<F>::value &&
			!std::is_volatile<F>::value,
			"FinalAction should store its callable by value");

		explicit FinalAction(F f) noexcept : f_(std::move(f)) {}

		FinalAction(FinalAction&& other) noexcept
			: f_(std::move(other.f_)), invoke_(std::exchange(other.invoke_, false))
		{}

		FinalAction(const FinalAction&) = delete;
		FinalAction& operator=(const FinalAction&) = delete;
		FinalAction& operator=(FinalAction&&) = delete;

		~FinalAction() noexcept
		{
			if (invoke_)
				f_();
		}

	private:
		F f_;
		bool invoke_{ true };
	};


	template <class F>
	[[nodiscard]] FinalAction<typename std::remove_cv<typename std::remove_reference<F>::type>::type>
		finally(F&& f) noexcept
	{
		return FinalAction<typename std::remove_cv<typename std::remove_reference<F>::type>::type>(
			std::forward<F>(f));
	}
} // namespace Guard