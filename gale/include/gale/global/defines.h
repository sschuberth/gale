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

#ifndef DEFINES_H
#define DEFINES_H

/**
 * \file
 * Global macro and type definitions
 */

/**
 * \def G_ASSERT(x)
 * Run-time assertion which resolves to a no-op when not debugging. The macro is
 * basically defined as in the C-standard.
 */

#ifdef G_ASSERT
    #undef G_ASSERT
#endif

#ifdef NDEBUG
    #define G_ASSERT(x) ((void)0);
#else
    #include <assert.h>
    #define G_ASSERT(x) assert(x);
#endif

/**
 * \def G_INLINE
 * Compiler-specific keyword definition to force a function to be inline.
 */

#ifdef G_INLINE
    #undef G_INLINE
#endif

#ifdef __GNUC__
    #define G_INLINE inline __attribute__ ((always_inline))
#elif defined(_MSC_VER)
    #define G_INLINE __forceinline
#else
    #define G_INLINE inline
#endif

#endif // DEFINES_H
