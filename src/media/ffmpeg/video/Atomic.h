#pragma once 

#include "Types.h"


namespace FFmpeg {
	[[nodiscard]] bool AtomicRcDec(atomic_rc_t* rc);
	void AtomicRcInit(atomic_rc_t* rc);
} // namespace FFmpeg