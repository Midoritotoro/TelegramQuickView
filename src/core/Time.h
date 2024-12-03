#pragma once

#include <cstdint>
#include <QString>

namespace Time {
	using time = std::int64_t;
	using profile_time = std::int64_t;

	namespace details {

		using inner_time_type = std::int64_t;
		using inner_profile_type = std::int64_t;

		void init();

		[[nodiscard]] inner_time_type currentValue();
		[[nodiscard]] time convert(inner_time_type value);

		[[nodiscard]] inner_profile_type currentProfileValue();
		[[nodiscard]] profile_time convertProfile(inner_profile_type);

	} // namespace details

	[[nodiscard]] time now();
	[[nodiscard]] profile_time profile();

	bool adjustTime();

	[[nodiscard]] QString formattedUnixTime(int64_t unixTime);

	[[nodiscard]] int unixTimeMinutes(int64_t unixTime);
	[[nodiscard]] int unixTimeHours(int64_t unixTime);
	[[nodiscard]] int unixTimeSeconds(int64_t unixTime);
} // namespace Time