#pragma once

#include <qendian.h>
#include <cassert>

#include "Windows.h"

#include "Types.h"
#include "Atomic.h"


#ifdef Q_BIG_ENDIAN
#define WORDS_BIGENDIAN Q_BIG_ENDIAN
#else // !Q_BIG_ENDIAN
#define WORDS_LITTLE_ENDIAN Q_LITTLE_ENDIAN
#ifdef WORDS_BIGENDIAN
#undef WORDS_BIGENDIAN
#endif // WORDS_BIGENDIAN
#endif // Q_BIG_ENDIAN

#if defined (_WIN32)
#define aligned_free(ptr)                   _aligned_free(ptr)
#else
#define aligned_free(ptr)                   free(ptr)
#endif

#define CLOCK_FREQ INT64_C(1000000)
#if (CLOCK_FREQ % 1000) == 0
#define TICK_FROM_MS(ms)  ((CLOCK_FREQ / INT64_C(1000)) * (ms))
#define MS_FROM_TICK(vtk) ((vtk) / (CLOCK_FREQ / INT64_C(1000)))
#elif (1000 % CLOCK_FREQ) == 0
#define TICK_FROM_MS(ms)  ((ms)  / (INT64_C(1000) / CLOCK_FREQ))
#define MS_FROM_TICK(vtk) ((vtk) * (INT64_C(1000) / CLOCK_FREQ))
#else /* rounded overflowing conversion */
#define TICK_FROM_MS(ms)  (CLOCK_FREQ * (ms) / 1000)
#define MS_FROM_TICK(vtk) ((vtk) * 1000 / CLOCK_FREQ)
#endif /* CLOCK_FREQ / 1000 */

typedef int64_t msftime_t;

#define MSFTIME_FROM_SEC(sec)       (INT64_C(10000000) * (sec))  /* seconds in msftime_t */
#define MSFTIME_FROM_MS(sec)        (INT64_C(10000) * (sec))     /* milliseconds in msftime_t */

#if (CLOCK_FREQ % 10000000) == 0
#define TICK_FROM_MSFTIME(msft) ((msft) * (CLOCK_FREQ / INT64_C(10000000))
#define MSFTIME_FROM_TICK(vtk)  ((vtk)  / (CLOCK_FREQ / INT64_C(10000000))
#elif (10000000 % CLOCK_FREQ) == 0
#define TICK_FROM_MSFTIME(msft) ((msft) / (INT64_C(10000000) / CLOCK_FREQ))
#define MSFTIME_FROM_TICK(vtk)  ((vtk)  * (INT64_C(10000000) / CLOCK_FREQ))
#else /* rounded overflowing conversion */
#define TICK_FROM_MSFTIME(msft) (CLOCK_FREQ * (msft) / INT64_C(10000000))
#define MSFTIME_FROM_TICK(vtk)  ((vtk)  * INT64_C(10000000) / CLOCK_FREQ)
#endif /* CLOCK_FREQ / 10000000 */

#if defined (__GNUC__) || defined (__clang__)
#define likely(p)                           __builtin_expect((p), 1)
#define unlikely(p)                         __builtin_expect((p), 0)
#define unreachable()                       __builtin_unreachable()
#elif defined(_MSC_VER)
#define likely(p)                           (p)
#define unlikely(p)                         (p)
#define unreachable()                       (__assume(0))
#else
#define likely(p)                           (p)
#define unlikely(p)                         (p)
#define unreachable()                       ((void)0)
#endif

#define SUCCESS                             0
/** Unspecified error */
#define EGENERIC                            (-2 * (1 << (sizeof (int) * 8 - 2))) /* INT_MIN */

#define CONFIG_HINT_CATEGORY                0x02  /* Start of new category */

#define CONFIG_SUBCATEGORY                  0x07 /* Set subcategory */
#define CONFIG_SECTION                      0x08 /* Start of new section */

/* Configuration item types */
#define CONFIG_ITEM_FLOAT                   (1 << 5)  /* Float option */
#define CONFIG_ITEM_INTEGER                 (2 << 5)  /* Integer option */
#define CONFIG_ITEM_RGB                     (CONFIG_ITEM_INTEGER | 0x01)  /* RGB color option */
#define CONFIG_ITEM_BOOL                    (3 << 5)  /* Bool option */
#define CONFIG_ITEM_STRING                  (4 << 5)  /* String option */
#define CONFIG_ITEM_PASSWORD                (CONFIG_ITEM_STRING | 0x01)  /* Password option (*) */
#define CONFIG_ITEM_KEY                     (CONFIG_ITEM_STRING | 0x02)  /* Hot key option */
#define CONFIG_ITEM_MODULE                  (CONFIG_ITEM_STRING | 0x04)  /* Module option */
#define CONFIG_ITEM_MODULE_CAT              (CONFIG_ITEM_STRING | 0x05)  /* Module option */
#define CONFIG_ITEM_MODULE_LIST             (CONFIG_ITEM_STRING | 0x06)  /* Module option */
#define CONFIG_ITEM_MODULE_LIST_CAT         (CONFIG_ITEM_STRING | 0x07)  /* Module option */
#define CONFIG_ITEM_LOADFILE                (CONFIG_ITEM_STRING | 0x0C)  /* Read file option */
#define CONFIG_ITEM_SAVEFILE                (CONFIG_ITEM_STRING | 0x0D)  /* Written file option */
#define CONFIG_ITEM_DIRECTORY               (CONFIG_ITEM_STRING | 0x0E)  /* Directory option */
#define CONFIG_ITEM_FONT                    (CONFIG_ITEM_STRING | 0x0F)  /* Font option */

/* reduce specific type to type class */
#define CONFIG_CLASS(x) ((x) & ~0x1F)

/* is proper option, not a special hint type? */
#define CONFIG_ITEM(x) (((x) & ~0xF) != 0)

#define IsConfigStringType(type) \
    (((type) & CONFIG_ITEM_STRING) != 0)
#define IsConfigIntegerType(type) \
    (((type) & CONFIG_ITEM_INTEGER) != 0)
#define IsConfigFloatType(type) \
    ((type) == CONFIG_ITEM_FLOAT)


namespace FFmpeg {
    static struct
    {
        struct param** list;
        size_t count;
    } config = { NULL, 0 };

    inline param* param_Find(const char* name)
    {
        param* const* p;

        assert(name != NULL);
        p = (param* const*)bsearch(name, config.list, config.count, sizeof(*p), confnamecmp);
        return (p != NULL) ? *p : NULL;
    }

    inline int confcmp(const void* a, const void* b)
    {
        const param* const* ca = (const param* const*)a, * const* cb = (const param* const*)b;

        return strcmp((*ca)->item.psz_name, (*cb)->item.psz_name);
    }

    inline int confnamecmp(const void* key, const void* elem)
    {
        const struct param* const* conf = (const struct param* const*)elem;

        return strcmp((const char*)key, (*conf)->item.psz_name);
    }

    inline int64_t config_GetInt(const char* name)
    {
        const struct param* param = param_Find(name);

        /* sanity checks */
        assert(param != NULL);
        assert(IsConfigIntegerType(param->item.i_type));

        return atomic_load_explicit(&param->value.i, std::memory_order_relaxed);
    }

    inline float config_GetFloat(const char* name)
    {
        const param* _param = (const param*)param_Find(name);

        /* sanity checks */
        assert(_param != NULL);
        assert(IsConfigFloatType(_param->item.i_type));

        return atomic_load_explicit(&_param->value.f, memory_order_relaxed);
    }

    inline char* config_GetPsz(const char* name)
    {
        const param* _param = param_Find(name);
        char* str;

        /* sanity checks */
        assert(_param != NULL);
        assert(IsConfigStringType(_param->item.i_type));

        /* return a copy of the string */
        rcu_read_lock();
        str = atomic_load_explicit(&_param->value.str, memory_order_acquire);
        if (str != NULL)
            str = strdup(str);
        rcu_read_unlock();
        return str;
    }
} // namespace FFmpeg