/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at http://developer.berlios.de/projects/gale/
 *
 * Copyright (C) 2005-2006  Sebastian Schuberth <sschuberth@gmail.com>
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

#include "gale/system/cpuinfo.h"

namespace gale {

namespace system {

CPUInfo& CPU=CPUInfo::the();

CPUInfo::CPUInfo():
  m_std_feat_flags_edx(0),m_std_feat_flags_ecx(0),
  m_ext_feat_flags_edx(0),m_ext_feat_flags_ecx(0)
{
    // Null-terminate the vendor string.
    m_vendor[0]=m_vendor[3*4]=0;

    // Until now, the ECX register contains the "this" pointer.
    if (!hasCPUID())
        return;

    // Check for CPUID standard feature flags support.
    if (getMaxCPUIDStdFunc()>0) {
        // EAX = Processor Signature
        // EBX = Brand ID etc.
        // EDX = Standard Feature Flags (part 1)
        // ECX = Standard Feature Flags (part 2)
#ifdef __GNUC__
        __asm__(
            "xorl %%eax,%%eax\n\t"
            "incl %%eax\n\t"
            "pushl %%ebx\n\t"
            "cpuid\n\t"
            "popl %%ebx\n\t"
            : "=d" (m_std_feat_flags_edx), "=c" (m_std_feat_flags_ecx)
            :
            : "eax"
        );
#else
        __asm {
            xor eax,eax
            inc eax
            cpuid
            mov eax,this
            mov [eax]CPUInfo.m_std_feat_flags_edx,edx
            mov [eax]CPUInfo.m_std_feat_flags_ecx,ecx
        }
#endif
    }

    // Check for CPUID extended feature flags support.
    if (getMaxCPUIDExtFunc()>0) {
        // EAX = Reserved (Intel) / Processor Signature (AMD)
        // EBX = Reserved (Intel) / Brand ID etc. (AMD)
        // EDX = Extended Feature Flags (part 1)
        // ECX = Extended Feature Flags (part 2)
#ifdef __GNUC__
        __asm__(
            "movl $0x80000001,%%eax\n\t"
            "pushl %%ebx\n\t"
            "cpuid\n\t"
            "popl %%ebx\n\t"
            : "=d" (m_ext_feat_flags_edx), "=c" (m_ext_feat_flags_ecx)
            :
            : "%eax"
        );
#else
        __asm {
            mov eax,80000001h
            cpuid
            mov eax,this
            mov [eax]CPUInfo.m_ext_feat_flags_edx,edx
            mov [eax]CPUInfo.m_ext_feat_flags_ecx,ecx
        }
#endif
    }
}

#ifdef __GNUC__

// Brennan's Guide to Inline Assembly
// http://www.delorie.com/djgpp/doc/brennan/brennan_att_inline_djgpp.html

// Always preserve the EBX register to be compatible with the -fPIC option.
// Note that specifying EBX in the clobber list does *not* work!

bool CPUInfo::hasCPUID() const {
    unsigned eax;
    __asm__(
        "// Read and invert the ID bit (21).\n\t"
        "pushfl\n\t"
        "popl %%eax\n\t"
        "xorl $0x00200000,%%eax\n\t"
        "movl %%eax,%%ecx\n\t"
        "// Write and read back modified flags.\n\t"
        "pushl %%eax\n\t"
        "popfl\n\t"
        "pushfl\n\t"
        "popl %%eax\n\t"
        "// Check if inverting the ID bit was successful.\n\t"
        "cmpl %%ecx,%%eax\n\t"
        "sete %%cl\n\t"
        "movzx %%cl,%%eax\n\t"
        : "=a" (eax)
        :
        : "%ecx"
    );
    return eax;
}

unsigned CPUInfo::getMaxCPUIDStdFunc() const {
    unsigned eax,*vendor=(unsigned*)m_vendor;
    unsigned *vendor4=(unsigned*)&m_vendor[4];
    unsigned *vendor8=(unsigned*)&m_vendor[8];
    __asm__(
        "xorl %%eax,%%eax\n\t"
        "pushl %%ebx\n\t"
        "cpuid\n\t"
        "movl %%ebx,%1\n\t"
        "popl %%ebx\n\t"
        : "=a" (eax), "=r" (*vendor), "=d" (*vendor4), "=c" (*vendor8)
    );
    return eax;
}

unsigned CPUInfo::getMaxCPUIDExtFunc() const {
    unsigned eax;
    __asm__(
        "movl $0x80000000,%%eax\n\t"
        "pushl %%eax\n\t"
        "pushl %%ebx\n\t"
        "cpuid\n\t"
        "popl %%ebx\n\t"
        "popl %%ecx\n\t"
        "subl %%ecx,%%eax\n\t"
        : "=a" (eax)
        :
        : "%ecx", "%edx"
    );
    return eax;
}

#else // __GNUC__

// When using __asm to write assembly language in C/C++ functions, you don't
// need to preserve the EAX, EBX, ECX, EDX, ESI, or EDI registers. However, by
// using EBX, ESI or EDI in inline assembly code, you force the compiler to
// save and restore those registers in the function prologue and epilogue. Note
// that if you use the __declspec(naked) modifier to prevent generation of
// prologue and epilogue code you will have to preserve the modified registers
// by yourself.

__declspec(naked) bool CPUInfo::hasCPUID() const {
    __asm {
        // Read and invert the ID bit (21).
        pushfd
        pop eax
        xor eax,00200000h
        mov ecx,eax
        // Write and read back modified flags.
        push eax
        popfd
        pushfd
        pop eax
        // Check if inverting the ID bit was successful.
        cmp eax,ecx
        sete cl
        movzx eax,cl
        ret
    }
}

__declspec(naked) unsigned CPUInfo::getMaxCPUIDStdFunc() const {
    __asm {
        // No prologue / epilogue is generated, so save EBX and EDI manually.
        push ebx
        push edi
        // Preserve the "this" pointer.
        push ecx
        xor eax,eax
        cpuid
        pop edi
#ifdef __INTEL_COMPILER
        mov [edi+CPUInfo.m_vendor],ebx
        mov [edi+CPUInfo.m_vendor+4],edx
        mov [edi+CPUInfo.m_vendor+8],ecx
#else
        lea edi,[edi]CPU.m_vendor
        mov [edi],ebx
        mov [edi+4],edx
        mov [edi+8],ecx
#endif
        pop edi
        pop ebx
        ret
    }
}

__declspec(naked) unsigned CPUInfo::getMaxCPUIDExtFunc() const {
    __asm {
        // No prologue / epilogue is generated, so save EBX manually.
        push ebx
        mov eax,80000000h
        push eax
        cpuid
        pop ebx
        sub eax,ebx
        pop ebx
        ret
    }
}

#endif // __GNUC__

} // namespace system

} // namespace gale