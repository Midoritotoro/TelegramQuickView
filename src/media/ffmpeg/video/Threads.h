#pragma once 

#include "Tools.h"
#include "Atomic.h"


#include <Windows.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <errno.h>
#include <sys/locking.h>
#include <sys/utime.h>
#include <sys/timeb.h>



namespace FFmpeg {
#define STATIC_MUTEX { \
    0, \
    0, \
    0 \
}

    enum mutex_type_t
    {
        AVCODEC_MUTEX = 0,
        GCRYPT_MUTEX,
        XLIB_MUTEX,
        MOSAIC_MUTEX,
#ifdef _WIN32
        MTA_MUTEX,
#endif
        /* Insert new entry HERE */
        MAX_MUTEX
    };

    struct thread
    {
        int      thread;

        void* (*entry)(void*);
        void* data;

        std::atomic_uint killed;
        bool killable;
    };

    static thread_local struct thread* thread = NULL;

    int savecancel()
    {
        if (!thread) /* not created by VLC, can't be cancelled */
            return true;

        int oldstate = thread->killable;
        thread->killable = false;
        return oldstate;
    }

    void global_mutex(unsigned n, bool acquire)
    {
        static mutex_t locks[] = {
            STATIC_MUTEX,
            STATIC_MUTEX,
            STATIC_MUTEX,
            STATIC_MUTEX,
    #ifdef _WIN32
            STATIC_MUTEX, // For MTA holder
    #endif
        };
        static_assert (mutex_type_t::MAX_MUTEX == ARRAY_SIZE(locks),
            "Wrong number of global mutexes");
        assert(n < ARRAY_SIZE(locks));

        mutex_t* lock = locks + n;
        if (acquire)
            mutex_lock(lock);
        else
            mutex_unlock(lock);
    }

    typedef void(*destroy)(void*);

    threadvar_t* threadvar_last = nullptr;

    int threadvar_set(threadvar_t key, void* value);
    void* threadvar_get(threadvar_t key);

    int atomic_timedwait(void* addr, unsigned value, tick_t deadline);
    tick_t mdate_wall();

    unsigned long thread_id();
    void mutex_init_common(mutex_t* mtx, bool recursive);


    int savecancel();
    void restorecancel(int state);

    void mutex_init(mutex_t* mtx);

    void mutex_init_recursive(mutex_t* mtx);
    bool mutex_held(const mutex_t* mtx);

    void mutex_lock(mutex_t* mtx);

    int mutex_trylock(mutex_t* mtx);

    void mutex_unlock(mutex_t* mtx);

    void cond_init(cond_t* cond);
    struct cond_waiter {
        struct cond_waiter** pprev, * next;
        std::atomic_uint value;
    };

    static void cond_signal_waiter(struct cond_waiter* waiter);
    void cond_signal(cond_t* cond);

    void cond_broadcast(cond_t* cond);

    static void cond_wait_prepare(struct cond_waiter* waiter,
        cond_t* cond, mutex_t* mutex);

    static void cond_wait_finish(struct cond_waiter* waiter,
        cond_t* cond, mutex_t* mutex);

    void cond_wait(cond_t* cond, mutex_t* mutex);

    int cond_timedwait(cond_t* cond, mutex_t* mutex,
        tick_t deadline);
    int cond_timedwait_daytime(cond_t* cond, mutex_t* mutex,
        time_t deadline);

    /* Generic semaphores */

    void sem_init(sem_t* sem, unsigned value);
    int sem_post(sem_t* sem);
    void sem_wait(sem_t* sem);

    int sem_timedwait(sem_t* sem, tick_t deadline);

    int sem_trywait(sem_t* sem);

    int atomic_timedwait_daytime(void* addr, unsigned val, time_t deadline);
}