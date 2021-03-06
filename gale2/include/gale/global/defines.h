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
 * Global macro and type definitions
 */

/*
 * Architecture defines
 */

#if defined(__amd64) || defined(__amd64__) || defined(__x86_64) || defined(__x86_64__) || defined(_M_X64)
    #ifdef G_ARCH_DEFINED
        #error The architecture has already been defined.
    #else
        #define G_ARCH_DEFINED
    #endif

    /**
     * \def G_ARCH_X86_64
     * AMD64 / Intel 64 architecture define, see
     * http://predef.sourceforge.net/prearch.html#sec3.
     */
    #define G_ARCH_X86_64
#endif

#if defined(__i386) || defined(__i386__) || defined(__I86__) || defined(__X86__) || defined(_M_IX86)
    #ifdef G_ARCH_DEFINED
        #error The architecture has already been defined.
    #else
        #define G_ARCH_DEFINED
    #endif

    /**
     * \def G_ARCH_X86
     * Intel x86 architecture define, see
     * http://predef.sourceforge.net/prearch.html#sec6.
     */
    #define G_ARCH_X86
#endif

#ifndef G_ARCH_DEFINED
    #error No architecture has been defined.
#else
    #undef G_ARCH_DEFINED
#endif

/*
 * Operating system defines
 */

#if defined(__linux) || defined(__linux__)
    #ifdef G_OS_DEFINED
        #error The operating system has already been defined.
    #else
        #define G_OS_DEFINED
    #endif

    /**
     * \def G_OS_LINUX
     * Linux operating system define, see
     * http://predef.sourceforge.net/preos.html#sec16.
     */
    #define G_OS_LINUX
#endif

// These defines are also set under Windows 64-bit. They do *not* indicate the
// bitness of the OS the compiler is running on, but the bitness of the target OS.
#if defined(_WIN32) || defined(__WIN32__)
    #ifdef G_OS_DEFINED
        #error The operating system has already been defined.
    #else
        #define G_OS_DEFINED
    #endif

    /**
     * \def G_OS_WINDOWS
     * Windows operating system define, see
     * http://predef.sourceforge.net/preos.html#sec23.
     */
    #define G_OS_WINDOWS
#endif

#ifndef G_OS_DEFINED
    #error No operating system has been defined.
#else
    #undef G_OS_DEFINED
#endif

/*
 * Compiler defines
 */

/**
 * \def G_COMP_VERSION
 * Can be used to easily compare any of the C_COMP_<compiler> defines against a
 * specific version.
 */

#ifdef G_COMP_VERSION
    #undef G_COMP_VERSION
#endif

#define G_COMP_VERSION(major,minor,patch) ((major)*10000000 + (minor)*100000 + (patch))

#ifdef __GNUC__
    #ifdef G_COMP_DEFINED
        #error The compiler has already been defined.
    #else
        #define G_COMP_DEFINED
    #endif

    /**
     * \def G_COMP_GNUC
     * GNU C/C++ compiler define, see
     * http://predef.sourceforge.net/precomp.html#sec13.
     */
    #define G_COMP_GNUC G_COMP_VERSION(__GNUC__,__GNUC_MINOR__,__GNUC_PATCHLEVEL__)
#endif

#ifdef _MSC_VER
    #ifdef G_COMP_DEFINED
        #error The compiler has already been defined.
    #else
        #define G_COMP_DEFINED
    #endif

    /**
     * \def G_COMP_MSVC
     * Microsoft Visual C++ compiler define, see
     * http://predef.sourceforge.net/precomp.html#sec32.
     */
    #define G_COMP_MSVC G_COMP_VERSION(_MSC_VER/100-6,_MSC_VER%100,_MSC_FULL_VER-_MSC_VER*100000)
#endif

#ifndef G_COMP_DEFINED
    #error No compiler has been defined.
#else
    #undef G_COMP_DEFINED
#endif

/*
 * Miscellaneous defines
 */

/**
 * \def G_ARRAY_LENGTH
 * Returns the number of elements in an array.
 */

#ifdef G_ARRAY_LENGTH
    #undef G_ARRAY_LENGTH
#endif

#define G_ARRAY_LENGTH(x) (sizeof(x)/sizeof(*x))

/**
 * \def G_ASSERT
 * Run-time assertion to the CRT, if available.
 */

#include <assert.h>

#ifdef G_ASSERT
    #undef G_ASSERT
#endif

#ifdef GALE_TINY_CODE
    #define G_ASSERT(x) do { if (!(x)) ExitProcess(0); } while (0);
#else
    #define G_ASSERT(x) assert(x);
#endif

/**
 * \def G_ASSERT_CALL
 * Run-time assertion for function calls.
 */

#ifdef G_ASSERT_CALL
    #undef G_ASSERT_CALL
#endif

#ifdef NDEBUG
    #define G_ASSERT_CALL(x) x;
#else
    #define G_ASSERT_CALL(x) assert(x);
#endif

/**
 * \def G_ASSERT_OPENGL
 * Run-time assertion for the OpenGL error state.
 */

#ifdef G_ASSERT_OPENGL
    #undef G_ASSERT_OPENGL
#endif

#define G_ASSERT_OPENGL G_ASSERT(glGetError()==GL_NO_ERROR)

/**
 * \def G_INLINE
 * Compiler-specific keyword definition to force a function to be inline.
 */

#ifdef G_INLINE
    #undef G_INLINE
#endif

#ifdef G_COMP_GNUC
    #define G_INLINE inline __attribute__ ((always_inline))
#elif defined(G_COMP_MSVC)
    #define G_INLINE __forceinline
#else
    #define G_INLINE inline
#endif

/**
 * \def G_NO_VTABLE
 * Compiler-specific keyword definition to force a class to have no virtual
 * function table.
 */

#ifdef G_NO_VTABLE
    #undef G_NO_VTABLE
#endif

#ifdef G_COMP_GNUC
    #define G_NO_VTABLE
#elif defined(G_COMP_MSVC)
    #define G_NO_VTABLE __declspec(novtable)
#else
    #define G_NO_VTABLE
#endif

/**
 * \def G_UNREF_PARAM
 * Macro to mark unreferenced parameters in order to avoid compiler warnings.
 * Works with "const" parameters.
 */

#ifdef G_UNREF_PARAM
    #undef G_UNREF_PARAM
#endif

#define G_UNREF_PARAM(x) (void)(x);
