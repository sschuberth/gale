/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
 *
 * Copyright (C) 2005-2009  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
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

#ifndef RUNTIME_H
#define RUNTIME_H

/**
 * \file
 * Minimal replacements for runtime functions
 */

#ifdef GALE_TINY

// In order to be able to not link against the CRT, the following settings need
// to be adjusted manually from within the MSVC IDE:
//
// C/C++
//     Optimization
//         Set "Optimization" to "Minimize Size" (/O1)
//         Set "Enable Intrinsic Functions" to "Yes" (/Oi)
//     Code Generation
//         Set "Enable C++ Exceptions" to "No" (/GX-)
//         Set "Basic Runtime Checks" to "Default"
//         Set "Buffer Security Check" to "No" (/GS-)
//         Set "Floating Point Model" to "Fast" (/fp:fast)
//     Language
//         Set "Enable Run-Time Typ Info" to "No" (/GR-)

// Ignore all default libraries during linking.
#pragma comment(linker,"/nodefaultlib")

// To further reduce the executable size, based on the "Release" mode
// configuration, adjust the following settings from within the MSVC IDE:

// Enable global optimization, favor code size, enable frame pointer omission.
#pragma optimize("gsy",on)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \name Replacements for C runtime functions
 */
//@{

/// Minimal "puts" function implementation.
int print(char const* str);
#define puts(str) print(#str "\n")

/// Minimal "calloc" function implementation.
void* calloc(size_t num,size_t size);

/// Minimal "malloc" function implementation.
void* malloc(size_t size);

/// Minimal "realloc" function implementation.
void* realloc(void* memblock,size_t size);

/// Minimal "free" function implementation.
void free(void* memblock);

/// Minimal "memcpy" function implementation.
void* memcpy(void* dest,void const* src,size_t count);

/// Minimal "memmove" function implementation.
void* memmove(void* dest,void const* src,size_t count);

/// Minimal "memset" function implementation.
void* memset(void* dest,int c,size_t count);

//@}

/**
 * \name Replacements for non-inline math functions, see
 * <http://www.cubic.org/docs/download/tnymath2.h>
 */
//@{

/// Replacement for a CRT function required by MSVC to calculate the arcus
/// cosinus of a number x. Arguments are passed in FPU registers.
double _CIacos(void);

/// Replacement for a CRT function required by MSVC to calculate x to the y-th
/// power. Arguments are passed in FPU registers.
double _CIpow(void);

//@}

#ifdef __cplusplus
};
#endif

/**
 * \name Replacements for C++ runtime functions
 */
//@{

/// Minimal "new" operator implementation.
void* operator new(size_t bytes);

/// Minimal array "new" operator implementation.
void* operator new[](size_t bytes);

/// Minimal placement "new" operator implementation.
void* operator new(size_t bytes,void* place);

/// Minimal "delete" operator implementation.
void operator delete(void* pointer);

/// Minimal array "delete" operator implementation.
void operator delete[](void* pointer);

/// Minimal placement "delete" operator implementation.
void operator delete(void* pointer,void* place);

//@}

#endif // GALE_TINY

#endif // RUNTIME_H
