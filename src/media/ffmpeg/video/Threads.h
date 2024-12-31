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


#define TICK_INVALID                    INT64_C(0)
#define TICK_0                          INT64_C(1)


namespace Threads {

    struct threadvar
    {
        PULONG                id;
        void                (*destroy) (void*);
        struct threadvar* prev;
        struct threadvar* next;
    };

    struct cond_waiter {
        struct cond_waiter** pprev, * next;
        std::atomic_uint value;
    };

    struct _thread
    {
        int      thread;

        void* (*entry)(void*);
        void* data;

        std::atomic_uint killed;
        bool killable;
    };

    struct sem_t
    {
        std::atomic_uint value;
    };

    using tick_t = int64_t;

    typedef void (*timer_func) (void*);
    struct timer
    {
        PTP_TIMER t;
        timer_func func;
        void* data;
    };


    struct mutex_t
    {
        struct {
            std::atomic_uint value;
            std::atomic_uint recursion;
            std::atomic_ulong owner;
        };
    };


    struct cond_t
    {
        cond_waiter* head;
        mutex_t lock;
    };

 
    struct plugin_t
    {
        struct plugin_t* next;
        module_t* module;
        unsigned modules_count;

        const char* textdomain; /**< gettext domain (or NULL) */

        /**
         * Variables set by the module to store its config options
         */
        struct
        {
            struct param* params; /**< Table of configuration items */
            size_t size; /**< Total count of all items */
            size_t count; /**< Count of real options (excludes hints) */
            size_t booleans; /**< Count of options that are of boolean type */
        } conf;
    };

    struct module_t
    {
        plugin_t* plugin; /**< Plug-in/library containing the module */
        module_t* next;

        /** Shortcuts to the module */
        unsigned    i_shortcuts;
        const char** pp_shortcuts;

        /*
         * Variables set by the module to identify itself
         */
        const char* psz_shortname;                              /**< Module name */
        const char* psz_longname;                   /**< Module descriptive name */
        const char* psz_help;        /**< Long help plain string for "special" modules */
        const char* psz_help_html;   /**< Long help HTML string, shown instead of the plain help where it makes sense to render HTML.
                                          Supports only a limited HTML4 subset, see https://doc.qt.io/qt-6/richtext-html-subset.html */

        const char* psz_capability;                              /**< Capability */
        int      i_score;                          /**< Score for the capability */

        /* Callbacks */
        const char* activate_name;
        const char* deactivate_name;
        void* pf_activate;
    };

    union module_value_t
    {
        char* psz;
        int64_t     i;
        float       f;
    };

    struct module_config_t
    {
        uint8_t     i_type; /**< Configuration type */

        const char* psz_type; /**< Configuration subtype */
        const char* psz_name; /**< Option name */
        const char* psz_text; /**< Short comment on the configuration option */
        const char* psz_longtext; /**< Long comment on the configuration option */

        module_value_t value; /**< Current value */
        module_value_t orig; /**< Default value */
        module_value_t min; /**< Minimum value (for scalars only) */
        module_value_t max; /**< Maximum value (for scalars only) */

        /* Values list */
        uint16_t list_count; /**< Choices count */
        union
        {
            const char** psz; /**< Table of possible string choices */
            const int* i; /**< Table of possible integer choices */
        } list; /**< Possible choices */
        const char** list_text; /**< Human-readable names for list values */
    };

    struct config_chain_t
    {
        config_chain_t* p_next;     /**< Pointer on the next config_chain_t element */

        char* psz_name;      /**< Option name */
        char* psz_value;     /**< Option value */
    };

    struct param {
        union {
            /*_Atomic*/ std::atomic<int64_t> i; /**< Current value (if integer or boolean) */
            /*_Atomic*/ std::atomic<float> f; /**< Current value (if floating point) */
            std::atomic<char*> /*_Atomic*/ str; /**< Current value (if character string) */
        } value;

        struct


            plugin_t* owner;
        unsigned char shortname; /**< Optional short option name */
    unsigned internal:1; /**< Hidden from preferences and help */
                     unsigned unsaved : 1; /**< Not stored in persistent configuration */
                     unsigned safe : 1; /**< Safe for untrusted provisioning (playlists) */
                     unsigned obsolete : 1; /**< Ignored for backward compatibility */
                     struct module_config_t item;
    };



#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define STATIC_MUTEX { \
    0, \
    0, \
    0 \
}

#if defined (_WIN32)
#include <process.h>
#ifndef ETIMEDOUT
#define ETIMEDOUT 10060 /* This is the value in winsock.h. */
#endif
    typedef _thread* thread_t;
    typedef threadvar* threadvar_t;
    typedef timer* timer_t;

    #define THREAD_CANCELED ((void*) UINTPTR_MAX)
#endif

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


    static  /*thread_local*/ thread_t thread = NULL;

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

} // namespace Threads