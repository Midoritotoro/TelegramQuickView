#pragma once 

#include <cstdlib>
#include <cstdint>

#ifndef __STDC_VERSION_STDCKDINT_H__
# define __STDC_VERSION_STDCKDINT_H__ 202311L

# if defined(__GNUC__) || defined(__clang__)
#  define ckd_add(r, a, b) __builtin_add_overflow(a, b, r)
#  define ckd_sub(r, a, b) __builtin_sub_overflow(a, b, r)
#  define ckd_mul(r, a, b) __builtin_mul_overflow(a, b, r)
# else
#  include <numeric>

#  define __ckd_unsigned(suffix, type, MAX) \
inline bool __ckd_add_##suffix(type *r, type a, type b) \
{ \
    *r = a + b; \
    return ((type)(a + b)) < a; \
} \
\
inline bool __ckd_sub_##suffix(type *r, type a, type b) \
{ \
    *r = a - b; \
    return a < b; \
} \
\
inline bool __ckd_mul_##suffix(type *r, type a, type b) \
{ \
    *r = a * b; \
    return b > 0 && a > (MAX / b); \
}

#  define __ckd_func(op, r, a, b) \
    _Generic (*(r), \
        unsigned char:      __ckd_##op##_uc((unsigned char *)(r), a, b), \
        unsigned short:     __ckd_##op##_us((unsigned short *)(r), a, b), \
        unsigned int:       __ckd_##op##_ui((unsigned int *)(r), a, b), \
        unsigned long:      __ckd_##op##_ul((unsigned long *)(r), a, b), \
        unsigned long long: __ckd_##op##_ull((unsigned long long *)(r), a, b))

__ckd_unsigned(uc, unsigned char, UCHAR_MAX)
__ckd_unsigned(us, unsigned short, USHRT_MAX)
__ckd_unsigned(ui, unsigned int, UINT_MAX)
__ckd_unsigned(ul, unsigned long, ULONG_MAX)
__ckd_unsigned(ull, unsigned long long, ULLONG_MAX)

#  define ckd_add(r, a, b) __ckd_func(add, r, a, b)
#  define ckd_sub(r, a, b) __ckd_func(sub, r, a, b)
#  define ckd_mul(r, a, b) __ckd_func(mul, r, a, b)
# endif
#endif /* __STDC_VERSION_STDCKDINT_H__ */


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
#define likely(p)                           __builtin_expect(!!(p), 1)
#define unlikely(p)                         __builtin_expect(!!(p), 0)
#define unreachable()                       __builtin_unreachable()
#elif defined(_MSC_VER)
#define likely(p)                           (!!(p))
#define unlikely(p)                         (!!(p))
#define unreachable()                       (__assume(0))
#else
#define likely(p)                           (!!(p))
#define unlikely(p)                         (!!(p))
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