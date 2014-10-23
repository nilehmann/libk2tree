/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#if defined(_WIN32) || defined(WIN32)
#define LIBDIVIDE_WINDOWS 1
#endif

#if defined(_MSC_VER)
#define LIBDIVIDE_VC 1
#endif

#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <type_traits>

#if ! LIBDIVIDE_HAS_STDINT_TYPES && (! LIBDIVIDE_VC || _MSC_VER >= 1600)
/* Only Visual C++ 2010 and later include stdint.h */
#include <stdint.h>
#define LIBDIVIDE_HAS_STDINT_TYPES 1
#endif

#if ! LIBDIVIDE_HAS_STDINT_TYPES
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
typedef __int8 int8_t;
typedef unsigned __int8 uint8_t;
#endif

#if LIBDIVIDE_USE_SSE2
    #include <emmintrin.h>
#endif

#if LIBDIVIDE_VC
    #include <intrin.h>
#endif

#ifndef __has_builtin
#define __has_builtin(x) 0  // Compatibility with non-clang compilers.
#endif

#ifdef __ICC
#define HAS_INT128_T 0
#else
#define HAS_INT128_T __LP64__
#endif

#if defined(__x86_64__) || defined(_WIN64) || defined(_M_64)
#define LIBDIVIDE_IS_X86_64 1
#endif

#if defined(__i386__)
#define LIBDIVIDE_IS_i386 1
#endif

#if __GNUC__ || __clang__
#define LIBDIVIDE_GCC_STYLE_ASM 1
#endif


// libdivide may use the pmuldq (vector signed 32x32->64 mult instruction) which
// is in SSE 4.1.  However, signed multiplication can be emulated efficiently
// with unsigned multiplication, and SSE 4.1 is currently rare, so it is OK to
// not turn this on 
#ifdef LIBDIVIDE_USE_SSE4_1
#include <smmintrin.h>
#endif

namespace libremainder {

// We place this section in an anonymous namespace so that the functions are
// only visible to files that #include this header and don't get external
// linkage. At least that's the theory.
namespace {
int32_t count_leading_zeros32(uint32_t val) {
#if __GNUC__ || __has_builtin(__builtin_clzll)
  // Fast way to count leading zeros
  return __builtin_clz(val);    
#elif LIBDIVIDE_VC
  unsigned long result;
  if (_BitScanReverse(&result, val))
    return 31 - result;
  return 0;
#else
  // Dorky way to count leading zeros. Note that this hangs for val = 0
  int32_t result = 0;
  while (! (val & (1U << 31))) {
    val <<= 1;
    result++;
  }
  return result;    
#endif
}
int32_t count_trailing_zeros32(uint32_t val) {
#if __GNUC__ || __has_builtin(__builtin_ctz)
  // Fast way to count trailing zeros
  return __builtin_ctz(val);
#elif LIBDIVIDE_VC
  unsigned long result;
  if (_BitScanForward(&result, val))
    return result;
  return 0;
#else
  // Dorky way to count trailing zeros. Note that this hangs for val = 0!
  int32_t result = 0;
  val = (val ^ (val - 1)) >> 1;  // Set v's trailing 0s to 1s and zero rest
  while (val) {
    val >>= 1;
    result++;
  }
  return result;
#endif
}

// _64_div_32_to_32: divides a 64 bit uint {u1, u0} by a 32 bit
// uint {v}. The result must fit in 32 bits. Returns the quotient directly
// and the remainder in *r
uint32_t _64_div_32_to_32(uint32_t u1, uint32_t u0,
                                    uint32_t v, 
                                    uint32_t *r) {
#if (LIBDIVIDE_IS_i386 || LIBDIVIDE_IS_X86_64) && LIBDIVIDE_GCC_STYLE_ASM
  uint32_t result;
  __asm__("divl %[v]"
          : "=a"(result), "=d"(*r)
          : [v] "r"(v), "a"(u0), "d"(u1)
          );
  return result;
#else
  uint64_t n = (((uint64_t)u1) << 32) | u0;
  uint32_t result = (uint32_t)(n / v);
  *r = (uint32_t)(n - result * (uint64_t)v);
  return result;
#endif
}
int32_t count_trailing_zeros64(uint64_t val) {
#if __LP64__ && (__GNUC__ || __has_builtin(__builtin_ctzll))
  // Fast way to count trailing zeros.  Note that we disable this in 32 bit
  // because gcc does something horrible - it calls through to a dynamically
  // bound function.
  return __builtin_ctzll(val);
#elif LIBDIVIDE_VC && _WIN64
  unsigned long result;
  if (_BitScanForward64(&result, val))
    return result;
  return 0;
#else
  // Pretty good way to count trailing zeros. Note that this hangs for
  // val = 0
  uint32_t lo = val & 0xFFFFFFFF;
  if (lo != 0)
    return count_trailing_zeros32(lo);
  return 32 + trailing_zeros32(val >> 32);
#endif
}

int32_t count_leading_zeros64(uint64_t val) {
#if __GNUC__ || __has_builtin(__builtin_clzll)
  // Fast way to count leading zeros
  return __builtin_clzll(val);
#elif LIBDIVIDE_VC && _WIN64
  unsigned long result;
  if (_BitScanReverse64(&result, val))
      return 63 - result;
  return 0;
#else
  // Dorky way to count leading zeros. Note that this hangs for val = 0
  int32_t result = 0;
  while (! (val & (1ULL << 63))) {
    val <<= 1;
    result++;
  }
  return result;
#endif
}

uint64_t _128_div_64_to_64(uint64_t u1, uint64_t u0,
                           uint64_t v,
                           uint64_t *r) {
#if LIBDIVIDE_IS_X86_64 && LIBDIVIDE_GCC_STYLE_ASM
  //u0 -> rax
  //u1 -> rdx
  //divq
  uint64_t result;
  __asm__("divq %[v]"
          : "=a"(result), "=d"(*r)
          : [v] "r"(v), "a"(u0), "d"(u1)
          );
  return result;
#else
 // Code taken from Hacker's Delight, http://www.hackersdelight.org/HDcode/divlu.c.
 // License permits inclusion here per http://www.hackersdelight.org/permissions.htm
  const uint64_t b = (1ULL << 32); // Number base (16 bits).
  uint64_t un1, un0,               // Norm. dividend LSD's.
  vn1, vn0,                        // Norm. divisor digits.
  q1, q0,                          // Quotient digits.
  un64, un21, un10,                // Dividend digit pairs.
  rhat;                            // A remainder.
  int s;                           // Shift amount for norm.
  
  if (u1 >= v) {            // If overflow, set rem.
    if (r != NULL)          // to an impossible value,
      *r = (uint64_t)(-1);  // and return the largest
    return (uint64_t)(-1);  // possible quotient.
  }    
  
  // count leading zeros
  s = count_leading_zeros64(v); // 0 <= s <= 63.
  if (s > 0) {
    v = v << s;               // Normalize divisor.
    un64 = (u1 << s) | ((u0 >> (64 - s)) & (-s >> 31));
    un10 = u0 << s;           // Shift dividend left.
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
  
  q0 = un21/vn1;               // Compute the second
  rhat = un21 - q0*vn1;        // quotient digit, q0.
again2:
  if (q0 >= b || q0*vn0 > b*rhat + un0) {
    q0 = q0 - 1;
    rhat = rhat + vn1;
    if (rhat < b) goto again2;
  }
  
  if (r != NULL)                      // If remainder is wanted,
    *r = (un21*b + un0 - q0*v) >> s;  // return it.
  return q1*b + q0;
#endif
}

static inline uint32_t mullhi_u32(uint32_t x, uint32_t y) {
  uint64_t xl = x, yl = y;
  uint64_t rl = xl * yl;
  return (uint32_t)(rl >> 32);
}
 
static uint64_t mullhi_u64(uint64_t x, uint64_t y) {
#if HAS_INT128_T
  __uint128_t xl = x, yl = y;
  __uint128_t rl = xl * yl;
  return (uint64_t)(rl >> 64);
#else
  //full 128 bits are x0 * y0 + (x0 * y1 << 32) + (x1 * y0 << 32) + (x1 * y1 << 64)
  const uint32_t mask = 0xFFFFFFFF;
  const uint32_t x0 = (uint32_t)(x & mask), x1 = (uint32_t)(x >> 32);
  const uint32_t y0 = (uint32_t)(y & mask), y1 = (uint32_t)(y >> 32);
  const uint32_t x0y0_hi = mullhi_u32(x0, y0);
  const uint64_t x0y1 = x0 * (uint64_t)y1;
  const uint64_t x1y0 = x1 * (uint64_t)y0;
  const uint64_t x1y1 = x1 * (uint64_t)y1;
  
  uint64_t temp = x1y0 + x0y0_hi;
  uint64_t temp_lo = temp & mask, temp_hi = temp >> 32;
  return x1y1 + temp_hi + ((temp_lo + x0y1) >> 32);
#endif
}

const int32_t LIBDIVIDE_ADD_MARKER = 0x40;


template<typename T>
struct is_unsigned : std::integral_constant<bool, false> {};
template<> struct is_unsigned<uint32_t> : std::integral_constant<bool, true> {};
template<> struct is_unsigned<uint64_t> : std::integral_constant<bool, true> {};

template<typename T>
struct is_signed : std::integral_constant<bool, false> {};
template<> struct is_unsigned<int32_t> : std::integral_constant<bool, true> {};
template<> struct is_unsigned<int64_t> : std::integral_constant<bool, true> {};

template<typename T> struct libdivide_unsigned_trait;
template<> struct libdivide_unsigned_trait<uint32_t> {
  static const uint8_t shift_mask = 0x1F;
  static const uint8_t shift_path = 0x80;
  static const uint32_t one = 1;

  static uint32_t divide(uint32_t u1, uint32_t u2, uint32_t v, uint32_t *r) {
    return _64_div_32_to_32(u1, u2, v, r);
  }

  static uint32_t mullhi(uint32_t x, uint32_t y) {
    return mullhi_u32(x, y);
  }

  static int32_t count_trailing_zeros(uint32_t d) {
    return count_trailing_zeros32(d);
  }

  static int32_t count_leading_zeros(uint32_t d) {
    return count_leading_zeros32(d);
  }
};

template<> struct libdivide_unsigned_trait<uint64_t> {
  static const uint8_t shift_mask = 0x3F;
  static const uint8_t shift_path = 0x80;
  static const uint64_t one = 1ULL;

  static uint64_t divide(uint64_t u1, uint64_t u2, uint64_t v, uint64_t *r) {
    return _128_div_64_to_64(u1, u2, v, r);
  }

  static uint64_t mullhi(uint64_t x, uint64_t y) {
    return mullhi_u64(x, y);
  }

  static int32_t count_trailing_zeros(uint64_t d) {
    return count_trailing_zeros64(d);
  }

  static int32_t count_leading_zeros(uint64_t d) {
    return count_leading_zeros64(d);
  }
};

}  // namespace

// Explanation of "more" field: bit 6 is whether to use shift path.  If we are
// using the shift path, bit 7 is whether the divisor is negative in the
// signed case; in the unsigned case it is 0. Bits 0-4 is shift value
// (for shift path or mult path). In 32 bit case, bit 5 is always 0. We use
// bit 7 as the "negative divisor indicator" so that we can use sign
// extension to efficiently go to a full-width -1.
//u32: [0-4] shift value
     //[5] ignored
     //[6] add indicator
     //[7] shift path
     
//s32: [0-4] shift value
     //[5] shift path
     //[6] add indicator
     //[7] indicates negative divisor

//u64: [0-5] shift value
     //[6] add indicator
     //[7] shift path

//s64: [0-5] shift value
     //[6] add indicator
     //[7] indicates negative divisor
     //magic number of 0 indicates shift path (we ran out of bits!)
     //
//template<typename T> class Divider;



template<typename T>
class UnsignedImpl {
  static_assert(is_unsigned<T>::value, "Template parameter is not "
                                       "an unsigned type");
  typedef libdivide_unsigned_trait<T> LDUTrait; 
 public:
  typedef T int_type;

  UnsignedImpl() : d_(1), magic_(0), more_(LDUTrait::shift_path) {}

  UnsignedImpl(T d) : d_(d), magic_(0), more_(0) {
    T magic;
    uint8_t more;
    if ((d & (d - 1)) == 0) {
      // magic_ = 0;
      // We store d - 1 for remainder operation
      magic = d - 1;
      more = LDUTrait::count_trailing_zeros(d) | LDUTrait::shift_path;
    } else {
      const uint32_t 
      floor_log_2_d = LDUTrait::shift_mask - LDUTrait::count_leading_zeros(d);
        
      T rem, proposed_m;
      proposed_m = LDUTrait::divide(LDUTrait::one << floor_log_2_d, 0, d, &rem);

      assert(rem > 0 && rem < d);
      const T e = d - rem;
        
      // This power works if e < 2**floor_log_2_d.
      if (e < (LDUTrait::one << floor_log_2_d)) {
        more = floor_log_2_d;
      } else {
        // We have to use the general (bits + 1) algorithm.  We need to compute
        // (2**power) / d. However, we already have (2**(power-1))/d and its
        // remainder.  By doubling both, and then correcting the remainder, we
        // can compute the larger division.

        //don't care about overflow here - in fact, we expect it
        proposed_m += proposed_m; 
        const T twice_rem = rem + rem;
        if (twice_rem >= d || twice_rem < rem) proposed_m += 1;
          more = floor_log_2_d | LIBDIVIDE_ADD_MARKER;
      }
      magic = 1 + proposed_m;
      // more's shift should in general be ceil_log_2_d. But if we used the
      // smaller power, we subtract one from the shift because we're using
      // the smaller power. If we're using the larger power, we subtract 
      // one from the shift because it's taken care of by the add indicator.
      // So floor_log_2_d happens to be correct in both cases.
    }
    magic_ = magic;
    more_ = more;
  }

  T GetNumber() const {
    return d_;
  }

  int GetDivisionAlgorithm() const {
    if (more_ & LDUTrait::shift_path) return 0;
    else if (! (more_ & LIBDIVIDE_ADD_MARKER)) return 1;
    else return 2;
  }

  T PerformDivision(T numer) const {
    switch (GetDivisionAlgorithm()) {
      case 0: return DivisionAlg0(numer);
      case 1: return DivisionAlg1(numer);
      case 2: return DivisionAlg2(numer);
      default: exit(1);
    }
  }

  T DivisionAlg0(T numer) const {
    assert(GetDivisionAlgorithm() == 0);
    return numer >> (more_ & LDUTrait::shift_mask);
  }

  T DivisionAlg1(T numer) const {
    assert(GetDivisionAlgorithm() == 1);
    T q = LDUTrait::mullhi(magic_, numer);
    return q >> more_;
  }

  T DivisionAlg2(T numer) const {
    assert(GetDivisionAlgorithm() == 2);
    T q = LDUTrait::mullhi(magic_, numer);
    T t = ((numer - q) >> 1) + q;
    return t >> (more_ & LDUTrait::shift_mask);
  } 

  int GetRemainderAlgorithm() const {
    if (more_ & LDUTrait::shift_path) return 0;
    else return 1;
  }

  T PerformRemainder(T numer) const {
    switch (UnsignedImpl<T>::GetRemainderAlgorithm()) {
      case 0: return RemainderAlg0(numer);
      case 1: return RemainderAlg1(numer);
      default: exit(1);
    }
  }

  T RemainderAlg0(T numer) const {
    assert(GetRemainderAlgorithm() == 0);
    return numer & magic_;
  }

  T RemainderAlg1(T numer) const {
    assert(GetRemainderAlgorithm() == 1);
    return numer >= d_ ? numer - d_*PerformDivision(numer) : numer;
  }

 protected:
  T d_;
  T magic_;
  int8_t more_;
};


template<typename T>
struct get_divider_impl {
  static_assert(is_unsigned<T>::value or is_signed<T>::value,
                "Type has no divider implementation");
  //typedef typename std::conditional<is_unsigned<T>::value,
                           //UnsignedImpl<T>,
                           //SignedImpl<T>>::type type;
  typedef typename std::enable_if<is_unsigned<T>::value,
                                  UnsignedImpl<T>>::type type;
};

template<typename T>
class Divider {
  typedef typename get_divider_impl<T>::type Impl;
  Impl impl_;
 public:
  Divider() : impl_() {}
  
  Divider(T d) : impl_(d) {}

  T PerformDivision(T numer) const {
    return impl_.PerformDivision(numer);
  }

  T PerformRemainder(T numer) const {
    return impl_.PerformRemainder(numer);
  }

  explicit operator T() const {
    return impl_.GetNumber();
  }
};


template<typename T>
T operator/(T numer, const Divider<T> &denom) {
  return denom.PerformDivision(numer);
}
template<typename T>
T& operator/=(T &numer, const Divider<T> &denom) {
  return numer = numer / denom;
}

template<typename T>
T operator%(T numer, const Divider<T> &denom) {
  return denom.PerformRemainder(numer);
}

template<typename T>
T& operator%=(T &numer, const Divider<T> &denom) {
  return numer = numer % denom;
}

}  // namespace libremainder
