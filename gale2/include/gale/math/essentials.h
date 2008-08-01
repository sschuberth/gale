/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
 *
 * Copyright (C) 2005-2008  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
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

#include "../global/defines.h"

/**
 * \file
 * Optimized essential mathematical functions
 */

#ifndef _USE_MATH_DEFINES
    #define _USE_MATH_DEFINES
#endif
#include <math.h>

#include <intrin.h>
#include <stdlib.h>

#include <limits>

namespace gale {

namespace math {

/**
 * \name Angle conversion related functions
 */
//@{

/// Converts the given \a angle specified in degrees to radians.
template<typename T>
inline float convDegToRad(T const angle)
{
    return static_cast<float>(M_PI*angle/180.0);
}

/// Converts the given double precision \a angle specified in degrees to radians.
inline double convDegToRad(double const angle)
{
    return static_cast<double>(M_PI*angle/180.0);
}

/// Converts the given \a angle specified in radians to degrees.
template<typename T>
inline float convRadToDeg(T const angle)
{
    return static_cast<float>(angle*180.0/M_PI);
}

/// Converts the given double precision \a angle specified in radians to degrees.
inline double convRadToDeg(double const angle)
{
    return static_cast<double>(angle*180.0/M_PI);
}

//@}

/**
 * \name Bit juggling functions
 */
//@{

/// Returns the number of leading (most significant) zero bits in \a x.
inline unsigned long countLeadingZeroBits(unsigned long x)
{
#ifdef G_COMP_GNUC

    if (x==0) {
        return sizeof(x)*8;
    }

    unsigned long result;
    __asm__(
        "bsrl %[x],%[result]\n\t"
        "xorl $31,%[result]\n\t"
        : [result] "=r" (result)  /* Output  */
        : [x] "r" (x)             /* Input   */
        : "cc"                    /* Clobber */
    );
    return result;

#elif defined(G_COMP_MSVC)

#ifdef G_ARCH_X86_64

    unsigned long index;
    if (!_BitScanReverse(&index,x)) {
        return sizeof(x)*8;
    }
    return index^31;

#else // G_ARCH_X86_64

    if (x==0) {
        return sizeof(x)*8;
    }

    // MSVC 8.0 does not support inline assembly on the x86-64 architecture.
    __asm {
        bsr eax,x
        xor eax,31
    }

#endif // G_ARCH_X86_64

#else // G_COMP_GNUC

    unsigned long const MSB=sizeof(x)*8-1;
    unsigned long const mask=1UL<<MSB;

    // At this point, we know there is at least one bit set.
    unsigned long count=0;
    while ((x&mask)==0) {
        ++count;
        x<<=1;
    }
    return count;

#endif // G_COMP_GNUC
}

/// Returns the number of set bits in \a x.
inline unsigned long countSetBits(unsigned long x) {
    x =    ((x & 0xaaaaaaaa) >>  1) + (x & 0x55555555);
    x =    ((x & 0xcccccccc) >>  2) + (x & 0x33333333);
    x =    ((x & 0xf0f0f0f0) >>  4) + (x & 0x0f0f0f0f);
    x =    ((x & 0xff00ff00) >>  8) + (x & 0x00ff00ff);
    return ((x & 0xffff0000) >> 16) + (x & 0x0000ffff);
}

//@}

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

/// Wraps \a x to range [\a a,\a b[, where \a a <= \a b.
template<typename T>
inline T wrap(T x,T const a,T const b)
{
    T const range=b-a;
    while (x<a) {
        x+=range;
    }
    while (x>=b) {
        x-=range;
    }
    return x;
}

/// Wraps \a x to range [\a a,\a b[ or range [\a b,\a a[.
template<typename T>
inline T wrapSafe(T x,T const a,T const b)
{
    return (a<=b)?wrap(x,a,b):wrap(x,b,a);
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

//@}

/**
 * \name Floating-point rounding related functions
 */
//@{

/// Rounds the 32-bit floating point value \a f to the nearest integer (to the
/// even one in case of ambiguity). This is the default rounding mode on x86
/// platforms, so the floating-point control word is not modified.
inline int roundToEven(float const f)
{
#ifdef GALE_SSE

    return _mm_cvtss_si32(_mm_load_ss(&f));

#elif defined(G_COMP_GNUC)

    int i;
    __asm__(
        "fistpl %[i]\n\t"
        : [i] "=m" (i)  /* Output  */
        : "t" (f)       /* Input   */
        :               /* Clobber */
    );
    return i;

#elif defined(G_COMP_MSVC) && !defined(G_ARCH_X86_64)

    // MSVC 8.0 does not support inline assembly on the x86-64 architecture.
    int i;
    __asm {
        fld f
        fistp i
    }
    return i;

#else // GALE_SSE

    return static_cast<int>(f+0.5f);

#endif // GALE_SSE
}

/// Rounds the 64-bit floating point value \a d to the nearest integer (to the
/// even one in case of ambiguity). This is the default rounding mode on x86
/// platforms, so the floating-point control word is not modified.
inline int roundToEven(double const d)
{
#ifdef GALE_SSE2

    return _mm_cvtsd_si32(_mm_load_sd(&d));

#elif defined(G_COMP_GNUC)

    int i;
    __asm__(
        "fistpl %[i]\n\t"
        : [i] "=m" (i)  /* Output  */
        : "t" (d)       /* Input   */
        :               /* Clobber */
    );
    return i;

#elif defined(G_COMP_MSVC) && !defined(G_ARCH_X86_64)

    // MSVC 8.0 does not support inline assembly on the x86-64 architecture.
    int i;
    __asm {
        fld d
        fistp i
    }
    return i;

#else // GALE_SSE2

    return static_cast<int>(d+0.5);

#endif // GALE_SSE2
}

/// Rounds the 32-bit floating point value \a f to the nearest integer towards
/// zero (truncating the number). This is the default ANSI C rounding mode; use
/// this method instead of a cast to an integer as it is faster.
inline int roundToZero(float const f)
{
#if defined(GALE_SSE3) && G_COMP_GNUC

    int i;
    __asm__(
        "fisttpl %[i]\n\t"
        : [i] "=m" (i)  /* Output  */
        : "t" (f)       /* Input   */
        :               /* Clobber */
    );
    return i;

#elif defined(GALE_SSE3) && defined(G_COMP_MSVC) && !defined(G_ARCH_X86_64)

    // MSVC 8.0 does not support inline assembly on the x86-64 architecture.
    int i;
    __asm {
        fld f
        fisttp i
    }
    return i;

#elif defined(GALE_SSE2)

    return _mm_cvttss_si32(_mm_load_ss(&f));

#else

    return static_cast<int>(f);

#endif
}

/// Rounds the 64-bit floating point value \a d to the nearest integer towards
/// zero (truncating the number). This is the default ANSI C rounding mode; use
/// this method instead of a cast to an integer as it is faster.
inline int roundToZero(double const d)
{
#if defined(GALE_SSE3) && defined(G_COMP_GNUC)

    int i;
    __asm__(
        "fisttpl %[i]\n\t"
        : [i] "=m" (i)  /* Output  */
        : "t" (d)       /* Input   */
        :               /* Clobber */
    );
    return i;

#elif defined(GALE_SSE3) && defined(G_COMP_MSVC) && !defined(G_ARCH_X86_64)

    // MSVC 8.0 does not support inline assembly on the x86-64 architecture.
    int i;
    __asm {
        fld d
        fisttp i
    }
    return i;

#elif defined(GALE_SSE2)

    return _mm_cvttsd_si32(_mm_load_sd(&d));

#else

    return static_cast<int>(d);

#endif
}

//@}

/**
 * \name Overloaded functions to calculate the absolute value
 */
//@{

/// This is needed as a dummy for template methods.
inline unsigned int abs(unsigned int const x)
{
    return x;
}

/// This is needed as a dummy for template methods.
inline unsigned long abs(unsigned long const x)
{
    return x;
}

/// This is needed as a dummy for template methods.
inline unsigned long long abs(unsigned long long const x)
{
    return x;
}

/// Calls the appropriate run-time function.
inline int abs(int const x)
{
    return ::abs(x);
}

/// Calls the appropriate run-time function.
inline long abs(long const x)
{
    return ::labs(x);
}

/// Calls the appropriate run-time function.
inline long long abs(long long const x)
{
#ifdef G_COMP_GNUC

    return ::llabs(x);

#elif defined(G_COMP_MSVC)

    return ::_abs64(x);

#else // G_COMP_GNUC

    return ::abs(x);

#endif // G_COMP_GNUC
}

/// Calls the appropriate run-time function.
inline float abs(float const x)
{
    return ::fabsf(x);
}

/// Calls the appropriate run-time function.
inline double abs(double const x)
{
    return ::fabs(x);
}

/// Calls the appropriate run-time function.
inline long double abs(long double const x)
{
    return ::fabsl(x);
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
inline int getLSBSet(unsigned int x)
{
    if (x==0) {
        return -1;
    }

#ifdef G_COMP_GNUC

    int result;
    __asm__(
        "bsfl %[x],%[result]\n\t"
        : [result] "=r" (result)  /* Output  */
        : [x] "r" (x)             /* Input   */
        : "cc"                    /* Clobber */
    );
    return result;

#elif defined(G_COMP_MSVC) && !defined(G_ARCH_X86_64)

    // MSVC 8.0 does not support inline assembly on the x86-64 architecture.
    __asm {
        bsf eax,x
    }

#else // G_COMP_GNUC

    // At this point, we know there is at least one bit set.
    int index=0;
    while ((x&1)==0) {
        ++index;
        x>>=1;
    }
    return index;

#endif // G_COMP_GNUC
}

/// Returns the position of the most significant bit set in \a x (the least
/// significant bit has index 0).
inline int getMSBSet(unsigned int x)
{
    if (x==0) {
        return -1;
    }

#ifdef G_COMP_GNUC

    int result;
    __asm__(
        "bsrl %[x],%[result]\n\t"
        : [result] "=r" (result)  /* Output  */
        : [x] "r" (x)             /* Input   */
        : "cc"                    /* Clobber */
    );
    return result;

#elif defined(G_COMP_MSVC) && !defined(G_ARCH_X86_64)

    // MSVC 8.0 does not support inline assembly on the x86-64 architecture.
    __asm {
        bsr eax,x
    }

#else // G_COMP_GNUC

    unsigned int const MSB=sizeof(x)*8-1;
    unsigned int const mask=1UL<<MSB;

    // At this point, we know there is at least one bit set.
    int index=MSB;
    while ((x&mask)==0) {
        --index;
        x<<=1;
    }
    return index;

#endif // G_COMP_GNUC
}

/// Returns the smallest power of 2 that is greater than or equal to \a x,
/// except for \a x=0 and \a x>2147483648 which returns 0.
inline unsigned int getCeilPow2(unsigned int const x)
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
    int i=getMSBSet(x-1)+1;
    return 1UL<<static_cast<unsigned int>(i);

#endif // G_COMP_GNUC
}

/// Returns the largest power of 2 that is smaller than or equal to \a x, except
/// for \a x=0 which returns 0.
inline unsigned int getFloorPow2(unsigned int const x)
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

    int i=getMSBSet(x);
    if (i<0) {
        return 0;
    }
    return 1UL<<static_cast<unsigned int>(i);

#endif // G_COMP_GNUC
}

//@}

} // namespace math

} // namespace gale

#endif // ESSENTIALS_H
