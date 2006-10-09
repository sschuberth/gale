/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at http://developer.berlios.de/projects/gale/
 *
 * Copyright (C) 2005-2006  Sebastian Schuberth <sschuberth@gmail.com>
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
 * Optimized essential mathematical functions.
 */

namespace gale {

namespace math {

/**
 * \name Angle conversion related functions
 */
//@{

/// Converts the given \a angle specified in degrees to radians.
template<typename T>
inline T convertDegToRad(T angle) {
    static T const PI=static_cast<T>(3.14159265358979323846);
    return (angle*PI)/180;
}

/// Converts the given \a angle specified in radians to degrees.
template<typename T>
inline T convertRadToDeg(T angle) {
    static T const PI=static_cast<T>(3.14159265358979323846);
    return (angle*180)/PI;
}

//@}

/**
 * \name Power-Of-2 (POT) related functions
 */
//@{

/// Returns whether \a x is an exact power of two.
inline bool isPowerOf2(unsigned int x) {
    if (x==0)
        return false;
    return (x&(x-1))==0;
}

/// Returns the position of the least significant bit set in \a x.
inline long getLSBSet(unsigned int x) {
    if (x==0)
        return -1;

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
#endif
}

/// Returns the position of the most significant bit set in \a x.
inline long getMSBSet(unsigned int x) {
    if (x==0)
        return -1;

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
    long index=31;
    while ((x&(1UL<<31))==0) {
        --index;
        x<<=1;
    }
    return index;
#endif
}

/// Returns the largest power of 2 that is smaller than or equal to \a x, except
/// for \a x=0 which returns 0.
inline unsigned int getFloorPow2(unsigned int x) {
#ifdef __GNUC__
    unsigned int result;
    __asm__(
        "xor eax,eax\n"
        "bsr ecx,ecx\n"
        "setnz al\n"
        "shl eax,cl\n"
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
    if (i<0)
        return 0;
    return 1UL<<static_cast<unsigned int>(i);
#endif
}

/// Returns the smallest power of 2 that is greater than or equal to \a x,
/// except for \a x=0 and \a x>2147483648 which returns 0.
inline unsigned int getCeilPow2(unsigned int x) {
#ifdef __GNUC__
    unsigned int result;
    __asm__(
        "xor eax,eax\n"
        "dec ecx\n"
        "bsr ecx,ecx\n"
        "cmovz ecx,eax\n"
        "setnz al\n"
        "inc eax\n"
        "shl eax,cl\n"
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
    if (x==0)
        return 0;
    long i=getMSBSet(x-1)+1;
    return 1UL<<static_cast<unsigned int>(i);
#endif
}

//@}

/**
 * \name Floating-point rounding related functions
 */
//@{

/// Rounds \a f to the nearest integer (to the even one in case of ambiguity).
/// This is the default rounding mode on x86 platforms, so the floating-point
/// control word is not modified.
inline long long roundToEven(float f) {
    // By default, the Pentium's fistp instruction does a "round to even", so
    // there is no need to save and restore the floating-point control word like
    // the C runtime does (see
    // http://web.archive.org/web/20041011125702/http://www.self-similar.com/rounding.html).
#ifdef __GNUC__
    long long i;
    __asm__(
        "fistpl %0\n\t"
        : "=m" (i)
        : "t" (f)
        : "st"
    );
    return i;
#elif defined(_MSC_VER) && !defined(_WIN64)
    // MSVC 8.0 does not support inline assembly on the x64 platform.
    long long i;
    __asm {
        fld f
        fistp i
    }
    return i;
#else
    union {
        double f;
        long long i;
    } u;
    // Add a float bias that shifts the mantissa to the right, and then subtract
    // the bias as an integer again (see
    // http://groups.google.de/group/comp.graphics.algorithms/tree/browse_frm/thread/5099095b1cd1a78a/2b72601d013c7a0f).
    u.f=static_cast<double>(3LL<<51);
    u.f+=static_cast<double>(f);
    return u.i-0x4338000000000000;
#endif
}

/// Rounds \a f to the nearest integer towards zero (truncating the number).
/// This is the default ANSI C rounding mode; use this method instead of a cast
/// to an integer as it is faster.
inline long long roundToZero(float f) {
    union {
        float f;
        long i;
    } u;
    // For positive numbers, this returns roundToEven(f-u.i). For negative
    // numbers, an additional two instructions are required negate this "magic"
    // number (see http://web.archive.org/web/20041011125702/http://www.self-similar.com/rounding.html).
    u.f=f;
    u.i=(u.i&0x80000000)|0x3efffffe;
    return roundToEven(f-u.f);
}

//@}

} // namespace math

} // namespace gale

#endif // ESSENTIALS_H
