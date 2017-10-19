// libdivide.h
// Copyright 2010 - 2016 ridiculous_fish
//
// libdivide is dual-licensed under the Boost or zlib
// licenses. You may use libdivide under the terms of
// either of these. See LICENSE.txt for more details.

#if defined(_MSC_VER)
#define LIBDIVIDE_VC 1
// disable warning C4146: unary minus operator applied to
// unsigned type, result still unsigned
#pragma warning(disable: 4146)
#endif


#include <stdint.h>

#define NULL 0

/*
#if ! LIBDIVIDE_HAS_STDINT_TYPES
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
typedef __int8 int8_t;
typedef unsigned __int8 uint8_t;
#endif
*/


#ifndef __has_builtin
#define __has_builtin(x) 0 // Compatibility with non-clang compilers.
#endif

#if defined(__SIZEOF_INT128__)
#define HAS_INT128_T 1
#endif

#if defined(__x86_64__) || defined(_WIN64) || defined(_M_X64)
#define LIBDIVIDE_IS_X86_64 1
#endif

#if defined(__i386__)
#define LIBDIVIDE_IS_i386 1
#endif

#if __GNUC__ || __clang__
#define LIBDIVIDE_GCC_STYLE_ASM 1
#endif

#define LIBDIVIDE_ASSERT(x)

// Explanation of "more" field: bit 6 is whether to use shift path. If we are
// using the shift path, bit 7 is whether the divisor is negative in the signed
// case; in the unsigned case it is 0. Bits 0-4 is shift value (for shift
// path or mult path).  In 32 bit case, bit 5 is always 0. We use bit 7 as the
// "negative divisor indicator" so that we can use sign extension to
// efficiently go to a full-width -1.
//
// u32: [0-4] shift value
//      [5] ignored
//      [6] add indicator
//      [7] shift path
//
// s32: [0-4] shift value
//      [5] shift path
//      [6] add indicator
//      [7] indicates negative divisor
//
// u64: [0-5] shift value
//      [6] add indicator
//      [7] shift path
//
// s64: [0-5] shift value
//      [6] add indicator
//      [7] indicates negative divisor
//      magic number of 0 indicates shift path (we ran out of bits!)
//
// In s32 and s64 branchfree modes, the magic number is negated according to
// whether the divisor is negated. In branchfree strategy, it is not negated.

enum {
    LIBDIVIDE_32_SHIFT_MASK = 0x1F,
    LIBDIVIDE_64_SHIFT_MASK = 0x3F,
    LIBDIVIDE_ADD_MARKER = 0x40,
    LIBDIVIDE_U32_SHIFT_PATH = 0x80,
    LIBDIVIDE_U64_SHIFT_PATH = 0x80,
    LIBDIVIDE_S32_SHIFT_PATH = 0x20,
    LIBDIVIDE_NEGATIVE_DIVISOR = 0x80    
};

// pack divider structs to prevent compilers from padding.
// This reduces memory usage by up to 43% when using a large
// array of libdivide dividers and improves performance
// by up to 10% because of reduced memory bandwidth.
#pragma pack(push, 1)

struct libdivide_u32_t {
    uint32_t magic;
    uint8_t more;
};

struct libdivide_s32_t {
    int32_t magic;
    uint8_t more;
};

struct libdivide_u64_t {
    uint64_t magic;
    uint8_t more;
};

struct libdivide_s64_t {
    int64_t magic;
    uint8_t more;
};

struct libdivide_u32_branchfree_t {
    uint32_t magic;
    uint8_t more;
};

struct libdivide_s32_branchfree_t {
    int32_t magic;
    uint8_t more;
};

struct libdivide_u64_branchfree_t {
    uint64_t magic;
    uint8_t more;
};

struct libdivide_s64_branchfree_t {
    int64_t magic;
    uint8_t more;
};

#pragma pack(pop)

#ifndef LIBDIVIDE_API
        #define LIBDIVIDE_API static inline
#endif

LIBDIVIDE_API struct libdivide_s32_t libdivide_s32_gen(int32_t y);
LIBDIVIDE_API struct libdivide_u32_t libdivide_u32_gen(uint32_t y);
LIBDIVIDE_API struct libdivide_s64_t libdivide_s64_gen(int64_t y);
LIBDIVIDE_API struct libdivide_u64_t libdivide_u64_gen(uint64_t y);

LIBDIVIDE_API struct libdivide_s32_branchfree_t libdivide_s32_branchfree_gen(int32_t y);
LIBDIVIDE_API struct libdivide_u32_branchfree_t libdivide_u32_branchfree_gen(uint32_t y);
LIBDIVIDE_API struct libdivide_s64_branchfree_t libdivide_s64_branchfree_gen(int64_t y);
LIBDIVIDE_API struct libdivide_u64_branchfree_t libdivide_u64_branchfree_gen(uint64_t y);
    
LIBDIVIDE_API int32_t  libdivide_s32_do(int32_t numer, const struct libdivide_s32_t *denom);
LIBDIVIDE_API uint32_t libdivide_u32_do(uint32_t numer, const struct libdivide_u32_t *denom);
LIBDIVIDE_API int64_t  libdivide_s64_do(int64_t numer, const struct libdivide_s64_t *denom);
LIBDIVIDE_API uint64_t libdivide_u64_do(uint64_t y, const struct libdivide_u64_t *denom);

LIBDIVIDE_API int32_t  libdivide_s32_branchfree_do(int32_t numer, const struct libdivide_s32_branchfree_t *denom);
LIBDIVIDE_API uint32_t libdivide_u32_branchfree_do(uint32_t numer, const struct libdivide_u32_branchfree_t *denom);
LIBDIVIDE_API int64_t  libdivide_s64_branchfree_do(int64_t numer, const struct libdivide_s64_branchfree_t *denom);
LIBDIVIDE_API uint64_t libdivide_u64_branchfree_do(uint64_t y, const struct libdivide_u64_branchfree_t *denom);
    
LIBDIVIDE_API int32_t  libdivide_s32_recover(const struct libdivide_s32_t *denom);
LIBDIVIDE_API uint32_t libdivide_u32_recover(const struct libdivide_u32_t *denom);
LIBDIVIDE_API int64_t  libdivide_s64_recover(const struct libdivide_s64_t *denom);
LIBDIVIDE_API uint64_t libdivide_u64_recover(const struct libdivide_u64_t *denom);

LIBDIVIDE_API int32_t  libdivide_s32_branchfree_recover(const struct libdivide_s32_branchfree_t *denom);
LIBDIVIDE_API uint32_t libdivide_u32_branchfree_recover(const struct libdivide_u32_branchfree_t *denom);
LIBDIVIDE_API int64_t  libdivide_s64_branchfree_recover(const struct libdivide_s64_branchfree_t *denom);
LIBDIVIDE_API uint64_t libdivide_u64_branchfree_recover(const struct libdivide_u64_branchfree_t *denom);

LIBDIVIDE_API int libdivide_u32_get_algorithm(const struct libdivide_u32_t *denom);
LIBDIVIDE_API uint32_t libdivide_u32_do_alg0(uint32_t numer, const struct libdivide_u32_t *denom);
LIBDIVIDE_API uint32_t libdivide_u32_do_alg1(uint32_t numer, const struct libdivide_u32_t *denom);
LIBDIVIDE_API uint32_t libdivide_u32_do_alg2(uint32_t numer, const struct libdivide_u32_t *denom);
 
LIBDIVIDE_API int libdivide_u64_get_algorithm(const struct libdivide_u64_t *denom);
LIBDIVIDE_API uint64_t libdivide_u64_do_alg0(uint64_t numer, const struct libdivide_u64_t *denom);
LIBDIVIDE_API uint64_t libdivide_u64_do_alg1(uint64_t numer, const struct libdivide_u64_t *denom);
LIBDIVIDE_API uint64_t libdivide_u64_do_alg2(uint64_t numer, const struct libdivide_u64_t *denom);
 
LIBDIVIDE_API int libdivide_s32_get_algorithm(const struct libdivide_s32_t *denom);
LIBDIVIDE_API int32_t libdivide_s32_do_alg0(int32_t numer, const struct libdivide_s32_t *denom);
LIBDIVIDE_API int32_t libdivide_s32_do_alg1(int32_t numer, const struct libdivide_s32_t *denom);
LIBDIVIDE_API int32_t libdivide_s32_do_alg2(int32_t numer, const struct libdivide_s32_t *denom);
LIBDIVIDE_API int32_t libdivide_s32_do_alg3(int32_t numer, const struct libdivide_s32_t *denom);
LIBDIVIDE_API int32_t libdivide_s32_do_alg4(int32_t numer, const struct libdivide_s32_t *denom);
 
LIBDIVIDE_API int libdivide_s64_get_algorithm(const struct libdivide_s64_t *denom);
LIBDIVIDE_API int64_t libdivide_s64_do_alg0(int64_t numer, const struct libdivide_s64_t *denom);
LIBDIVIDE_API int64_t libdivide_s64_do_alg1(int64_t numer, const struct libdivide_s64_t *denom);
LIBDIVIDE_API int64_t libdivide_s64_do_alg2(int64_t numer, const struct libdivide_s64_t *denom);
LIBDIVIDE_API int64_t libdivide_s64_do_alg3(int64_t numer, const struct libdivide_s64_t *denom);
LIBDIVIDE_API int64_t libdivide_s64_do_alg4(int64_t numer, const struct libdivide_s64_t *denom);

//////// Internal Utility Functions

static inline uint32_t libdivide__mullhi_u32(uint32_t x, uint32_t y) {
    uint64_t xl = x, yl = y;
    uint64_t rl = xl * yl;
    return (uint32_t)(rl >> 32);
}
 
static uint64_t libdivide__mullhi_u64(uint64_t x, uint64_t y) {
#if LIBDIVIDE_VC && LIBDIVIDE_IS_X86_64
    return __umulh(x, y);
#elif HAS_INT128_T
    __uint128_t xl = x, yl = y;
    __uint128_t rl = xl * yl;
    return (uint64_t)(rl >> 64);
#else
    // full 128 bits are x0 * y0 + (x0 * y1 << 32) + (x1 * y0 << 32) + (x1 * y1 << 64)
    const uint32_t mask = 0xFFFFFFFF;
    const uint32_t x0 = (uint32_t)(x & mask), x1 = (uint32_t)(x >> 32);
    const uint32_t y0 = (uint32_t)(y & mask), y1 = (uint32_t)(y >> 32);
    const uint32_t x0y0_hi = libdivide__mullhi_u32(x0, y0);
    const uint64_t x0y1 = x0 * (uint64_t)y1;
    const uint64_t x1y0 = x1 * (uint64_t)y0;
    const uint64_t x1y1 = x1 * (uint64_t)y1;
    
    uint64_t temp = x1y0 + x0y0_hi;
    uint64_t temp_lo = temp & mask, temp_hi = temp >> 32;
    return x1y1 + temp_hi + ((temp_lo + x0y1) >> 32);
#endif
}
 
static inline int64_t libdivide__mullhi_s64(int64_t x, int64_t y) {
#if LIBDIVIDE_VC && LIBDIVIDE_IS_X86_64
    return __mulh(x, y);
#elif HAS_INT128_T
    __int128_t xl = x, yl = y;
    __int128_t rl = xl * yl;
    return (int64_t)(rl >> 64);    
#else
    // full 128 bits are x0 * y0 + (x0 * y1 << 32) + (x1 * y0 << 32) + (x1 * y1 << 64)
    const uint32_t mask = 0xFFFFFFFF;
    const uint32_t x0 = (uint32_t)(x & mask), y0 = (uint32_t)(y & mask);
    const int32_t x1 = (int32_t)(x >> 32), y1 = (int32_t)(y >> 32);
    const uint32_t x0y0_hi = libdivide__mullhi_u32(x0, y0);
    const int64_t t = x1*(int64_t)y0 + x0y0_hi;
    const int64_t w1 = x0*(int64_t)y1 + (t & mask);
    return x1*(int64_t)y1 + (t >> 32) + (w1 >> 32);
#endif
}
    
 
static inline int32_t libdivide__count_leading_zeros32(uint32_t val) {
#if __GNUC__ || __has_builtin(__builtin_clz)
    // Fast way to count leading zeros
    return __builtin_clz(val);    
#elif LIBDIVIDE_VC
    unsigned long result;
    if (_BitScanReverse(&result, val)) {
        return 31 - result;
    }
    return 0;
#else
  int32_t result = 0;
  uint32_t hi = 1U << 31;

  while (~val & hi) {
      hi >>= 1;
      result++;
  }
  return result;
#endif
}
    
static inline int32_t libdivide__count_leading_zeros64(uint64_t val) {
#if __GNUC__ || __has_builtin(__builtin_clzll)
    // Fast way to count leading zeros
    return __builtin_clzll(val);
#elif LIBDIVIDE_VC && _WIN64
    unsigned long result;
    if (_BitScanReverse64(&result, val)) {
        return 63 - result;
    }
    return 0;
#else
    uint32_t hi = val >> 32;
    uint32_t lo = val & 0xFFFFFFFF;
    if (hi != 0) return libdivide__count_leading_zeros32(hi);
    return 32 + libdivide__count_leading_zeros32(lo);
#endif
}

// libdivide_64_div_32_to_32: divides a 64 bit uint {u1, u0} by a 32 bit
// uint {v}. The result must fit in 32 bits.
// Returns the quotient directly and the remainder in *r
#if (LIBDIVIDE_IS_i386 || LIBDIVIDE_IS_X86_64) && LIBDIVIDE_GCC_STYLE_ASM
static uint32_t libdivide_64_div_32_to_32(uint32_t u1, uint32_t u0, uint32_t v, uint32_t *r) {
    uint32_t result;
    __asm__("divl %[v]"
            : "=a"(result), "=d"(*r)
            : [v] "r"(v), "a"(u0), "d"(u1)
            );
    return result;
}
#else
static uint32_t libdivide_64_div_32_to_32(uint32_t u1, uint32_t u0, uint32_t v, uint32_t *r) {
    uint64_t n = (((uint64_t)u1) << 32) | u0;
    uint32_t result = (uint32_t)(n / v);
    *r = (uint32_t)(n - result * (uint64_t)v);
    return result;
}
#endif
    
#if LIBDIVIDE_IS_X86_64 && LIBDIVIDE_GCC_STYLE_ASM
static uint64_t libdivide_128_div_64_to_64(uint64_t u1, uint64_t u0, uint64_t v, uint64_t *r) {
    // u0 -> rax
    // u1 -> rdx
    // divq
    uint64_t result;
    __asm__("divq %[v]"
            : "=a"(result), "=d"(*r)
            : [v] "r"(v), "a"(u0), "d"(u1)
            );
    return result;
}
#else

// Code taken from Hacker's Delight:
// http://www.hackersdelight.org/HDcode/divlu.c.
// License permits inclusion here per:
// http://www.hackersdelight.org/permissions.htm

static uint64_t libdivide_128_div_64_to_64(uint64_t u1, uint64_t u0, uint64_t v, uint64_t *r) {    
    const uint64_t b = (1ULL << 32); // Number base (16 bits).
    uint64_t un1, un0,  // Norm. dividend LSD's.
    vn1, vn0,           // Norm. divisor digits.
    q1, q0,             // Quotient digits.
    un64, un21, un10,   // Dividend digit pairs.
    rhat;               // A remainder.
    int s;              // Shift amount for norm.
    
    if (u1 >= v) {                 // If overflow, set rem.
        if (r != NULL)             // to an impossible value,
            *r = (uint64_t) -1;    // and return the largest
        return (uint64_t) -1;      // possible quotient.
    }

    // count leading zeros
    s = libdivide__count_leading_zeros64(v); // 0 <= s <= 63.
    if (s > 0) {
        v = v << s;         // Normalize divisor.
        un64 = (u1 << s) | ((u0 >> (64 - s)) & (-s >> 31));
        un10 = u0 << s;     // Shift dividend left.
    } else {
        // Avoid undefined behavior.
        un64 = u1 | u0;
        un10 = u0;
    }

    vn1 = v >> 32;            // Break divisor up into
    vn0 = v & 0xFFFFFFFF;     // two 32-bit digits.

    un1 = un10 >> 32;         // Break right half of
    un0 = un10 & 0xFFFFFFFF;  // dividend into two digits.

    q1 = un64/vn1;            // Compute the first
    rhat = un64 - q1*vn1;     // quotient digit, q1.
again1:
    if (q1 >= b || q1*vn0 > b*rhat + un1) {
        q1 = q1 - 1;
        rhat = rhat + vn1;
        if (rhat < b) goto again1;
    }

    un21 = un64*b + un1 - q1*v;  // Multiply and subtract.

    q0 = un21/vn1;            // Compute the second
    rhat = un21 - q0*vn1;     // quotient digit, q0.
again2:
    if (q0 >= b || q0*vn0 > b*rhat + un0) {
        q0 = q0 - 1;
        rhat = rhat + vn1;
        if (rhat < b) goto again2;
    }

    if (r != NULL)                          // If remainder is wanted,
        *r = (un21*b + un0 - q0*v) >> s;    // return it.
    return q1*b + q0;
}
#endif

// Bitshift a u128 in place, left (signed_shift > 0) or right (signed_shift < 0)
static inline void libdivide_u128_shift(uint64_t *u1, uint64_t *u0, int32_t signed_shift)
{
    if (signed_shift > 0) {
        uint32_t shift = signed_shift;
        *u1 <<= shift;
        *u1 |= *u0 >> (64 - shift);
        *u0 <<= shift;
    } else {
        uint32_t shift = -signed_shift;
        *u0 >>= shift;
        *u0 |= *u1 << (64 - shift);
        *u1 >>= shift;
    }
}
    
// Computes a 128 / 128 -> 64 bit division, with a 128 bit remainder.
static uint64_t libdivide_128_div_128_to_64(uint64_t u_hi, uint64_t u_lo, uint64_t v_hi, uint64_t v_lo, uint64_t *r_hi, uint64_t *r_lo) {
#if HAS_INT128_T
    __uint128_t ufull = u_hi;
    ufull = (ufull << 64) | u_lo;
    __uint128_t vfull = v_hi;
    vfull = (vfull << 64) | v_lo;
    __uint128_t remainder = ufull % vfull;
    *r_lo = (uint64_t)remainder;
    *r_hi = (uint64_t)(remainder >> 64);
    return (uint64_t)(ufull / vfull);
#else
    // Adapted from "Unsigned Doubleword Division" in Hacker's Delight
    // We want to compute u / v
    typedef struct { uint64_t hi; uint64_t lo; } u128_t;
    u128_t u = {u_hi, u_lo};
    u128_t v = {v_hi, v_lo};
    if (v.hi == 0) {
        // divisor v is a 64 bit value, so we just need one 128/64 division
        // Note that we are simpler than Hacker's Delight here, because we know
        // the quotient fits in 64 bits whereas Hacker's Delight demands a full
        // 128 bit quotient
        *r_hi = 0;
        return libdivide_128_div_64_to_64(u.hi, u.lo, v.lo, r_lo);
    }
    // Here v >= 2**64
    // We know that v.hi != 0, so count leading zeros is OK
    // We have 0 <= n <= 63
    uint32_t n = libdivide__count_leading_zeros64(v.hi);
    
    // Normalize the divisor so its MSB is 1
    u128_t v1t = v;
    libdivide_u128_shift(&v1t.hi, &v1t.lo, n);
    uint64_t v1 = v1t.hi; // i.e. v1 = v1t >> 64
    
    // To ensure no overflow
    u128_t u1 = u;
    libdivide_u128_shift(&u1.hi, &u1.lo, -1);
    
    // Get quotient from divide unsigned insn.
    uint64_t rem_ignored;
    uint64_t q1 = libdivide_128_div_64_to_64(u1.hi, u1.lo, v1, &rem_ignored);
    
    // Undo normalization and division of u by 2.
    u128_t q0 = {0, q1};
    libdivide_u128_shift(&q0.hi, &q0.lo, n);
    libdivide_u128_shift(&q0.hi, &q0.lo, -63);
    
    // Make q0 correct or too small by 1
    // Equivalent to `if (q0 != 0) q0 = q0 - 1;`
    if (q0.hi != 0 || q0.lo != 0) {
        q0.hi -= (q0.lo == 0); // borrow
        q0.lo -= 1;
    }
    
    // Now q0 is correct.
    // Compute q0 * v as q0v
    // = (q0.hi<<64 + q0.lo) * (v.hi<<64 + v.lo)
    // = (q0.hi*v.hi<<128 + q0.hi*v.lo<<64 + q0.lo*v.hi<<64 + q0.lo*v.lo)
    // Each term is 128 bit
    // High half of full product (upper 128 bits!) are dropped
    u128_t q0v = {0, 0};
    q0v.hi = q0.hi*v.lo + q0.lo*v.hi + libdivide__mullhi_u64(q0.lo, v.lo);
    q0v.lo = q0.lo*v.lo;
    
    // Compute u - q0v as u_q0v
    // This is the remainder
    u128_t u_q0v = u;
    u_q0v.hi -= q0v.hi + (u.lo < q0v.lo); // second term is borrow
    u_q0v.lo -= q0v.lo;
    
    // Check if u_q0v >= v
    // This checks if our remainder is larger than the divisor
    if ((u_q0v.hi > v.hi) || (u_q0v.hi == v.hi && u_q0v.lo >= v.lo)) {
        // Increment q0
        q0.lo += 1;
        q0.hi += (q0.lo == 0); // carry
        
        // Subtract v from remainder
        u_q0v.hi -= v.hi + (u_q0v.lo < v.lo);
        u_q0v.lo -= v.lo;
    }
        
    *r_hi = u_q0v.hi;
    *r_lo = u_q0v.lo;
    
    LIBDIVIDE_ASSERT(q0.hi == 0);
    return q0.lo;
#endif
}

#ifndef LIBDIVIDE_HEADER_ONLY
  
////////// UINT32

static inline struct libdivide_u32_t libdivide_internal_u32_gen(uint32_t d, int branchfree) {
    // 1 is not supported with branchfree algorithm
    LIBDIVIDE_ASSERT(!branchfree || d != 1);

    struct libdivide_u32_t result;
    const uint32_t floor_log_2_d = 31 - libdivide__count_leading_zeros32(d);
    if ((d & (d - 1)) == 0) {
        // Power of 2
        if (! branchfree) {
            result.magic = 0;
            result.more = floor_log_2_d | LIBDIVIDE_U32_SHIFT_PATH;
        } else {
            // We want a magic number of 2**32 and a shift of floor_log_2_d
            // but one of the shifts is taken up by LIBDIVIDE_ADD_MARKER, so we
            // subtract 1 from the shift
            result.magic = 0;
            result.more = (floor_log_2_d-1) | LIBDIVIDE_ADD_MARKER;
        }
    } else {
        uint8_t more;
        uint32_t rem, proposed_m;
        proposed_m = libdivide_64_div_32_to_32(1U << floor_log_2_d, 0, d, &rem);
        
        LIBDIVIDE_ASSERT(rem > 0 && rem < d);
        const uint32_t e = d - rem;
        
        // This power works if e < 2**floor_log_2_d.
        if (!branchfree && (e < (1U << floor_log_2_d))) {
            // This power works
            more = floor_log_2_d;
        } else {
            // We have to use the general 33-bit algorithm.  We need to compute
            // (2**power) / d. However, we already have (2**(power-1))/d and
            // its remainder.  By doubling both, and then correcting the
            // remainder, we can compute the larger division.
            // don't care about overflow here - in fact, we expect it
            proposed_m += proposed_m;
            const uint32_t twice_rem = rem + rem;
            if (twice_rem >= d || twice_rem < rem) proposed_m += 1;
            more = floor_log_2_d | LIBDIVIDE_ADD_MARKER;
        }
        result.magic = 1 + proposed_m;
        result.more = more;
        // result.more's shift should in general be ceil_log_2_d. But if we
        // used the smaller power, we subtract one from the shift because we're
        // using the smaller power. If we're using the larger power, we
        // subtract one from the shift because it's taken care of by the add
        // indicator. So floor_log_2_d happens to be correct in both cases.
    }
    return result;
}
    
struct libdivide_u32_t libdivide_u32_gen(uint32_t d) {
    return libdivide_internal_u32_gen(d, 0);
}
    
struct libdivide_u32_branchfree_t libdivide_u32_branchfree_gen(uint32_t d) {
    struct libdivide_u32_t tmp = libdivide_internal_u32_gen(d, 1);
    struct libdivide_u32_branchfree_t ret = {tmp.magic, (uint8_t)(tmp.more & LIBDIVIDE_32_SHIFT_MASK)};
    return ret;
}

uint32_t libdivide_u32_do(uint32_t numer, const struct libdivide_u32_t *denom) {
    uint8_t more = denom->more;
    if (more & LIBDIVIDE_U32_SHIFT_PATH) {
        return numer >> (more & LIBDIVIDE_32_SHIFT_MASK);
    }
    else {
        uint32_t q = libdivide__mullhi_u32(denom->magic, numer);
        if (more & LIBDIVIDE_ADD_MARKER) {
            uint32_t t = ((numer - q) >> 1) + q;
            return t >> (more & LIBDIVIDE_32_SHIFT_MASK);
        }
        else {
            return q >> more; // all upper bits are 0 - don't need to mask them off
        }
    }
}

uint32_t libdivide_u32_recover(const struct libdivide_u32_t *denom) {
    uint8_t more = denom->more;
    uint8_t shift = more & LIBDIVIDE_32_SHIFT_MASK;
    if (more & LIBDIVIDE_U32_SHIFT_PATH) {
        return 1U << shift;
    } else if (! (more & LIBDIVIDE_ADD_MARKER)) {
        // We compute q = n/d = n*m / 2^(32 + shift)
        // Therefore we have d = 2^(32 + shift) / m
        // We need to ceil it.
        // We know d is not a power of 2, so m is not a power of 2,
        // so we can just add 1 to the floor
        uint32_t hi_dividend = 1U << shift;
        uint32_t rem_ignored;
        return 1 + libdivide_64_div_32_to_32(hi_dividend, 0, denom->magic, &rem_ignored);
    } else {
        // Here we wish to compute d = 2^(32+shift+1)/(m+2^32).
        // Notice (m + 2^32) is a 33 bit number. Use 64 bit division for now
        // Also note that shift may be as high as 31, so shift + 1 will
        // overflow. So we have to compute it as 2^(32+shift)/(m+2^32), and
        // then double the quotient and remainder.
        // TODO: do something better than 64 bit math
        uint64_t half_n = 1ULL << (32 + shift);
        uint64_t d = (1ULL << 32) | denom->magic;
        // Note that the quotient is guaranteed <= 32 bits, but the remainder
        // may need 33!
        uint32_t half_q = (uint32_t)(half_n / d);
        uint64_t rem = half_n % d;
        // We computed 2^(32+shift)/(m+2^32)
        // Need to double it, and then add 1 to the quotient if doubling th
        // remainder would increase the quotient.
        // Note that rem<<1 cannot overflow, since rem < d and d is 33 bits
        uint32_t full_q = half_q + half_q + ((rem<<1) >= d);
        
        // We rounded down in gen unless we're a power of 2 (i.e. in branchfree case)
        // We can detect that by looking at m. If m zero, we're a power of 2
        return full_q + (denom->magic != 0);
    }
}

uint32_t libdivide_u32_branchfree_recover(const struct libdivide_u32_branchfree_t *denom) {
    struct libdivide_u32_t denom_u32 = {denom->magic, (uint8_t)(denom->more | LIBDIVIDE_ADD_MARKER)};
    return libdivide_u32_recover(&denom_u32);
}

int libdivide_u32_get_algorithm(const struct libdivide_u32_t *denom) {
    uint8_t more = denom->more;
    if (more & LIBDIVIDE_U32_SHIFT_PATH) return 0;
    else if (! (more & LIBDIVIDE_ADD_MARKER)) return 1;
    else return 2;
}
 
uint32_t libdivide_u32_do_alg0(uint32_t numer, const struct libdivide_u32_t *denom) {
    return numer >> (denom->more & LIBDIVIDE_32_SHIFT_MASK);
}
 
uint32_t libdivide_u32_do_alg1(uint32_t numer, const struct libdivide_u32_t *denom) {
    uint32_t q = libdivide__mullhi_u32(denom->magic, numer);
    return q >> denom->more;
}    
 
uint32_t libdivide_u32_do_alg2(uint32_t numer, const struct libdivide_u32_t *denom) {
    // denom->add != 0
    uint32_t q = libdivide__mullhi_u32(denom->magic, numer);
    uint32_t t = ((numer - q) >> 1) + q;
    // Note that this mask is typically free. Only the low bits are meaningful
    // to a shift, so compilers can optimize out this AND.
    return t >> (denom->more & LIBDIVIDE_32_SHIFT_MASK);
}

uint32_t libdivide_u32_branchfree_do(uint32_t numer, const struct libdivide_u32_branchfree_t *denom) {
    // same as alg 2
    uint32_t q = libdivide__mullhi_u32(denom->magic, numer);
    uint32_t t = ((numer - q) >> 1) + q;
    return t >> denom->more;
}
    
 
/////////// UINT64

static inline struct libdivide_u64_t libdivide_internal_u64_gen(uint64_t d, int branchfree) {
    // 1 is not supported with branchfree algorithm
    LIBDIVIDE_ASSERT(!branchfree || d != 1);
    
    struct libdivide_u64_t result;
    const uint32_t floor_log_2_d = 63 - libdivide__count_leading_zeros64(d);
    if ((d & (d - 1)) == 0) {
        // Power of 2
        if (! branchfree) {
            result.magic = 0;
            result.more = floor_log_2_d | LIBDIVIDE_U64_SHIFT_PATH;
        } else {
            // We want a magic number of 2**64 and a shift of floor_log_2_d
            // but one of the shifts is taken up by LIBDIVIDE_ADD_MARKER, so we
            // subtract 1 from the shift
            result.magic = 0;
            result.more = (floor_log_2_d-1) | LIBDIVIDE_ADD_MARKER;
        }
    } else {
        uint64_t proposed_m, rem;
        uint8_t more;
        proposed_m = libdivide_128_div_64_to_64(1ULL << floor_log_2_d, 0, d, &rem); // == (1 << (64 + floor_log_2_d)) / d
        
        LIBDIVIDE_ASSERT(rem > 0 && rem < d);
        const uint64_t e = d - rem;
        
        // This power works if e < 2**floor_log_2_d.
        if (!branchfree && e < (1ULL << floor_log_2_d)) {
            // This power works
            more = floor_log_2_d;
        } else {
            // We have to use the general 65-bit algorithm.  We need to compute
            // (2**power) / d. However, we already have (2**(power-1))/d and
            // its remainder. By doubling both, and then correcting the
            // remainder, we can compute the larger division.
            // don't care about overflow here - in fact, we expect it
            proposed_m += proposed_m;
            const uint64_t twice_rem = rem + rem;
            if (twice_rem >= d || twice_rem < rem) proposed_m += 1;
                more = floor_log_2_d | LIBDIVIDE_ADD_MARKER;
        }
        result.magic = 1 + proposed_m;
        result.more = more;
        // result.more's shift should in general be ceil_log_2_d. But if we
        // used the smaller power, we subtract one from the shift because we're
        // using the smaller power. If we're using the larger power, we
        // subtract one from the shift because it's taken care of by the add
        // indicator. So floor_log_2_d happens to be correct in both cases,
        // which is why we do it outside of the if statement.
    }
    return result;
}

struct libdivide_u64_t libdivide_u64_gen(uint64_t d)
{
    return libdivide_internal_u64_gen(d, 0);
}

struct libdivide_u64_branchfree_t libdivide_u64_branchfree_gen(uint64_t d)
{
    struct libdivide_u64_t tmp = libdivide_internal_u64_gen(d, 1);
    struct libdivide_u64_branchfree_t ret = {tmp.magic, (uint8_t)(tmp.more & LIBDIVIDE_64_SHIFT_MASK)};
    return ret;
}

uint64_t libdivide_u64_do(uint64_t numer, const struct libdivide_u64_t *denom) {
    uint8_t more = denom->more;
    if (more & LIBDIVIDE_U64_SHIFT_PATH) {
        return numer >> (more & LIBDIVIDE_64_SHIFT_MASK);
    }
    else {
        uint64_t q = libdivide__mullhi_u64(denom->magic, numer);
        if (more & LIBDIVIDE_ADD_MARKER) {
            uint64_t t = ((numer - q) >> 1) + q;
            return t >> (more & LIBDIVIDE_64_SHIFT_MASK);
        }
        else {
            return q >> more; // all upper bits are 0 - don't need to mask them off
        }
    }
}

uint64_t libdivide_u64_recover(const struct libdivide_u64_t *denom) {
    uint8_t more = denom->more;
    uint8_t shift = more & LIBDIVIDE_64_SHIFT_MASK;
    if (more & LIBDIVIDE_U64_SHIFT_PATH) {
        return 1ULL << shift;
    } else if (! (more & LIBDIVIDE_ADD_MARKER)) {
        // We compute q = n/d = n*m / 2^(64 + shift)
        // Therefore we have d = 2^(64 + shift) / m
        // We need to ceil it.
        // We know d is not a power of 2, so m is not a power of 2,
        // so we can just add 1 to the floor
        uint64_t hi_dividend = 1ULL << shift;
        uint64_t rem_ignored;
        return 1 + libdivide_128_div_64_to_64(hi_dividend, 0, denom->magic, &rem_ignored);
    } else {
        // Here we wish to compute d = 2^(64+shift+1)/(m+2^64).
        // Notice (m + 2^64) is a 65 bit number. This gets hairy. See
        // libdivide_u32_recover for more on what we do here.
        // TODO: do something better than 128 bit math
        
        // Hack: if d is not a power of 2, this is a 128/128->64 divide
        // If d is a power of 2, this may be a bigger divide
        // However we can optimize that easily
        if (denom->magic == 0) {
            // 2^(64 + shift + 1) / (2^64) == 2^(shift + 1)
            return 1ULL << (shift + 1);
        }
        
        // Full n is a (potentially) 129 bit value
        // half_n is a 128 bit value
        // Compute the hi half of half_n. Low half is 0.
        uint64_t half_n_hi = 1ULL << shift, half_n_lo = 0;
        // d is a 65 bit value. The high bit is always set to 1.
        const uint64_t d_hi = 1, d_lo = denom->magic;
        // Note that the quotient is guaranteed <= 64 bits,
        // but the remainder may need 65!
        uint64_t r_hi, r_lo;
        uint64_t half_q = libdivide_128_div_128_to_64(half_n_hi, half_n_lo, d_hi, d_lo, &r_hi, &r_lo);
        // We computed 2^(64+shift)/(m+2^64)
        // Double the remainder ('dr') and check if that is larger than d
        // Note that d is a 65 bit value, so r1 is small and so r1 + r1 cannot
        // overflow
        uint64_t dr_lo = r_lo + r_lo;
        uint64_t dr_hi = r_hi + r_hi + (dr_lo < r_lo); // last term is carry
        int dr_exceeds_d = (dr_hi > d_hi) || (dr_hi == d_hi && dr_lo >= d_lo);        
        uint64_t full_q = half_q + half_q + (dr_exceeds_d ? 1 : 0);
        return full_q + 1;
    }
}

uint64_t libdivide_u64_branchfree_recover(const struct libdivide_u64_branchfree_t *denom) {
    struct libdivide_u64_t denom_u64 = {denom->magic, (uint8_t)(denom->more | LIBDIVIDE_ADD_MARKER)};
    return libdivide_u64_recover(&denom_u64);
}
    
int libdivide_u64_get_algorithm(const struct libdivide_u64_t *denom) {
    uint8_t more = denom->more;
    if (more & LIBDIVIDE_U64_SHIFT_PATH) return 0;
    else if (! (more & LIBDIVIDE_ADD_MARKER)) return 1;
    else return 2;
}
 
uint64_t libdivide_u64_do_alg0(uint64_t numer, const struct libdivide_u64_t *denom) {
    return numer >> (denom->more & LIBDIVIDE_64_SHIFT_MASK);    
}
 
uint64_t libdivide_u64_do_alg1(uint64_t numer, const struct libdivide_u64_t *denom) {
    uint64_t q = libdivide__mullhi_u64(denom->magic, numer);
    return q >> denom->more;
}
 
uint64_t libdivide_u64_do_alg2(uint64_t numer, const struct libdivide_u64_t *denom) {
    uint64_t q = libdivide__mullhi_u64(denom->magic, numer);
    uint64_t t = ((numer - q) >> 1) + q;
    return t >> (denom->more & LIBDIVIDE_64_SHIFT_MASK);
}

uint64_t libdivide_u64_branchfree_do(uint64_t numer, const struct libdivide_u64_branchfree_t *denom) {
    // same as alg 2
    uint64_t q = libdivide__mullhi_u64(denom->magic, numer);
    uint64_t t = ((numer - q) >> 1) + q;
    return t >> denom->more;
}

 
/////////// SINT32

static inline int32_t libdivide__mullhi_s32(int32_t x, int32_t y) {
    int64_t xl = x, yl = y;
    int64_t rl = xl * yl;
    return (int32_t)(rl >> 32); // needs to be arithmetic shift
}

static inline struct libdivide_s32_t libdivide_internal_s32_gen(int32_t d, int branchfree) {
    // branchfree cannot support or -1
    LIBDIVIDE_ASSERT(!branchfree || (d != 1 && d != -1));
    
    struct libdivide_s32_t result;
    
    // If d is a power of 2, or negative a power of 2, we have to use a shift.
    // This is especially important because the magic algorithm fails for -1.
    // To check if d is a power of 2 or its inverse, it suffices to check
    // whether its absolute value has exactly one bit set. This works even for
    // INT_MIN, because abs(INT_MIN) == INT_MIN, and INT_MIN has one bit set
    // and is a power of 2.
    uint32_t ud = (uint32_t)d;
    uint32_t absD = (d < 0 ? -ud : ud); // gcc optimizes this to the fast abs trick
    const uint32_t floor_log_2_d = 31 - libdivide__count_leading_zeros32(absD);
    // check if exactly one bit is set,
    // don't care if absD is 0 since that's divide by zero
    if ((absD & (absD - 1)) == 0) {
        // Branchfree and normal paths are exactly the same
        result.magic = 0;
        result.more = floor_log_2_d | (d < 0 ? LIBDIVIDE_NEGATIVE_DIVISOR : 0) | LIBDIVIDE_S32_SHIFT_PATH;
    } else {
        LIBDIVIDE_ASSERT(floor_log_2_d >= 1);    
        
        uint8_t more;
        // the dividend here is 2**(floor_log_2_d + 31), so the low 32 bit word
        // is 0 and the high word is floor_log_2_d - 1
        uint32_t rem, proposed_m;
        proposed_m = libdivide_64_div_32_to_32(1U << (floor_log_2_d - 1), 0, absD, &rem);
        const uint32_t e = absD - rem;
        
        // We are going to start with a power of floor_log_2_d - 1.
        // This works if works if e < 2**floor_log_2_d.
        if (!branchfree && e < (1U << floor_log_2_d)) {
            // This power works
            more = floor_log_2_d - 1;
        } else {
            // We need to go one higher. This should not make proposed_m
            // overflow, but it will make it negative when interpreted as an
            // int32_t.
            proposed_m += proposed_m;
            const uint32_t twice_rem = rem + rem;
            if (twice_rem >= absD || twice_rem < rem) proposed_m += 1;
            more = floor_log_2_d | LIBDIVIDE_ADD_MARKER;
        }
        
        proposed_m += 1;
        int32_t magic = (int32_t)proposed_m;
        
        // Mark if we are negative. Note we only negate the magic number in the
        // branchfull case.
        if (d < 0) {
            more |= LIBDIVIDE_NEGATIVE_DIVISOR;
            if (! branchfree) {
                magic = -magic;
            }
        }
        
        result.more = more;
        result.magic = magic;
    }
    return result;
}

LIBDIVIDE_API struct libdivide_s32_t libdivide_s32_gen(int32_t d) {
    return libdivide_internal_s32_gen(d, 0);
}

LIBDIVIDE_API struct libdivide_s32_branchfree_t libdivide_s32_branchfree_gen(int32_t d) {
    struct libdivide_s32_t tmp = libdivide_internal_s32_gen(d, 1);
    struct libdivide_s32_branchfree_t result = {tmp.magic, tmp.more};
    return result;
}

int32_t libdivide_s32_do(int32_t numer, const struct libdivide_s32_t *denom) {
    uint8_t more = denom->more;
    uint32_t sign = (int8_t)more >> 7;
    if (more & LIBDIVIDE_S32_SHIFT_PATH) {
        uint8_t shifter = more & LIBDIVIDE_32_SHIFT_MASK;
        uint32_t uq = (uint32_t)(numer + ((numer >> 31) & ((1U << shifter) - 1)));
        int32_t q = (int32_t)uq;
        q = q >> shifter;
        q = (q ^ sign) - sign;
        return q;
    } else {
        uint32_t uq = (uint32_t)libdivide__mullhi_s32(denom->magic, numer);
        if (more & LIBDIVIDE_ADD_MARKER) {
            // must be arithmetic shift and then sign extend
            int32_t sign = (int8_t)more >> 7;
            // q += (more < 0 ? -numer : numer), casts to avoid UB
            uq += (((uint32_t)numer ^ sign) - sign);
        }
        int32_t q = (int32_t)uq;
        q >>= more & LIBDIVIDE_32_SHIFT_MASK;
        q += (q < 0);
        return q;
    }
}

int32_t libdivide_s32_branchfree_do(int32_t numer, const struct libdivide_s32_branchfree_t *denom) {
    uint8_t more = denom->more;
    uint8_t shift = more & LIBDIVIDE_32_SHIFT_MASK;
    // must be arithmetic shift and then sign extend
    int32_t sign = (int8_t)more >> 7;
    
    int32_t magic = denom->magic;
    int32_t q = libdivide__mullhi_s32(magic, numer);
    q += numer;
    
    // If q is non-negative, we have nothing to do
    // If q is negative, we want to add either (2**shift)-1 if d is a power of
    // 2, or (2**shift) if it is not a power of 2
    uint32_t is_power_of_2 = !!(more & LIBDIVIDE_S32_SHIFT_PATH);
    uint32_t q_sign = (uint32_t)(q >> 31);
    q += q_sign & ((1 << shift) - is_power_of_2);
    
    // Now arithmetic right shift
    q >>= shift;
    
    // Negate if needed
    q = ((q ^ sign) - sign);
    
    return q;
}

int32_t libdivide_s32_recover(const struct libdivide_s32_t *denom) {
    uint8_t more = denom->more;
    uint8_t shift = more & LIBDIVIDE_32_SHIFT_MASK;
    if (more & LIBDIVIDE_S32_SHIFT_PATH) {
        uint32_t absD = 1U << shift;
        if (more & LIBDIVIDE_NEGATIVE_DIVISOR) {
            absD = -absD;
        }
        return (int32_t)absD;
    } else {
        // Unsigned math is much easier
        // We negate the magic number only in the branchfull case, and we don't
        // know which case we're in. However we have enough information to
        // determine the correct sign of the magic number. The divisor was
        // negative if LIBDIVIDE_NEGATIVE_DIVISOR is set. If ADD_MARKER is set,
        // the magic number's sign is opposite that of the divisor.
        // We want to compute the positive magic number.
        int negative_divisor = (more & LIBDIVIDE_NEGATIVE_DIVISOR);
        int magic_was_negated = (more & LIBDIVIDE_ADD_MARKER) ? denom->magic > 0 : denom->magic < 0;
        
        // Handle the power of 2 case (including branchfree)
        if (denom->magic == 0) {
            int32_t result = 1 << shift;
            return negative_divisor ? -result : result;
        }
        
        uint32_t d = (uint32_t)(magic_was_negated ? -denom->magic : denom->magic);
        uint64_t n = 1ULL << (32 + shift); // Note that the shift cannot exceed 30
        uint32_t q = (uint32_t)(n / d);
        int32_t result = (int32_t)q;
        result += 1;
        return negative_divisor ? -result : result;
    }
}

int32_t libdivide_s32_branchfree_recover(const struct libdivide_s32_branchfree_t *denom) {
    return libdivide_s32_recover((const struct libdivide_s32_t *)denom);
}

int libdivide_s32_get_algorithm(const struct libdivide_s32_t *denom) {
    uint8_t more = denom->more;
    int positiveDivisor = ! (more & LIBDIVIDE_NEGATIVE_DIVISOR);
    if (more & LIBDIVIDE_S32_SHIFT_PATH) return (positiveDivisor ? 0 : 1);
    else if (more & LIBDIVIDE_ADD_MARKER) return (positiveDivisor ? 2 : 3); 
    else return 4;
}
 
int32_t libdivide_s32_do_alg0(int32_t numer, const struct libdivide_s32_t *denom) {
    uint8_t shifter = denom->more & LIBDIVIDE_32_SHIFT_MASK;
    int32_t q = numer + ((numer >> 31) & ((1U << shifter) - 1));
    return q >> shifter;
}
 
int32_t libdivide_s32_do_alg1(int32_t numer, const struct libdivide_s32_t *denom) {
    uint8_t shifter = denom->more & LIBDIVIDE_32_SHIFT_MASK;
    int32_t q = numer + ((numer >> 31) & ((1U << shifter) - 1));
    return - (q >> shifter);
}
 
int32_t libdivide_s32_do_alg2(int32_t numer, const struct libdivide_s32_t *denom) {
    int32_t q = libdivide__mullhi_s32(denom->magic, numer);
    q += numer;
    q >>= denom->more & LIBDIVIDE_32_SHIFT_MASK;
    q += (q < 0);    
    return q;
}
 
int32_t libdivide_s32_do_alg3(int32_t numer, const struct libdivide_s32_t *denom) {
    int32_t q = libdivide__mullhi_s32(denom->magic, numer);
    q -= numer;
    q >>= denom->more & LIBDIVIDE_32_SHIFT_MASK;
    q += (q < 0);    
    return q;
}
 
int32_t libdivide_s32_do_alg4(int32_t numer, const struct libdivide_s32_t *denom) {
    int32_t q = libdivide__mullhi_s32(denom->magic, numer);
    q >>= denom->more & LIBDIVIDE_32_SHIFT_MASK;
    q += (q < 0);
    return q;
}


///////////// SINT64

static inline struct libdivide_s64_t libdivide_internal_s64_gen(int64_t d, int branchfree) {
    LIBDIVIDE_ASSERT(!branchfree || (d != 1 && d != -1));
    struct libdivide_s64_t result;
    
    // If d is a power of 2, or negative a power of 2, we have to use a shift.
    // This is especially important because the magic algorithm fails for -1.
    // To check if d is a power of 2 or its inverse, it suffices to check
    // whether its absolute value has exactly one bit set.  This works even for
    // INT_MIN, because abs(INT_MIN) == INT_MIN, and INT_MIN has one bit set
    // and is a power of 2.
    const uint64_t ud = (uint64_t)d;
    const uint64_t absD = (d < 0 ? -ud : ud); // gcc optimizes this to the fast abs trick
    const uint32_t floor_log_2_d = 63 - libdivide__count_leading_zeros64(absD);
    // check if exactly one bit is set,
    // don't care if absD is 0 since that's divide by zero
    if ((absD & (absD - 1)) == 0) {
        // Branchfree and non-branchfree cases are the same
        result.magic = 0;
        result.more = floor_log_2_d | (d < 0 ? LIBDIVIDE_NEGATIVE_DIVISOR : 0);
    } else {
        // the dividend here is 2**(floor_log_2_d + 63), so the low 64 bit word
        // is 0 and the high word is floor_log_2_d - 1
        uint8_t more;
        uint64_t rem, proposed_m;
        proposed_m = libdivide_128_div_64_to_64(1ULL << (floor_log_2_d - 1), 0, absD, &rem);
        const uint64_t e = absD - rem;
        
        // We are going to start with a power of floor_log_2_d - 1.
        // This works if works if e < 2**floor_log_2_d.
        if (!branchfree && e < (1ULL << floor_log_2_d)) {
            // This power works
            more = floor_log_2_d - 1;
        } else {
            // We need to go one higher. This should not make proposed_m
            // overflow, but it will make it negative when interpreted as an
            // int32_t.
            proposed_m += proposed_m;
            const uint64_t twice_rem = rem + rem;
            if (twice_rem >= absD || twice_rem < rem) proposed_m += 1;
            // note that we only set the LIBDIVIDE_NEGATIVE_DIVISOR bit if we
            // also set ADD_MARKER this is an annoying optimization that
            // enables algorithm #4 to avoid the mask. However we always set it
            // in the branchfree case
            more = floor_log_2_d | LIBDIVIDE_ADD_MARKER;
        }
        proposed_m += 1;
        int64_t magic = (int64_t)proposed_m;
        
        // Mark if we are negative
        if (d < 0) {
            more |= LIBDIVIDE_NEGATIVE_DIVISOR;
            if (! branchfree) {
                magic = -magic;
            }
        }
        
        result.more = more;
        result.magic = magic;
    }
    return result;
}

struct libdivide_s64_t libdivide_s64_gen(int64_t d) {
    return libdivide_internal_s64_gen(d, 0);
}

struct libdivide_s64_branchfree_t libdivide_s64_branchfree_gen(int64_t d) {
    struct libdivide_s64_t tmp = libdivide_internal_s64_gen(d, 1);
    struct libdivide_s64_branchfree_t ret = {tmp.magic, tmp.more};
    return ret;
}

int64_t libdivide_s64_do(int64_t numer, const struct libdivide_s64_t *denom) {
    uint8_t more = denom->more;
    int64_t magic = denom->magic;
    if (magic == 0) { //shift path
        uint32_t shifter = more & LIBDIVIDE_64_SHIFT_MASK;
        uint64_t uq = (uint64_t)numer + ((numer >> 63) & ((1ULL << shifter) - 1));
        int64_t q = (int64_t)uq;
        q = q >> shifter;
        // must be arithmetic shift and then sign-extend
        int64_t shiftMask = (int8_t)more >> 7;
        q = (q ^ shiftMask) - shiftMask;
        return q;
    } else {
        uint64_t uq = (uint64_t)libdivide__mullhi_s64(magic, numer);
        if (more & LIBDIVIDE_ADD_MARKER) {
            // must be arithmetic shift and then sign extend
            int64_t sign = (int8_t)more >> 7;
            uq += (((uint64_t)numer ^ sign) - sign);
        }
        int64_t q = (int64_t)uq;
        q >>= more & LIBDIVIDE_64_SHIFT_MASK;
        q += (q < 0);
        return q;
    }
}

int64_t libdivide_s64_branchfree_do(int64_t numer, const struct libdivide_s64_branchfree_t *denom) {
    uint8_t more = denom->more;
    uint32_t shift = more & LIBDIVIDE_64_SHIFT_MASK;
    // must be arithmetic shift and then sign extend
    int64_t sign = (int8_t)more >> 7;
    int64_t magic = denom->magic;
    int64_t q = libdivide__mullhi_s64(magic, numer);
    q += numer;
    
    // If q is non-negative, we have nothing to do.
    // If q is negative, we want to add either (2**shift)-1 if d is a power of
    // 2, or (2**shift) if it is not a power of 2.
    uint32_t is_power_of_2 = (magic == 0);
    uint64_t q_sign = (uint64_t)(q >> 63);
    q += q_sign & ((1ULL << shift) - is_power_of_2);
    
    // Arithmetic right shift
    q >>= shift;
    
    // Negate if needed
    q = ((q ^ sign) - sign);
    return q;
}

int64_t libdivide_s64_recover(const struct libdivide_s64_t *denom) {
    uint8_t more = denom->more;
    uint8_t shift = more & LIBDIVIDE_64_SHIFT_MASK;
    if (denom->magic == 0) { // shift path
        uint64_t absD = 1ULL << shift;
        if (more & LIBDIVIDE_NEGATIVE_DIVISOR) {
            absD = -absD;
        }
        return (int64_t)absD;
    } else {
        // Unsigned math is much easier
        int negative_divisor = (more & LIBDIVIDE_NEGATIVE_DIVISOR);
        int magic_was_negated = (more & LIBDIVIDE_ADD_MARKER) ? denom->magic > 0 : denom->magic < 0;

        uint64_t d = (uint64_t)(magic_was_negated ? -denom->magic : denom->magic);
        uint64_t n_hi = 1ULL << shift, n_lo = 0;
        uint64_t rem_ignored;
        uint64_t q = libdivide_128_div_64_to_64(n_hi, n_lo, d, &rem_ignored);
        int64_t result = (int64_t)(q + 1);
        if (negative_divisor) {
            result = -result;
        }
        return result;
    }
}

int64_t libdivide_s64_branchfree_recover(const struct libdivide_s64_branchfree_t *denom) {
    return libdivide_s64_recover((const struct libdivide_s64_t *)denom);
}

int libdivide_s64_get_algorithm(const struct libdivide_s64_t *denom) {
    uint8_t more = denom->more;
    int positiveDivisor = ! (more & LIBDIVIDE_NEGATIVE_DIVISOR);
    if (denom->magic == 0) return (positiveDivisor ? 0 : 1); // shift path
    else if (more & LIBDIVIDE_ADD_MARKER) return (positiveDivisor ? 2 : 3);
    else return 4;
}
 
int64_t libdivide_s64_do_alg0(int64_t numer, const struct libdivide_s64_t *denom) {
    uint32_t shifter = denom->more & LIBDIVIDE_64_SHIFT_MASK;
    int64_t q = numer + ((numer >> 63) & ((1ULL << shifter) - 1));
    return q >> shifter;    
}
 
int64_t libdivide_s64_do_alg1(int64_t numer, const struct libdivide_s64_t *denom) {
    // denom->shifter != -1 && demo->shiftMask != 0
    uint32_t shifter = denom->more & LIBDIVIDE_64_SHIFT_MASK;
    int64_t q = numer + ((numer >> 63) & ((1ULL << shifter) - 1));
    return - (q >> shifter);
}
 
int64_t libdivide_s64_do_alg2(int64_t numer, const struct libdivide_s64_t *denom) {
    int64_t q = libdivide__mullhi_s64(denom->magic, numer);
    q += numer;
    q >>= denom->more & LIBDIVIDE_64_SHIFT_MASK;
    q += (q < 0);
    return q;
}
    
int64_t libdivide_s64_do_alg3(int64_t numer, const struct libdivide_s64_t *denom) {
    int64_t q = libdivide__mullhi_s64(denom->magic, numer);
    q -= numer;
    q >>= denom->more & LIBDIVIDE_64_SHIFT_MASK;
    q += (q < 0);    
    return q;
}
    
int64_t libdivide_s64_do_alg4(int64_t numer, const struct libdivide_s64_t *denom) {
    int64_t q = libdivide__mullhi_s64(denom->magic, numer);
    q >>= denom->more & LIBDIVIDE_64_SHIFT_MASK;
    q += (q < 0);
    return q;   
}

    
#endif // LIBDIVIDE_HEADER_ONLY

