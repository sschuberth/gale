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

#include <assert.h>

/**
 * \def G_ASSERT_CALL
 * Run-time assertion for function calls.
 */

#ifdef G_ASSERT_CALL
    #undef G_ASSERT_CALL
#endif

#ifndef NDEBUG
    #define G_ASSERT_CALL(x) assert(x);
#else
    #define G_ASSERT_CALL(x) x;
#endif

/**
 * \def G_ASSERT_OPENGL
 * Run-time assertion for the OpenGL error flag.
 */

#ifdef G_ASSERT_OPENGL
    #undef G_ASSERT_OPENGL
#endif

#define G_ASSERT_OPENGL assert(glGetError()==GL_NO_ERROR);

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
