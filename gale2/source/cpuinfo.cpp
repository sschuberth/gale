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

#include "gale/global/defines.h"
#include "gale/system/cpuinfo.h"

#ifdef G_ARCH_X86_64
    #include <intrin.h>
#endif

namespace gale {

namespace system {

CPUInfo& CPU=CPUInfo::the();

CPUInfo::CPUInfo():
  m_std_misc_info(0),m_std_cache_params(0),m_ext_address_sizes(0),
  m_std_feat_flags_edx(0),m_std_feat_flags_ecx(0),
  m_ext_feat_flags_edx(0),m_ext_feat_flags_ecx(0)
{
    // Null-terminate the vendor string.
    m_vendor[0]=m_vendor[3*4]='\0';

    if (!hasCPUID()) {
        return;
    }

#ifdef G_ARCH_X86_64
    int info[4];
#endif

    if (getMaxCPUIDStdFunc()>=1) {
        // CPUID function 0x00000001 output:
        // EAX = Processor signature
        // EBX = APIC ID, processor count, CLFLUSH size, brand ID
        // ECX = Standard feature flags (part 2)
        // EDX = Standard feature flags (part 1)
#ifdef G_COMP_GNUC

        __asm__(
            "xorl %%eax,%%eax\n\t"
            "incl %%eax\n\t"
            "pushl %%ebx\n\t"
            "cpuid\n\t"
            "movl %%ebx,%%eax\n\t"
            "popl %%ebx\n\t"
            : "=a" (m_std_misc_info),       /* Output  */
              "=d" (m_std_feat_flags_edx),
              "=c" (m_std_feat_flags_ecx)
            :                               /* Input   */
            : "cc"                          /* Clobber */
        );

#else // G_COMP_GNUC

#ifdef G_ARCH_X86_64

        __cpuid(info,0x00000001);
        m_std_misc_info=info[1];
        m_std_feat_flags_edx=info[3];
        m_std_feat_flags_ecx=info[2];

#else // G_ARCH_X86_64

        __asm {
            xor eax,eax
            inc eax
            cpuid
            mov eax,this
            mov [eax]CPUInfo.m_std_misc_info,ebx
            mov [eax]CPUInfo.m_std_feat_flags_edx,edx
            mov [eax]CPUInfo.m_std_feat_flags_ecx,ecx
        }

#endif // G_ARCH_X86_64

#endif // G_COMP_GNUC
    }

    if (getMaxCPUIDStdFunc()>=4) {
        // Required to get the number of cores on Intel processors.
#ifdef G_COMP_GNUC

        __asm__(
            "movl $0x00000004,%%eax\n\t"
            "xorl %%ecx,%%ecx\n\t"
            "pushl %%ebx\n\t"
            "cpuid\n\t"
            "popl %%ebx\n\t"
            : "=a" (m_std_cache_params)  /* Output  */
            :                            /* Input   */
            : "%ecx", "%edx", "cc"       /* Clobber */
        );

#else // G_COMP_GNUC

#ifdef G_ARCH_X86_64

        __cpuid(info,0x00000004);
        m_std_cache_params=info[0];

#else // G_ARCH_X86_64

        __asm {
            mov eax,00000004h
            xor ecx,ecx
            cpuid
            mov ecx,this
            mov [ecx]CPUInfo.m_std_cache_params,eax
        }

#endif // G_ARCH_X86_64

#endif // G_COMP_GNUC
    }

    if (getMaxCPUIDExtFunc()>=1) {
        // CPUID function 0x80000001 output:
        // EAX = Reserved (Intel) / processor signature (AMD)
        // EBX = Reserved (Intel) / APIC ID etc. (AMD)
        // ECX = Extended feature flags (part 2)
        // EDX = Extended feature flags (part 1)
#ifdef G_COMP_GNUC

        __asm__(
            "movl $0x80000001,%%eax\n\t"
            "pushl %%ebx\n\t"
            "cpuid\n\t"
            "popl %%ebx\n\t"
            : "=d" (m_ext_feat_flags_edx),  /* Output  */
              "=c" (m_ext_feat_flags_ecx)
            :                               /* Input   */
            : "%eax"                        /* Clobber */
        );

#else // G_COMP_GNUC

#ifdef G_ARCH_X86_64

        __cpuid(info,0x80000001);
        m_ext_feat_flags_edx=info[3];
        m_ext_feat_flags_ecx=info[2];

#else // G_ARCH_X86_64

        __asm {
            mov eax,80000001h
            cpuid
            mov eax,this
            mov [eax]CPUInfo.m_ext_feat_flags_edx,edx
            mov [eax]CPUInfo.m_ext_feat_flags_ecx,ecx
        }

#endif // G_ARCH_X86_64

#endif // G_COMP_GNUC
    }

    if (getMaxCPUIDExtFunc()>=8) {
        // Required to get the number of cores on AMD processors.
#ifdef G_COMP_GNUC

        __asm__(
            "movl $0x80000008,%%eax\n\t"
            "pushl %%ebx\n\t"
            "cpuid\n\t"
            "popl %%ebx\n\t"
            : "=c" (m_ext_address_sizes)  /* Output  */
            :                             /* Input   */
            : "%eax", "%edx"              /* Clobber */
        );

#else // G_COMP_GNUC

#ifdef G_ARCH_X86_64

        __cpuid(info,0x80000008);
        m_ext_address_sizes=info[2];

#else // G_ARCH_X86_64

        __asm {
            mov eax,80000008h
            cpuid
            mov eax,this
            mov [eax]CPUInfo.m_ext_address_sizes,ecx
        }

#endif // G_ARCH_X86_64

#endif // G_COMP_GNUC
    }
}

#ifdef G_COMP_GNUC

// See Brennan's Guide to Inline Assembly,
// <http://www.delorie.com/djgpp/doc/brennan/brennan_att_inline_djgpp.html>

// See Machine Constraints - Using the GNU Compiler Collection (GCC),
// <http://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints>

// Always preserve the EBX register to be compatible with the -fPIC option.
// Note that specifying EBX in the clobber list does *not* work!

bool CPUInfo::hasCPUID() const
{
    unsigned int eax;
    __asm__(
        "pushfl\n\t"
        "popl %%eax\n\t"
        "xorl $0x00200000,%%eax\n\t"
        "movl %%eax,%%ecx\n\t"
        "pushl %%eax\n\t"
        "popfl\n\t"
        "pushfl\n\t"
        "popl %%eax\n\t"
        "cmpl %%ecx,%%eax\n\t"
        "sete %%cl\n\t"
        "movzxl %%cl,%%eax\n\t"
        : "=a" (eax)    /* Output  */
        :               /* Input   */
        : "%ecx", "cc"  /* Clobber */
    );
    return eax;
}

unsigned int CPUInfo::getMaxCPUIDStdFunc()
{
    unsigned int eax;
    int* vendor0=reinterpret_cast<int*>(m_vendor);
    int* vendor4=vendor0+1;
    int* vendor8=vendor4+1;
    __asm__(
        "xorl %%eax,%%eax\n\t"
        "pushl %%ebx\n\t"
        "cpuid\n\t"
        "movl %%ebx,%1\n\t"
        "popl %%ebx\n\t"
        : "=a" (eax),
          "=r" (*vendor0),
          "=d" (*vendor4),
          "=c" (*vendor8)  /* Output  */
        :                  /* Input   */
        : "cc"             /* Clobber */
    );
    return eax;
}

unsigned int CPUInfo::getMaxCPUIDExtFunc() const
{
    unsigned int eax;
    __asm__(
        "movl $0x80000000,%%eax\n\t"
        "pushl %%ebx\n\t"
        "pushl %%eax\n\t"
        "cpuid\n\t"
        "popl %%ecx\n\t"
        "popl %%ebx\n\t"
        "subl %%ecx,%%eax\n\t"
        : "=a" (eax)            /* Output  */
        :                       /* Input   */
        : "%ecx", "%edx", "cc"  /* Clobber */
    );
    return eax;
}

#else // G_COMP_GNUC

#ifdef G_ARCH_X86_64

bool CPUInfo::hasCPUID() const
{
    // All x86_64 CPUs have the CPUID instruction.
    return true;
}

unsigned int CPUInfo::getMaxCPUIDStdFunc()
{
    int info[4];
    __cpuid(info,0);
    *reinterpret_cast<int*>(&m_vendor[0])=info[1];
    *reinterpret_cast<int*>(&m_vendor[4])=info[3];
    *reinterpret_cast<int*>(&m_vendor[8])=info[2];
    return static_cast<unsigned int>(info[0]);
}

unsigned int CPUInfo::getMaxCPUIDExtFunc() const
{
    int info[4];
    __cpuid(info,0x80000000);
    return static_cast<unsigned int>(info[0]);
}

#else // G_ARCH_X86_64

// When using __asm to write assembly language in C/C++ functions, you don't
// need to preserve the EAX, EBX, ECX, EDX, ESI, or EDI registers. However, by
// using EBX, ESI or EDI in inline assembly code, you force the compiler to
// save and restore those registers in the function prologue and epilogue. Note
// that if you use the __declspec(naked) modifier to prevent generation of
// prologue and epilogue code you will have to preserve the modified registers
// by yourself.

__declspec(naked) bool CPUInfo::hasCPUID() const
{
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

__declspec(naked) unsigned int CPUInfo::getMaxCPUIDStdFunc()
{
    __asm {
        // Get the maximum standard function number.
        xor eax,eax

        // Save registers that are assumed to be unmodified.
        push edi
        push ebx

        // Preserve the "this" pointer in ECX.
        push ecx
        cpuid
        pop edi

        // Write the vendor string.
        lea edi,[edi]CPU.m_vendor
        mov [edi],ebx
        mov [edi+4],edx
        mov [edi+8],ecx

        // Restore registers that are assumed to be unmodified.
        pop ebx
        pop edi

        ret
    }
}

__declspec(naked) unsigned int CPUInfo::getMaxCPUIDExtFunc() const
{
    __asm {
        // Get the maximum extended function number.
        mov eax,80000000h

        // Save registers that are assumed to be unmodified.
        push ebx

        // Preserve the CPUID function in EAX.
        push eax
        cpuid
        pop ecx

        // Restore registers that are assumed to be unmodified.
        pop ebx

        // Start function numbering at 0.
        sub eax,ecx

        ret
    }
}

#endif // G_ARCH_X86_64

#endif // G_COMP_GNUC

} // namespace system

} // namespace gale
