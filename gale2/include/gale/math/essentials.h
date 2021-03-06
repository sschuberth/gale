/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at https://github.com/sschuberth/gale/.
 *
 * Copyright (C) 2005-2011  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#pragma once

/**
 * \file
 * Optimized essential mathematical functions
 */

#include "../global/platform.h"

#ifndef _USE_MATH_DEFINES
    #define _USE_MATH_DEFINES
#endif
#include <cmath>

#include <float.h>
#include <limits.h>

#include <type_traits>

/**
 * \name Overloaded functions to calculate the absolute value
 */
//@{

/// Dummy for meta-templates as used by e.g. the Color class.
template<typename T>
typename std::enable_if<std::is_unsigned<T>::value, T>::type abs(T const x)
{
    return x;
}

//@}

namespace gale {

namespace math {

/**
 * Definition of data type specific mathematical constants.
 */
template<typename T>
struct Constants
{
    /// Definition for external access to the data type.
    typedef T Type;

    /// Returns the ratio of a circle's circumference to its diameter.
    static T PI() {
        return static_cast<T>(M_PI);
    }

    /// Returns the factor to convert degrees to radians.
    static T const& DEG_TO_RAD() {
        static T const f=PI()/T(180);
        return f;
    }

    /// Returns the factor to convert radians to degrees.
    static T const& RAD_TO_DEG() {
        static T const f=T(180)/PI();
        return f;
    }

    /// Returns the Golden Ratio as commonly used in arts.
    static T const& GOLDEN_RATIO() {
        static T const r=(1+sqrt(T(5)))/T(2);
        return r;
    }
};

/**
 * \name Type definitions for mathematical constants
 */
//@{

typedef Constants<double> Constd;
typedef Constants<float> Constf;

//@}

/**
 * Definition of data type specific numeric limits.
 */
template<typename T>
struct Numerics
{
    /// Definition for external access to the data type.
    typedef T Type;

    /// Returns the minimum / minimum positive value for integral / floating
    /// point data types.
    static T MIN();

    /// Returns the maximum value for the data type.
    static T MAX();

    /// Returns the smallest effective increment from 1.
    static T EPSILON();

    /// Returns the maximum difference to be tolerated when comparing to 0.
    static T ZERO_TOLERANCE();
};

/**
 * \name Type definitions for numeric limits
 */
//@{

typedef Numerics<double> Numd;
typedef Numerics<float> Numf;
typedef Numerics<int> Numi;
typedef Numerics<unsigned int> Numui;
typedef Numerics<short> Nums;
typedef Numerics<unsigned short> Numus;
typedef Numerics<signed char> Numb;
typedef Numerics<unsigned char> Numub;

//@}

/// \cond DOXYGEN_IGNORE
template<>
struct Numerics<double>
{
    static double MIN()                    { return DBL_MIN;      }
    static double MAX()                    { return DBL_MAX;      }
    static double EPSILON()                { return DBL_EPSILON;  }
    static double ZERO_TOLERANCE()         { return 1e-8;        }
};

template<>
struct Numerics<float>
{
    static float MIN()                     { return FLT_MIN;      }
    static float MAX()                     { return FLT_MAX;      }
    static float EPSILON()                 { return FLT_EPSILON;  }
    static float ZERO_TOLERANCE()          { return 1e-6f;       }
};

template<>
struct Numerics<int>
{
    static int MIN()                       { return INT_MIN;      }
    static int MAX()                       { return INT_MAX;      }
    static int EPSILON()                   { return 0;            }
    static int ZERO_TOLERANCE()            { return 0;            }
};

template<>
struct Numerics<unsigned int>
{
    static unsigned int MIN()              { return 0;            }
    static unsigned int MAX()              { return UINT_MAX;     }
    static unsigned int EPSILON()          { return 0;            }
    static unsigned int ZERO_TOLERANCE()   { return 0;            }
};

template<>
struct Numerics<short>
{
    static short MIN()                     { return SHRT_MIN;     }
    static short MAX()                     { return SHRT_MAX;     }
    static short EPSILON()                 { return 0;            }
    static short ZERO_TOLERANCE()          { return 0;            }
};

template<>
struct Numerics<unsigned short>
{
    static unsigned short MIN()            { return 0;            }
    static unsigned short MAX()            { return USHRT_MAX;    }
    static unsigned short EPSILON()        { return 0;            }
    static unsigned short ZERO_TOLERANCE() { return 0;            }
};

template<>
struct Numerics<signed char>
{
    static signed char MIN()               { return SCHAR_MIN;    }
    static signed char MAX()               { return SCHAR_MAX;    }
    static signed char EPSILON()           { return 0;            }
    static signed char ZERO_TOLERANCE()    { return 0;            }
};

template<>
struct Numerics<unsigned char>
{
    static unsigned char MIN()             { return 0;            }
    static unsigned char MAX()             { return UCHAR_MAX;    }
    static unsigned char EPSILON()         { return 0;            }
    static unsigned char ZERO_TOLERANCE()  { return 0;            }
};
/// \endcond

/**
 * \name Extremes determination related functions
 */
//@{

/// Clamps \a x to range [\a a,\a b], where \a a <= \a b.
template<typename T>
inline T clamp(T const x,T const a,T const b)
{
    return min(max(a,x),b);
}

/// Clamps \a x to range [\a a,\a b] or range [\a b,\a a].
template<typename T>
inline T clampSafe(T const x,T const a,T const b)
{
    return (a<=b)?clamp(x,a,b):clamp(x,b,a);
}

/// Wraps \a x to [0,\a range[.
template<typename T>
inline T wrap(T x,T const range)
{
    while (x<0) {
        x+=range;
    }
    while (x>=range) {
        x-=range;
    }
    return x;
}

/// Wraps \a x to range [\a a,\a b], where \a a <= \a b.
template<typename T>
inline T wrap(T x,T const a,T const b)
{
    for (;;) {
        if (x<a) {
            x=b-(a-x);
        }
        else if (x>b) {
            x=a+(x-b);
        }
        else {
            break;
        }
    }
    return x;
}

/// Wraps \a x to range [\a a,\a b] or range [\a b,\a a].
template<typename T>
inline T wrapSafe(T x,T const a,T const b)
{
    return (a<=b)?wrap(x,a,b):wrap(x,b,a);
}

/// Returns the minimum value among \a a and \a b.
template<typename T>
inline T min(T const a,T const b)
{
    return a<b?a:b;
}

/// Returns the minimum value among \a a, \a b and \a c.
template<typename T>
inline T min(T const a,T const b,T const c)
{
    return min(min(a,b),c);
}

/// Returns the minimum value among \a a, \a b, \a c and \a d.
template<typename T>
inline T min(T const a,T const b,T const c,T const d)
{
    return min(min(a,b,c),d);
}

/// Returns the maximum value among \a a and \a b.
template<typename T>
inline T max(T const a,T const b)
{
    return a>b?a:b;
}

/// Returns the maximum value among \a a, \a b and \a c.
template<typename T>
inline T max(T const a,T const b,T const c)
{
    return max(max(a,b),c);
}

/// Returns the maximum value among \a a, \a b, \a c and \a d.
template<typename T>
inline T max(T const a,T const b,T const c,T const d)
{
    return max(max(a,b,c),d);
}

/// Returns the sign of \a x, or 0 if \a x is 0.
template<typename T>
inline T sgn(T const x)
{
    // See http://graphics.stanford.edu/~seander/bithacks.html#CopyIntegerSign.
    return static_cast<T>(x>0)-static_cast<T>(x<0);
}

//@}

/**
 * \name Floating-point rounding related functions
 */
//@{

/// Rounds the 32-bit floating point value \a f to the nearest integer (to the
/// even one in case of ambiguity). This is the default rounding mode on x86
/// platforms, so the floating-point control word is not modified.
#ifdef G_ARCH_X86_64

G_INLINE long long roundToEven(float const f)
{
    // cvtss2si (64-bit variant)
    return _mm_cvtss_si64x(_mm_load_ss(&f));
}

#elif defined(GALE_USE_SSE)

G_INLINE int roundToEven(float const f)
{
    // cvtss2si (32-bit variant)
    return _mm_cvtss_si32(_mm_load_ss(&f));
}

#elif defined(G_COMP_GNUC)

G_INLINE int roundToEven(float const f)
{
    int i;
    __asm__(
        "fistpl %[i]\n\t"
        : [i] "=m" (i)  /* Output  */
        : "t" (f)       /* Input   */
        :               /* Clobber */
    );
    return i;
}

#elif defined(G_COMP_MSVC)

G_INLINE int roundToEven(float const f)
{
    // MSVC 8.0 does not support inline assembly on the x86-64 architecture.
    int i;
    __asm {
        fld f
        fistp i
    }
    return i;
}

#endif // G_ARCH_X86_64

/// Rounds the 64-bit floating point value \a d to the nearest integer (to the
/// even one in case of ambiguity). This is the default rounding mode on x86
/// platforms, so the floating-point control word is not modified.
#ifdef G_ARCH_X86_64

G_INLINE long long roundToEven(double const d)
{
    // cvtsd2si (64-bit variant)
    return _mm_cvtsd_si64x(_mm_load_sd(&d));
}

#elif defined(GALE_USE_SSE2)

G_INLINE int roundToEven(double const d)
{
    // cvtsd2si (32-bit variant)
    return _mm_cvtsd_si32(_mm_load_sd(&d));
}

#elif defined(G_COMP_GNUC)

G_INLINE long long roundToEven(double const d)
{
    long long i;
    __asm__(
        "fistpl %[i]\n\t"
        : [i] "=m" (i)  /* Output  */
        : "t" (d)       /* Input   */
        :               /* Clobber */
    );
    return i;
}

#elif defined(G_COMP_MSVC)

G_INLINE long long roundToEven(double const d)
{
    // MSVC 8.0 does not support inline assembly on the x86-64 architecture.
    long long i;
    __asm {
        fld d
        fistp i
    }
    return i;
}

#endif // G_ARCH_X86_64

/// Rounds the 32-bit floating point value \a f to the nearest integer towards
/// zero (truncating the number). This is the default ANSI C rounding mode; use
/// this method instead of a cast to an integer as it is faster.
#ifdef G_ARCH_X86_64

G_INLINE long long roundToZero(float const f)
{
    // cvttss2si (64-bit variant)
    return _mm_cvttss_si64x(_mm_load_ss(&f));
}

#elif defined(GALE_USE_SSE3) && defined(G_COMP_GNUC)

G_INLINE int roundToZero(float const f)
{
    int i;
    __asm__(
        "fisttpl %[i]\n\t"
        : [i] "=m" (i)  /* Output  */
        : "t" (f)       /* Input   */
        :               /* Clobber */
    );
    return i;
}

#elif defined(GALE_USE_SSE3) && defined(G_COMP_MSVC)

G_INLINE int roundToZero(float const f)
{
    // MSVC 8.0 does not support inline assembly on the x86-64 architecture.
    int i;
    __asm {
        fld f
        fisttp i
    }
    return i;
}

#elif defined(GALE_USE_SSE)

G_INLINE int roundToZero(float const f)
{
    // cvttss2si (32-bit variant)
    return _mm_cvttss_si32(_mm_load_ss(&f));
}

#else // G_ARCH_X86_64

G_INLINE int roundToZero(float const f)
{
    return static_cast<int>(f);
}

#endif // G_ARCH_X86_64

/// Rounds the 64-bit floating point value \a d to the nearest integer towards
/// zero (truncating the number). This is the default ANSI C rounding mode; use
/// this method instead of a cast to an integer as it is faster.
#ifdef G_ARCH_X86_64

G_INLINE long long roundToZero(double const d)
{
    // cvttsd2si (64-bit variant)
    return _mm_cvttsd_si64x(_mm_load_sd(&d));
}

#elif defined(GALE_USE_SSE3) && defined(G_COMP_GNUC)

G_INLINE long long roundToZero(double const d)
{
    long long i;
    __asm__(
        "fisttpl %[i]\n\t"
        : [i] "=m" (i)  /* Output  */
        : "t" (d)       /* Input   */
        :               /* Clobber */
    );
    return i;
}

#elif defined(GALE_USE_SSE3) && defined(G_COMP_MSVC)

G_INLINE long long roundToZero(double const d)
{
    // MSVC 8.0 does not support inline assembly on the x86-64 architecture.
    long long i;
    __asm {
        fld d
        fisttp i
    }
    return i;
}

#elif defined(GALE_USE_SSE2)

G_INLINE int roundToZero(double const d)
{
    // cvttsd2si (32-bit variant)
    return _mm_cvttsd_si32(_mm_load_sd(&d));
}

#else // G_ARCH_X86_64

G_INLINE long long roundToZero(double const d)
{
    return static_cast<long long>(d);
}

#endif // G_ARCH_X86_64

//@}

/**
 * \name Floating-point conversion related functions
 */
//@{

/**
 * A union to access the components of a 32-bit float.
 */
union FP32
{
    unsigned int u; ///< The float bits mapped to an integer.
    float f; ///< The float itself.
    struct
    {
        unsigned int mantissa:23;
        unsigned int exponent:8;
        unsigned int sign:1;
    };
};

/**
 * A union to access the components of a 16-bit "half" float.
 */
union FP16
{
    unsigned short u; ///< The float bits mapped to an integer.
    struct
    {
        unsigned int mantissa:10;
        unsigned int exponent:5;
        unsigned int sign:1;
    };
};

/// Converts the given 16-bit "half" float \a h to a 32-bit float, see
/// https://gist.github.com/2144712#L218.
G_INLINE FP32 convertHalfToFloat(FP16 h)
{
    static FP32 const magic={(254-15)<<23};
    static FP32 const was_infnan={(127+16)<<23};
    FP32 o;

    // Exponent/mantissa bits.
    o.u=(h.u&0x7fff)<<13;

    // Exponent adjustment.
    o.f*=magic.f;

    // Make sure Inf/NaN survive.
    if (o.f>=was_infnan.f)
        o.u|=255<<23;

    // Sign bit.
    o.u|=(h.u&0x8000)<<16;

    return o;
}

/// Converts the given 32-bit float \a f to a 16-bit "half" float, see
/// https://gist.github.com/2156668#L153.
G_INLINE FP16 convertFloatToHalf(FP32 f)
{
    FP32 infty={31<<23};
    FP32 magic={15<<23};
    FP16 o={0};

    unsigned int sign=f.sign;
    f.sign=0;

    // If all exponent bits are set this is Inf or NaN.
    if (f.exponent == 255)
    {
        // NaN->qNaN and Inf->Inf.
        o.exponent=31;
        o.mantissa=f.mantissa?0x200:0;
    }
    // This is a (de)normalized number or zero.
    else {
        // Make sure we don't get sticky bits.
        f.u&=~0xfff;

        // Shift exponent down, denormalize if necessary.
        f.f*=magic.f;

        // Rounding bias.
        f.u+=0x1000;
        if (f.u>infty.u) {
            // Clamp to signed infinity if overflowed.
            f.u=infty.u;
        }

        // Take the bits!
        o.u=f.u>>13;
    }

    o.sign=sign;

    return o;
}

//@}

/**
 * \name Bit juggling functions
 */
//@{

/// Returns the number of leading (most significant) zero bits in \a x.
inline unsigned int countLeadingZeroBits(unsigned int x)
{
#ifdef G_COMP_GNUC

    if (x==0) {
        return sizeof(x)*8;
    }

    // At this point, we know there is at least one bit set.
    return __builtin_clz(x);

#elif defined(G_COMP_MSVC)

    DWORD index;
    if (!_BitScanReverse(&index,x)) {
        return sizeof(x)*8;
    }

    unsigned int const MSB=sizeof(x)*8-1;
    return index^MSB;

#else // G_COMP_GNUC

    if (x==0) {
        return sizeof(x)*8;
    }

    unsigned int const MSB=sizeof(x)*8-1;

    // At this point, we know there is at least one bit set.
    unsigned int count=0;
    unsigned int const mask=1UL<<MSB;

    while ((x&mask)==0) {
        ++count;
        x<<=1UL;
    }

    return count;

#endif // G_COMP_GNUC
}

/// Returns the number of set bits in \a x.
inline unsigned int countSetBits(unsigned int x)
{
#if defined(G_COMP_MSVC) && G_COMP_MSVC>=G_COMP_VERSION(9,0,0)

    // This requires SSE 4.2 or CPUInfo::hasPOPCNT() to return true.
    return __popcnt(x);

#else

    x = ((x & 0xaaaaaaaaUL) >>  1) + (x & 0x55555555UL);
    x = ((x & 0xccccccccUL) >>  2) + (x & 0x33333333UL);
    x = ((x & 0xf0f0f0f0UL) >>  4) + (x & 0x0f0f0f0fUL);
    x = ((x & 0xff00ff00UL) >>  8) + (x & 0x00ff00ffUL);
    x = ((x & 0xffff0000UL) >> 16) + (x & 0x0000ffffUL);
    return x;

#endif
}

/// Returns the remainder of \a x divided by 3.
inline unsigned int mod3(unsigned int x)
{
    // Taken from http://www.codercorner.com/Modulo3.htm.
    return (1<<x)&3;
}

//@}

/**
 * \name Power-Of-2 (POT) related functions
 */
//@{

/// Returns whether \a x is an exact power of two.
inline bool isPowerOf2(unsigned int const x)
{
    return x && !(x&(x-1));
}

/// Returns the position of the least significant bit set in \a x (the least
/// significant bit has index 0).
inline int indexLSBSet(unsigned int x)
{
#ifdef G_COMP_GNUC

    if (x==0) {
        return -1;
    }

    // At this point, we know there is at least one bit set.
    return __builtin_ctz(x);

#elif defined(G_COMP_MSVC)

    DWORD index;
    if (_BitScanForward(&index,x)) {
        return static_cast<int>(index);
    }
    return -1;

#else // G_COMP_GNUC

    if (x==0) {
        return -1;
    }

    // At this point, we know there is at least one bit set.
    int index=0;

    while ((x&1)==0) {
        ++index;
        x>>=1UL;
    }

    return index;

#endif // G_COMP_GNUC
}

/// Returns the position of the most significant bit set in \a x (the least
/// significant bit has index 0).
inline int indexMSBSet(unsigned int x)
{
#ifdef G_COMP_GNUC

    if (x==0) {
        return -1;
    }

    // At this point, we know there is at least one bit set.
    unsigned int const MSB=sizeof(x)*8-1;
    return static_cast<int>(MSB)-__builtin_clz(x);

#elif defined(G_COMP_MSVC)

    DWORD index;
    if (_BitScanReverse(&index,x)) {
        return static_cast<int>(index);
    }
    return -1;

#else // G_COMP_GNUC

    if (x==0) {
        return -1;
    }

    unsigned int const MSB=sizeof(x)*8-1;

    // At this point, we know there is at least one bit set.
    int index=static_cast<int>(MSB);
    unsigned int const mask=1UL<<MSB;

    while ((x&mask)==0) {
        --index;
        x<<=1UL;
    }

    return index;

#endif // G_COMP_GNUC
}

/// Returns the smallest power of 2 that is greater than or equal to \a x,
/// except for \a x=0 and \a x>2147483648 which returns 0.
inline unsigned int ceilPow2(unsigned int const x)
{
#ifdef G_COMP_GNUC

    unsigned int result;
    __asm__(
        "decl %%edx\n\t"
        "xorl %%eax,%%eax\n\t"
        "bsrl %%edx,%%ecx\n\t"
        "cmovzl %%eax,%%ecx\n\t"
        "setnz %%al\n\t"
        "incl %%eax\n\t"
        "shll %%cl,%%eax\n\t"
        : "=a" (result)  /* Output  */
        : "d" (x)        /* Input   */
        : "%ecx", "cc"   /* Clobber */
    );
    return result;

#elif defined(G_COMP_MSVC) && !defined(G_ARCH_X86_64)

    // MSVC 8.0 does not support inline assembly on the x86-64 architecture.
    __asm {
        mov edx,x
        dec edx
        xor eax,eax
        bsr ecx,edx
        cmovz ecx,eax
        setnz al
        inc eax
        shl eax,cl
    }

#else // G_COMP_GNUC

    if (x==0) {
        return 0;
    }
    int i=indexMSBSet(x-1)+1;
    return 1UL<<static_cast<unsigned int>(i);

#endif // G_COMP_GNUC
}

/// Returns the largest power of 2 that is smaller than or equal to \a x, except
/// for \a x=0 which returns 0.
inline unsigned int floorPow2(unsigned int const x)
{
#ifdef G_COMP_GNUC

    unsigned int result;
    __asm__(
        "xorl %%eax,%%eax\n\t"
        "bsrl %%edx,%%ecx\n\t"
        "setnz %%al\n\t"
        "shll %%cl,%%eax\n\t"
        : "=a" (result)  /* Output  */
        : "d" (x)        /* Input   */
        : "%ecx", "cc"   /* Clobber */
    );
    return result;

#elif defined(G_COMP_MSVC) && !defined(G_ARCH_X86_64)

    // MSVC 8.0 does not support inline assembly on the x86-64 architecture.
    __asm {
        mov edx,x
        xor eax,eax
        bsr ecx,edx
        setnz al
        shl eax,cl
    }

#else // G_COMP_GNUC

    int i=indexMSBSet(x);
    if (i<0) {
        return 0;
    }
    return 1UL<<static_cast<unsigned int>(i);

#endif // G_COMP_GNUC
}

//@}

} // namespace math

} // namespace gale
