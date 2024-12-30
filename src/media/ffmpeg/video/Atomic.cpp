#include "Atomic.h"

#include <Windows.h>
#include <cassert>


namespace Threads {
    bool AtomicRcDec(atomic_rc_t* rc) {
        uintptr_t prev = atomic_fetch_sub_explicit(&rc->refs, (uintptr_t)1,
            std::memory_order_acq_rel);
        assert(prev);

        return prev == 1;
    }

    void AtomicRcInit(atomic_rc_t* rc) {
        atomic_init(&rc->refs, (uintptr_t)1);
    }


    void atomic_notify_one(void* addr)
    {
        WakeByAddressSingle(addr);
    }

    void atomic_notify_all(void* addr)
    {
        WakeByAddressAll(addr);
    }
} // namespace Threads