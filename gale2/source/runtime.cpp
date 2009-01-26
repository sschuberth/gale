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
#include "gale/system/runtime.h"

static HANDLE stdout;

#ifdef __cplusplus
extern "C" {
#endif

// MSVC needs this symbol as soon as floating-point numbers are used.
int _fltused=0x9875;

// Forward declaration to the usual main() entry point.
int main();

// This is the real entry point as used by the C runtime; just call the usual
// main() entry point and exit with its return value.
int mainCRTStartup()
{
    stdout=GetStdHandle(STD_OUTPUT_HANDLE);
    ExitProcess(main());
}

#ifdef __cplusplus
};
#endif

/*
 * Minimal C function implementations
 */

int print(char const* str)
{
    DWORD length=0,written;
    while (str[length]) {
        ++length;
    }
    if (WriteFile(stdout,str,length,&written,NULL)==FALSE || written!=length) {
        return -1;
    }
    return written;
}

__declspec(noalias,restrict) void* calloc(size_t num,size_t size)
{
    return HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,num*size);
}

__declspec(noalias,restrict) void* malloc(size_t size)
{
    return HeapAlloc(GetProcessHeap(),0,size);
}

__declspec(noalias,restrict) void* realloc(void* memblock,size_t size)
{
    if (memblock) {
        return HeapReAlloc(GetProcessHeap(),0,memblock,size);
    }
    else {
        return HeapAlloc(GetProcessHeap(),0,size);
    }
}

__declspec(noalias) void free(void* memblock)
{
    if (memblock) {
        HeapFree(GetProcessHeap(),0,memblock);
    }
}

void* memcpy(void* dest,void const* src,size_t count)
{
    char* d=(char*)dest;
    char const* s=(char const*)src;

    for (size_t i=0;i<count;++i) {
        *d++=*s++;
    }

    return dest;
}

void* memmove(void* dest,void const* src,size_t count)
{
    char* d=(char*)dest;
    char const* s=(char const*)src;

    if (d<=s) {
        return memcpy(dest,src,count);
    }
    else {
        d+=count;
        s+=count;
        while (count--) {
            *(--d)=*(--s);
        }
    }

    return dest;
}

/*
 * Minimal C++ operator implementations
 */

void* operator new(size_t bytes)
{
    return malloc(bytes);
}

void* operator new[](size_t bytes)
{
    return malloc(bytes);
}

void* operator new(size_t bytes,void* place)
{
    UNREFERENCED_PARAMETER(bytes);
    return place;
}

void operator delete(void* pointer)
{
    free(pointer);
}

void operator delete[](void* pointer)
{
    free(pointer);
}

void operator delete(void* pointer,void* place)
{
    // Do nothing, not even calling a destructor.
}

#endif // GALE_TINY
