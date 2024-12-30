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
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define STATIC_MUTEX { \
    0, \
    0, \
    0 \
}

    static /*thread_local*/ thread_t thread = NULL;

    int savecancel();
    void global_mutex(unsigned n, bool acquire);

    typedef void(*destroy)(void*);

    threadvar_t* threadvar_last = nullptr;

    int threadvar_set(threadvar_t key, void* value);
    void* threadvar_get(threadvar_t key);

    int atomic_timedwait(void* addr, unsigned value, tick_t deadline);
    tick_t mdate_wall();

    unsigned long thread_id();
    void mutex_init_common(mutex_t* mtx, bool recursive);


    void restorecancel(int state);

    void mutex_init(mutex_t* mtx);

    void mutex_init_recursive(mutex_t* mtx);
    bool mutex_held(const mutex_t* mtx);

    void mutex_lock(mutex_t* mtx);

    int mutex_trylock(mutex_t* mtx);

    void mutex_unlock(mutex_t* mtx);

    void cond_init(cond_t* cond);

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