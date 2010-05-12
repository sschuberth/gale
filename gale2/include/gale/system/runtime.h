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

#ifdef GALE_TINY_CODE

// In order to be able to not link against the CRT, the following settings need
// to be adjusted manually from within the MSVC IDE:
//
// C/C++
//     Optimization
//         Set "Optimization" to "Minimize Size" (/O1)
//         Set "Enable Intrinsic Functions" to "Yes" (/Oi)
//     Code Generation
//         Set "Enable C++ Exceptions" to "No" (/EHs-c-) (/GX- deprecated since VS2005)
//         Set "Basic Runtime Checks" to "Default"
//         Set "Runtime Library" to "Multi-threaded" (/MT) (to avoid linker warnings)
//         Set "Buffer Security Check" to "No" (/GS-)
//         Set "Floating Point Model" to "Fast" (/fp:fast)
//     Language
//         Set "Enable Run-Time Type Info" to "No" (/GR-)
//     Command Line
//         Specify "/Gs999999" ("control stack checking calls")
//         Specify "/QIfist" ("suppress _ftol") (deprecated)
// Linker
//     Input
//         Set "Ignore All Default Libraries" to "Yes"

// To further reduce the executable size, based on the "Release" mode
// configuration, adjust the following settings from within the MSVC IDE:
//
// C/C++
//     Optimization
//         Set "Favor Size or Speed" to "Favor Small Code" (/Os)
//         Set "Omit Frame Pointers" to "Yes" (/Oy)
//     Code Generation
//         Set "Enable String Pooling" to "Yes" (/GF)
//         Set "Enable Function-Level Linking" to "Yes" (/Gy)
//     Advanced
//         Set "Calling Convention" to "__fastcall" (/Gr)
//     Command Line
//         Specify "/Zl" ("omit default library name in .OBJ")
// Linker
//     Manifest File
//         Set "Generate Manifest" to "No" (/MANIFEST:NO)
//     Optimization
//         Set "References" to "Eliminate Unreferenced Data" (/OPT:REF)
//         Set "Optimize for Windows98" to "No" (/OPT:NOWIN98) (deprecated since VS2008)
//     Advanced
//         Set "Randomized Base Address" to "Disable Image Randomization" (/DYNAMICBASE:NO) (since VS2008)
//         Set "Fixed Base Address" to "Image must be loaded at a fixed address" (/FIXED)
//     Command Line
//         Specify "/MERGE:.rdata=.text" ("Merge read-only data and code sections")

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \name Replacements for C runtime functions
 */
//@{

/// Replacement for various functions that print a string to stdout.
int print_str(char const* str);

/// Replacement for various functions that print an integer to stdout.
int print_int(int num);

/// Minimal "puts" function implementation.
#define puts(str)        \
    {                    \
        print_str(str);  \
        print_str("\n"); \
    }

//@}

/**
 * \name Replacements for non-inline math functions
 * Arguments are passed on the FPU stack; they need to be popped, and the result
 * needs to be pushed to st(0).
 */
//@{

/// Replacement for a CRT function required by MSVC to calculate the arcus
/// cosinus of a number x.
double __cdecl _CIacos(/* double x */);

/// Replacement for a CRT function required by MSVC to calculate the floating-
/// point remainder of x / y.
double __cdecl _CIfmod(/* double x,double y */);

/// Replacement for a CRT function required by MSVC to calculate x to the y-th
/// power.
double __cdecl _CIpow(/* double x,double y */);

//@}

#ifdef __cplusplus
};
#endif

/**
 * \name Replacements for C++ runtime functions
 */
//@{

/// Minimal "new" operator implementation.
void* __cdecl operator new(size_t bytes);

/// Minimal array "new" operator implementation.
void* __cdecl operator new[](size_t bytes);

/// Minimal placement "new" operator implementation.
void* __cdecl operator new(size_t bytes,void* place);

/// Minimal "delete" operator implementation.
void __cdecl operator delete(void* pointer);

/// Minimal array "delete" operator implementation.
void __cdecl operator delete[](void* pointer);

/// Minimal placement "delete" operator implementation.
void __cdecl operator delete(void* pointer,void* place);

//@}

#endif // GALE_TINY_CODE

#endif // RUNTIME_H
