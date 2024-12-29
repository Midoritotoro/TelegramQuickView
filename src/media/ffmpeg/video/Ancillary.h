#pragma once 

#include "Types.h"


namespace FFmpeg {
	void AncillaryRelease(ancillary* ancillary);
	void AncillaryArrayClear(ancillary*** array);

	void AncillaryArrayInit(ancillary*** array);
} // namespace FFmpeg