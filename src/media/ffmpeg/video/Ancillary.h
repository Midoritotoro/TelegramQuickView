#pragma once 

#include "Types.h"


namespace FFmpeg {
	void AncillaryRelease(struct ancillary* ancillary);
	void AncillaryArrayClear(struct ancillary*** array);
} // namespace FFmpeg