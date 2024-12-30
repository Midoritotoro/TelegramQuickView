#pragma once 

#include <cstdlib>
#include <cstdint>

#include <limits.h>
#include "Types.h"
#include "Config.h"

#include <search.h>
#include <winsock.h>

#include "Threads.h"
#include "TFunctions.h"

#include <stdatomic.h>

#define container_of(ptr, type, member) \
    ((type *)(((char *)(ptr)) - offsetof(type, member)))

namespace FFmpeg {
 
    static thread_local struct rcu_thread current;

    bool rcu_read_held(void)
    {
        const struct rcu_thread* const self = &current;

        return self->recursion > 0;
    }

    static std::atomic<rcu_generation*> generation;


    void rcu_read_lock(void)
    {
        rcu_thread* const self = &current;
        rcu_generation* gen;

        if (self->recursion++ > 0)
            return; /* recursion: nothing to do */

        assert(self->generation == NULL);
        gen = atomic_load_explicit(&generation, std::memory_order_acquire);
        self->generation = gen;
        atomic_fetch_add_explicit(&gen->readers, 1, std::memory_order_relaxed);
    }

    void rcu_read_unlock(void)
    {
        rcu_thread* const self = &current;
        rcu_generation* gen;

        assert(rcu_read_held());

        if (--self->recursion > 0)
            return; /* recursion: nothing to do */

        gen = self->generation;
        self->generation = NULL;

        uintptr_t readers = atomic_fetch_sub_explicit(&gen->readers, 1,
            std::memory_order_relaxed);
        if (readers == 0)
            assert("unreachable!", unreachable());
        if (readers > 1)
            return; /* Other reader threads remain: nothing to do */

        if (unlikely(atomic_exchange_explicit(&gen->writer, 0,
            std::memory_order_release)))
            atomic_notify_one(&gen->writer); /* Last reader wakes writer up */
    }

} // namespace FFmpeg
