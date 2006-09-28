#include "gale/system/cpuinfo.h"

namespace gale {

namespace system {

CPUInfo& CPU=CPUInfo::the();

CPUInfo::CPUInfo()
:m_feat_flags_edx(0),m_feat_flags_ecx(0),m_ext_feat_flags_edx(0) {
    // Null-terminate the vendor string.
    m_vendor[0]=m_vendor[3*4]=0;

    // Until now, the ECX register contains the "this" pointer.
    if (!hasCPUID()) return;

    // Check for CPUID feature flags support.
    #ifdef __GNUC__
    if (getMaxCPUIDFunc()>0) {
        __asm__(
            "xor eax,eax\n"
            "inc eax\n"
            "cpuid\n"
            : "=d" (m_feat_flags_edx), "=c" (m_feat_flags_ecx)
            : : "eax", "ebx"
        );
    }
    #else
    if (getMaxCPUIDFunc()>0) {
        __asm {
            xor eax,eax
            inc eax
            cpuid
            mov eax,this
            mov [eax]CPUInfo.m_feat_flags_edx,edx
            mov [eax]CPUInfo.m_feat_flags_ecx,ecx
        }
    }
    #endif

    // Check for CPUID extended feature flags support.
    #ifdef __GNUC__
    if (getMaxCPUIDExtFunc()>0) {
        __asm__(
            "mov eax,0x80000001\n"
            "cpuid\n"
            : "=d" (m_ext_feat_flags_edx)
            : : "eax", "ebx", "ecx"
        );
    }
    #else
    if (getMaxCPUIDExtFunc()>0) {
        __asm {
            mov eax,80000001h
            cpuid
            mov eax,this
            mov [eax]CPUInfo.m_ext_feat_flags_edx,edx
        }
    }
    #endif
}

#ifdef __GNUC__
// Brennan's Guide to Inline Assembly
// http://www.delorie.com/djgpp/doc/brennan/brennan_att_inline_djgpp.html

// Check for the presence of the CPUID instruction.
bool CPUInfo::hasCPUID() const {
    unsigned eax;
    __asm__(
        "// Read and invert the ID bit (21).\n"
        "pushfd\n"
        "pop eax\n"
        "xor eax,0x00200000\n"
        "mov ecx,eax\n"
        "// Write and read back modified flags.\n"
        "push eax\n"
        "popfd\n"
        "pushfd\n"
        "pop eax\n"
        "// Check if inverting the ID bit was successful.\n"
        "cmp eax,ecx\n"
        "sete cl\n"
        "movzx eax,cl\n"
        : "=a" (eax)
        : : "ecx"
    );
    return eax;
}

// Get the maximum input value supported by the CPUID instruction.
unsigned CPUInfo::getMaxCPUIDFunc() const {
    unsigned eax,*vendor=(unsigned*)m_vendor;
    unsigned *vendor4=(unsigned*)&m_vendor[4];
    unsigned *vendor8=(unsigned*)&m_vendor[8];
    __asm__(
        "xor eax,eax\n"
        "cpuid\n"
        : "=a" (eax), "=b" (*vendor), "=d" (*vendor4), "=c" (*vendor8)
    );
    return eax;
}

// Get the maximum extended function input value supported.
unsigned CPUInfo::getMaxCPUIDExtFunc() const {
    unsigned eax;
    __asm__(
        "mov eax,0x80000000\n"
        "push eax\n"
        "cpuid\n"
        "pop ebx\n"
        "sub eax,ebx\n"
        : "=a" (eax)
        : : "ebx", "ecx", "edx"
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

// Check for the presence of the CPUID instruction.
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

// Get the maximum input value supported by the CPUID instruction.
__declspec(naked) unsigned CPUInfo::getMaxCPUIDFunc() const {
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
        mov [edi+m_vendor],ebx
        mov [edi+m_vendor+4],edx
        mov [edi+m_vendor+8],ecx
#endif
        pop edi
        pop ebx
        ret
    }
}

// Get the maximum extended function input value supported.
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
