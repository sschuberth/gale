/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <https://sourceforge.net/projects/gale-opengl/>.
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
 * Platform independent type definitions
 */

#include "platform.h"

/// \typedef g_int8
/// Platform-independent signed 8-bit integer type.

/// \typedef g_int16;
/// Platform-independent signed 16-bit integer type.

/// \typedef g_int32;
/// Platform-independent signed 32-bit integer type.

/// \typedef g_int64;
/// Platform-independent signed 64-bit integer type.

/// \typedef g_uint8;
/// Platform-independent unsigned 8-bit integer type.

/// \typedef g_uint16;
/// Platform-independent unsigned 16-bit integer type.

/// \typedef g_uint32;
/// Platform-independent unsigned 32-bit integer type.

/// \typedef g_uint64;
/// Platform-independent unsigned 64-bit integer type.

/// \typedef g_real32;
/// Platform-independent 32-bit floating-point type.

/// \typedef g_real64;
/// Platform-independent 64-bit floating-point type.

#ifdef G_OS_LINUX

typedef int8_t g_int8;
typedef int16_t g_int16;
typedef int32_t g_int32;
typedef int64_t g_int64;

typedef uint8_t g_uint8;
typedef uint16_t g_uint16;
typedef uint32_t g_uint32;
typedef uint64_t g_uint64;

typedef float g_real32;
typedef double g_real64;

#endif // G_OS_LINUX

#ifdef G_OS_WINDOWS

typedef INT8 g_int8;
typedef INT16 g_int16;
typedef INT32 g_int32;
typedef INT64 g_int64;

typedef UINT8 g_uint8;
typedef UINT16 g_uint16;
typedef UINT32 g_uint32;
typedef UINT64 g_uint64;

typedef float g_real32;
typedef double g_real64;

#endif // G_OS_WINDOWS
