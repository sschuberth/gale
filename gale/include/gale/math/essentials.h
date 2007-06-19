/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at http://developer.berlios.de/projects/gale/
 *
 * Copyright (C) 2005-2007  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
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

#ifndef ESSENTIALS_H
#define ESSENTIALS_H

/**
 * \file
 * Optimized essential mathematical functions
 */

#ifndef _USE_MATH_DEFINES
    #define _USE_MATH_DEFINES
#endif
#include <math.h>

#ifdef GALE_SSE
    #include <xmmintrin.h>
#endif

#include <limits>

namespace gale {

namespace math {

/**
 * \name Angle conversion related functions
 */
//@{

/// Converts the given \a angle specified in degrees to radians.
inline double convDegToRad(double const angle)
{
    return (static_cast<double>(M_PI)*angle)/180.0;
}

/// Converts the given \a angle specified in degrees to radians.
inline float convDegToRad(float const angle)
{
    return (static_cast<float>(M_PI)*angle)/180.0f;
}

/// Converts the given \a angle specified in degrees to radians.
inline float convDegToRad(int const angle)
{
    return (static_cast<float>(M_PI)*angle)/180.0f;
}

/// Converts the given \a angle specified in degrees to radians.
inline float convDegToRad(short const angle)
{
    return (static_cast<float>(M_PI)*angle)/180.0f;
}

/// Converts the given \a angle specified in radians to degrees.
inline double convRadToDeg(double const angle)
{
    return (angle*180.0)/static_cast<double>(M_PI);
}

/// Converts the given \a angle specified in radians to degrees.
inline float convRadToDeg(float const angle)
{
    return (angle*180.0f)/static_cast<float>(M_PI);
}

/// Converts the given \a angle specified in radians to degrees.
inline float convRadToDeg(int const angle)
{
    return (angle*180.0f)/static_cast<float>(M_PI);
}

/// Converts the given \a angle specified in radians to degrees.
inline float convRadToDeg(short const angle)
{
    return (angle*180.0f)/static_cast<float>(M_PI);
}

//@}

/**
 * \name Extremes determination related functions
 */
//@{

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

/// Clamps \a x to the range specified by \a a and \a b.
template<typename T>
inline T clamp(T const x,T const a,T const b)
{
    return (a<b)?min(max(a,x),b):min(max(b,x),a);
}

//@}

/**
 * \name Floating-point rounding related functions
 */
//@{

/// Rounds \a f to the nearest integer (to the even one in case of ambiguity).
/// This is the default rounding mode on x86 platforms, so the floating-point
/// control word is not modified.
inline long roundToEven(float const f)
{
    // By default, the Pentium's fistp instruction does a "round to even", so
    // there is no need to save and restore the floating-point control word like
    // the C runtime does (see http://www.self-similar.com/rounding.html).

#ifdef GALE_SSE

    return _mm_cvtss_si32(_mm_set_ss(f));

#elif defined(__GNUC__)

    long i;
    __asm__(
        "fistpl %0\n\t"
        : "=m" (i)
        : "t" (f)
        : "st"
    );
    return i;

#elif defined(_MSC_VER) && !defined(_WIN64)

    // MSVC 8.0 does not support inline assembly on the x64 platform.
    long i;
    __asm {
        fld f
        fistp i
    }
    return i;

#else

    return static_cast<long>(f+0.5f);

#endif // __GNUC__
}

/// Rounds \a f to the nearest integer towards zero (truncating the number).
/// This is the default ANSI C rounding mode; use this method instead of a cast
/// to an integer as it is faster.
inline long roundToZero(float const f)
{
#if defined(GALE_SSE3) && defined(__GNUC__)

    long i;
    __asm__(
        "fisttpl %0\n\t"
        : "=m" (i)
        : "t" (f)
        : "st"
    );
    return i;

#elif defined(GALE_SSE3) && defined(_MSC_VER) && !defined(_WIN64)

    // MSVC 8.0 does not support inline assembly on the x64 platform.
    long i;
    __asm {
        fld f
        fisttp i
    }
    return i;

#elif defined(GALE_SSE)

    return _mm_cvttss_si32(_mm_set_ss(f));

#else

    return static_cast<long>(f);

#endif
}

//@}

/**
 * \name Power-Of-2 (POT) related functions
 */
//@{

/// Returns whether \a x is an exact power of two.
inline bool isPowerOf2(unsigned int const x)
{
    if (x==0) {
        return false;
    }
    return (x&(x-1))==0;
}

/// Returns the position of the least significant bit set in \a x (the least
/// significant bit has index 0).
inline long getLSBSet(unsigned int const x)
{
    if (x==0) {
        return -1;
    }

#ifdef __GNUC__

    long result;
    __asm__(
        "bsf %%ecx,%%eax\n\t"
        : "=a" (result)
        : "c" (x)
        : "cc"
    );
    return result;

#elif defined(_MSC_VER) && !defined(_WIN64)

    // MSVC 8.0 does not support inline assembly on the x64 platform.
    __asm {
        mov ebx,x
        bsf eax,ebx
    }

#else

    long index=0;
    while ((x&1)==0) {
        ++index;
        x>>=1;
    }
    return index;

#endif // __GNUC__
}

/// Returns the position of the most significant bit set in \a x (the least
/// significant bit has index 0).
inline long getMSBSet(unsigned int const x)
{
    if (x==0) {
        return -1;
    }

#ifdef __GNUC__

    long result;
    __asm__(
        "bsr %%ecx,%%eax\n\t"
        : "=a" (result)
        : "c" (x)
        : "cc"
    );
    return result;

#elif defined(_MSC_VER) && !defined(_WIN64)

    // MSVC 8.0 does not support inline assembly on the x64 platform.
    __asm {
        mov ebx,x
        bsr eax,ebx
    }

#else

    unsigned int const MSB=(1UL<<(sizeof(x)+1))-1;
    unsigned int const mask=1UL<<MSB;

    // At this point, we know there is at least one bit set.
    long index=MSB;
    while ((x&mask)==0) {
        --index;
        x<<=1;
    }
    return index;

#endif // __GNUC__
}

/// Returns the number of leading (most significant) zero bits in \a x.
inline long countLeadingZeroBits(unsigned int const x)
{
    if (x==0) {
        return 1<<(sizeof(x)+1);
    }

#ifdef __GNUC__

    long result;
    __asm__(
        "bsr %%ecx,%%eax\n\t"
        "xorl $31,%%eax\n\t"
        : "=a" (result)
        : "c" (x)
        : "cc"
    );
    return result;

#elif defined(_MSC_VER) && !defined(_WIN64)

    // MSVC 8.0 does not support inline assembly on the x64 platform.
    __asm {
        mov ebx,x
        bsr eax,ebx
        xor eax,31
    }

#else

    unsigned int const MSB=(1UL<<(sizeof(x)+1))-1;
    unsigned int const mask=1UL<<MSB;

    // At this point, we know there is at least one bit set.
    long count=0;
    while ((x&mask)==0) {
        ++count;
        x<<=1;
    }
    return count;

#endif
}

/// Returns the largest power of 2 that is smaller than or equal to \a x, except
/// for \a x=0 which returns 0.
inline unsigned int getFloorPow2(unsigned int const x)
{
#ifdef __GNUC__

    unsigned int result;
    __asm__(
        "xor eax,eax\n\t"
        "bsr ecx,ecx\n\t"
        "setnz al\n\t"
        "shl eax,cl\n\t"
        : "=a" (result)
        : "c" (x)
    );
    return result;

#elif defined(_MSC_VER) && !defined(_WIN64)

    // MSVC 8.0 does not support inline assembly on the x64 platform.
    __asm {
        mov ecx,x
        xor eax,eax
        bsr ecx,ecx
        setnz al
        shl eax,cl
    }

#else

    long i=getMSBSet(x);
    if (i<0) {
        return 0;
    }
    return 1UL<<static_cast<unsigned int>(i);

#endif // __GNUC__
}

/// Returns the smallest power of 2 that is greater than or equal to \a x,
/// except for \a x=0 and \a x>2147483648 which returns 0.
inline unsigned int getCeilPow2(unsigned int const x)
{
#ifdef __GNUC__

    unsigned int result;
    __asm__(
        "xor eax,eax\n\t"
        "dec ecx\n\t"
        "bsr ecx,ecx\n\t"
        "cmovz ecx,eax\n\t"
        "setnz al\n\t"
        "inc eax\n\t"
        "shl eax,cl\n\t"
        : "=a" (result)
        : "c" (x)
    );
    return result;

#elif defined(_MSC_VER) && !defined(_WIN64)

    // MSVC 8.0 does not support inline assembly on the x64 platform.
    __asm {
        mov ecx,x
        xor eax,eax
        dec ecx
        bsr ecx,ecx
        cmovz ecx,eax
        setnz al
        inc eax
        shl eax,cl
    }

#else

    if (x==0) {
        return 0;
    }
    long i=getMSBSet(x-1)+1;
    return 1UL<<static_cast<unsigned int>(i);

#endif // __GNUC__
}

//@}

/**
 * \name Template specializations for the abs() functions
 */
//@{

template<typename T>
inline T abs(T const x)
{
    return ::abs(x);
}

template<>
inline long double abs(long double const x)
{
    return ::fabsl(x);
}

template<>
inline double abs(double const x)
{
    return ::fabs(x);
}

template<>
inline float abs(float const x)
{
    return ::fabsf(x);
}

template<>
inline long long abs(long long const x)
{
#ifdef __GNUC__

    return ::llabs(x);

#elif defined(_MSC_VER)

    return ::_abs64(x);

#else

    return ::abs(x);

#endif
}

template<>
inline long abs(long const x)
{
    return ::labs(x);
}

//@}

} // namespace math

} // namespace gale

#endif // ESSENTIALS_H
