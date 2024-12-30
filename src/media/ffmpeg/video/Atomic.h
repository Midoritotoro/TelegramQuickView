#pragma once 

#include <stdatomic.h>


namespace Threads {
	struct atomic_rc_t {
		std::atomic_uintptr_t refs;
	};

	[[nodiscard]] bool AtomicRcDec(atomic_rc_t* rc);
	void AtomicRcInit(atomic_rc_t* rc);
} // namespace Threads