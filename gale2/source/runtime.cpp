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

#ifdef GALE_TINY

#include "gale/global/platform.h"

// MSVC needs this symbol as soon as floating-point numbers are used.
extern "C" int _fltused=0x9875;

// Forward declaration to the usual main() entry point.
extern "C" int main();

// This is the real entry point as used by the C runtime; just call the usual
// main() entry point and exit with its return value.
extern "C" int mainCRTStartup()
{
    ExitProcess(main());
}

void* operator new(size_t bytes)
{
    return HeapAlloc(GetProcessHeap(),0,bytes);
}

void* operator new[](size_t bytes)
{
    return HeapAlloc(GetProcessHeap(),0,bytes);
}

void* operator new(size_t bytes,void* place)
{
    UNREFERENCED_PARAMETER(bytes);
    return place;
}

void operator delete(void* pointer)
{
    if (pointer) {
        HeapFree(GetProcessHeap(),0,pointer);
    }
}

void operator delete[](void* pointer)
{
    if (pointer) {
        HeapFree(GetProcessHeap(),0,pointer);
    }
}

#endif // GALE_TINY
