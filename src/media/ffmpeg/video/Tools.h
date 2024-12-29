#pragma once 

#include <cstdlib>
#include <cstdint>

#include <limits.h>

template <typename type>
bool ckd_add(type *r, type a, type b)
{
    *r = a + b;
    return ((type)(a + b)) < a;
}

template <typename type>
bool ckd_sub(type *r, type a, type b)
{
    *r = a - b;
    return a < b;
}

template <typename type>
bool ckd_mul(type *r, type a, type b)
{
    if (b == 0) return true;
    *r = a * b;
    return a > (INT_MAX / b);
}



#define container_of(ptr, type, member) \
    ((type *)(((char *)(ptr)) - offsetof(type, member)))

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define VLC_TICK_INVALID                    INT64_C(0)
#define VLC_TICK_0                          INT64_C(1)

#if defined (_WIN32)
#define aligned_free(ptr)                   _aligned_free(ptr)
#else
#define aligned_free(ptr)                   free(ptr)
#endif

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

#define PICTURE_SW_SIZE_MAX                 (UINT32_C(1) << 28) /* 256MB: 8K * 8K * 4*/

#define SUCCESS                             0
/** Unspecified error */
#define EGENERIC                            (-2 * (1 << (sizeof (int) * 8 - 2))) /* INT_MIN */
#ifdef WORDS_BIGENDIAN
#   define FOURCC( a, b, c, d ) \
        ( ((uint32_t)d) | ( ((uint32_t)c) << 8 ) \
           | ( ((uint32_t)b) << 16 ) | ( ((uint32_t)a) << 24 ) )
#   define TWOCC( a, b ) \
        ( (uint16_t)(b) | ( (uint16_t)(a) << 8 ) )

#else
#   define FOURCC( a, b, c, d ) \
        ( ((uint32_t)a) | ( ((uint32_t)b) << 8 ) \
           | ( ((uint32_t)c) << 16 ) | ( ((uint32_t)d) << 24 ) )
#   define TWOCC( a, b ) \
        ( (uint16_t)(a) | ( (uint16_t)(b) << 8 ) )

#endif

#define MINIMUM_WIDTH (32)