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
 * Minimal replacements for C runtime functions
 */

#ifdef GALE_TINY

// In order to work without linking against the CRT, the following C/C++ compiler
// settings need to be adjust from within the IDE.
//
// Optimization:
// Set "Enable Intrinsic Functions" to "Yes" (/Oi)
//
// Code Generation:
// Set "Enable C++ Exceptions" to "No"
// Set "Buffer Security Check" to "No" (/GS-)
// Set "Floating Point Model" to "Fast" (/fp:fast)

#pragma optimize("gsy",on)
#pragma comment(linker,"/nodefaultlib")

#ifdef __cplusplus
extern "C" {
#endif

/// Minimal "puts" function implementation.
#define puts(str) print(#str "\n")
int print(char const* str);

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

#ifdef __cplusplus
};
#endif

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

#endif // GALE_TINY

#endif // RUNTIME_H
