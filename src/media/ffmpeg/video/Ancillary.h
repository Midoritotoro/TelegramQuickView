#pragma once 

#include "Atomic.h"
#include "../../../core/Types.h"


namespace Threads {
    typedef void (*ancillary_free_cb)(void* data);
    struct ancillary
    {
        atomic_rc_t rc;

        uint32 id;
        void* data;
        ancillary_free_cb free_cb;
    };

	void AncillaryRelease(ancillary* ancillary);
	void AncillaryArrayClear(ancillary*** array);

	void AncillaryArrayInit(ancillary*** array);
} // namespace Threads