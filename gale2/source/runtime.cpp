/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
 *
 * Copyright (C) 2005-2010  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
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

#ifdef GALE_TINY_CODE

#include "gale/global/platform.h"

#ifdef __cplusplus
extern "C" {
#endif

// MSVC needs this symbol as soon as floating-point numbers are used.
int _fltused=0x9875;

// Forward declaration to the usual main() entry point.
int __cdecl main();

// This is the real entry point as used by the C runtime; just call the usual
// main() entry point and exit with its return value.
int mainCRTStartup()
{
    ExitProcess(main());
}

#ifdef __cplusplus
};
#endif

/*
 * Replacements for C runtime functions
 */

int print_str(char const* str)
{
    DWORD length=0,written;
    while (str[length]) {
        ++length;
    }
    if (WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),str,length,&written,NULL)==FALSE || written!=length) {
        return -1;
    }
    return written;
}

int print_int(int num)
{
    // Longest integer string is "-2147483648".
    char buffer[11+1];
    int pos=11, digit;

    buffer[pos--]='\0';
    do {
        digit=num%10;
        num/=10;
        buffer[pos--]='0'+static_cast<char>(abs(digit));
    }
    while (num);

    if (digit<0) {
        buffer[pos--]='-';
    }

    return print_str(&buffer[pos+1]);
}

__declspec(noalias,restrict) void* __cdecl calloc(size_t num,size_t size)
{
    return GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT,num*size);
}

__declspec(noalias,restrict) void* __cdecl malloc(size_t size)
{
    return GlobalAlloc(GMEM_FIXED,size);
}

__declspec(noalias,restrict) void* __cdecl realloc(void* memblock,size_t size)
{
    if (memblock) {
        return GlobalReAlloc(memblock,size,GMEM_MOVEABLE);
    }
    else {
        return GlobalAlloc(GMEM_FIXED,size);
    }
}

__declspec(noalias) void __cdecl free(void* memblock)
{
    GlobalFree(memblock);
}

// "Enable Intrinsic Functions" (/Oi) is supposed to make memcpy and memset
// intrinsics, but that does not work in VS2005. Although the compiler claims it
// would work, the linker wants to pull in the functions from the CRT. So in
// order to implement our own minimal versions, we first need to force the
// functions to formally be a function, also see
// <http://connect.microsoft.com/VisualStudio/feedback/ViewFeedback.aspx?FeedbackID=101250>.

// Warning C4164: Intrinsic function not declared.
#pragma warning(disable:4164)
#pragma function(memcpy)
#pragma function(memset)
#pragma warning(default:4164)

void* __cdecl memcpy(void* dest,void const* src,size_t count)
{
    char* d=(char*)dest;
    char const* s=(char const*)src;

    while (count--) {
        *d++=*s++;
    }

    return dest;
}

void* __cdecl memmove(void* dest,void const* src,size_t count)
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

void* __cdecl memset(void* dest,int c,size_t count)
{
    char* d=(char*)dest;

    while (count--) {
        *d++=(char)c;
    }

    return dest;
}

/*
 * Replacements for non-inline math functions
 */

// Inspired by http://www.cubic.org/docs/download/tnymath2.h
__declspec(naked) double __cdecl _CIacos(/* double x */)
{
    // acos(x) = atan(sqrt(1 - sqr(x)) / x)
    // When this is called, the argument is in st(0)=x.
    __asm {
        sub esp,4
        fst DWORD PTR [esp]

        fmul st(0),st(0)
        fld1
        fsubrp st(1),st(0)

        fsqrt
        fdiv DWORD PTR [esp]

        fld1
        fpatan

        mov eax,DWORD PTR [esp]
        cmp eax,0x80000000
        jbe _acos_exit

        fldpi
        faddp st(1),st(0)

    _acos_exit:
        pop eax

        ret
    }
}

// Inspired by http://svn.reactos.org/svn/reactos/trunk/reactos/lib/sdk/crt/math/i386/fmod.c?view=markup
__declspec(naked) double __cdecl _CIfmod(/* double x,double y */)
{
    // x = i * y + f, where i is an integer, f has the same sign as x, and the
    // absolute value of f is less than the absolute value of y.
    // When this is called, the arguments are in st(0)=y and st(1)=x.
    __asm {
        fxch

    _fmod_remainder:
        fprem
        fstsw ax
        sahf
        jp _fmod_remainder

        fstp st(1)

        ret
    }
}

// Inspired by http://www.xyzw.de/c190.html
__declspec(naked) double __cdecl _CIpow(/* double x,double y */)
{
    // pow(x,y) = exp2(log2(x) * y)
    // When this is called, the arguments are in st(0)=y and st(1)=x.
    __asm {
        fxch

        ftst
        fstsw ax
        sahf
        jz _pow_exit

        fyl2x

        fld1
        fld st(1)
        fprem
        f2xm1

        faddp st(1),st(0)
        fscale

    _pow_exit:
        fstp st(1)

        ret
    }
}

/*
 * Replacements for C++ runtime functions
 */

void* __cdecl operator new(size_t bytes)
{
    return malloc(bytes);
}

void* __cdecl operator new[](size_t bytes)
{
    return malloc(bytes);
}

void* __cdecl operator new(size_t bytes,void* place)
{
    G_UNREF_PARAM(bytes)
    return place;
}

void __cdecl operator delete(void* pointer)
{
    free(pointer);
}

void __cdecl operator delete[](void* pointer)
{
    free(pointer);
}

void __cdecl operator delete(void* pointer,void* place)
{
    // Do nothing, not even calling a destructor.
    G_UNREF_PARAM(pointer)
    G_UNREF_PARAM(place)
}

#endif // GALE_TINY_CODE
