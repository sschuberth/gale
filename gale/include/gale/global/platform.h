/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
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

#ifndef PLATFORM_H
#define PLATFORM_H

/**
 * \file
 * Basic platform specific includes
 */

#ifdef _WIN32
    /**
     * \def NOMINMAX
     * Suppress the min and max macro definitions in Windef.h to avoid conflicts
     * with STL, see
     * <http://support.microsoft.com/default.aspx?scid=kb;en-us;143208>.
     */
    #ifndef NOMINMAX
        #define NOMINMAX 1
    #endif

    /**
     * \def VC_EXTRA_LEAN
     * Exclude rarely-used services from Windows headers, thus reducing the size
     * of the files and speeding up the build process, see
     * <http://support.microsoft.com/default.aspx?scid=kb;en-us;166474>.
     */
    #ifndef VC_EXTRA_LEAN
        #define VC_EXTRA_LEAN 1
    #endif

    /**
     * \def WIN32_LEAN_AND_MEAN
     * Exclude rarely-used services from Windows headers, thus reducing the size
     * of the files and speeding up the build process, see
     * <http://support.microsoft.com/default.aspx?scid=kb;en-us;166474>.
     */
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN 1
    #endif

    /**
     * \def _WIN32_WINNT
     * Assume at least Windows 2000 to get some additional defines, see
     * <http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winprog/winprog/using_the_windows_headers.asp>.
     */
    #if defined(_WIN32_WINNT) && _WIN32_WINNT<0x0500
        #undef _WIN32_WINNT
    #endif

    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT 0x0500
    #endif

    #include <windows.h>

    /**
     * \def PFD_SUPPORT_COMPOSITION
     * Pixel format flag to enable composition of OpenGL contexts in Windows
     * Vista, see <http://opengl.org/pipeline/article/vol003_7/>.
     */
    #ifndef PFD_SUPPORT_COMPOSITION
        #define PFD_SUPPORT_COMPOSITION 0x00008000
    #endif
#else
    #include <sys/times.h>
    #include <unistd.h>
#endif

#include <GL/gl.h>

#endif // PLATFORM_H
