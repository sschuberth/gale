#ifndef CPUINFO_H
#define CPUINFO_H

/**
 * \file
 * Means to identify CPUs and their features in detail.
 */

#include "gale/global/singleton.h"

namespace gale {

namespace system {

/**
 * Using the x86 CPUID instruction, this class reports detailed processor
 * capabilities. For details refer to:
 *
 * - Intel Processor Identification and the CPUID Instruction
 *   http://www.intel.com/design/xeon/applnots/241618.htm 
 *
 * - AMD CPUID Specification
 *   http://www.amd.com/us-en/assets/content_type/white_papers_and_tech_docs/25481.pdf
 */
class CPUInfo:public global::Singleton<CPUInfo> {
    friend class global::Singleton<CPUInfo>;

  public:
    /// Checks if this is a little-endian (as opposed to big-endian) machine.
    bool isLittleEndian() const {
        // See http://sjbaker.org/steve/software/cute_code.html.
        static int i=1;
        return *((char*)&i)!=0;
    }

    /**
     * \name CPUID helper methods
     */
    //@{
    /// Checks for the presence of the CPUID instruction.
    bool hasCPUID() const;
    /// Returns the highest standard function number supported.
    unsigned getMaxCPUIDStdFunc() const;
    /// Returns the highest extended function number supported.
    unsigned getMaxCPUIDExtFunc() const;
    //@}

    /**
     * \name Vendor identification methods
     */
    //@{
    /// Return the CPU vendor string.
    const char* getVendorString() const {
        return m_vendor;
    }

#define MAKE_DWORD(a,b,c,d) (a|(int)(b)<<8|(int)(c)<<16|(int)(d)<<24)
    /// Returns if this is an Intel CPU.
    bool isIntel() const {
        if (*((int*)&m_vendor[0])!=MAKE_DWORD('G','e','n','u'))
            return false;
        if (*((int*)&m_vendor[4])!=MAKE_DWORD('i','n','e','I'))
            return false;
        if (*((int*)&m_vendor[8])!=MAKE_DWORD('n','t','e','l'))
            return false;
        return true;
    }

    /// Returns if this is an AMD CPU.
    bool isAMD() const {
        if (*((int*)&m_vendor[0])!=MAKE_DWORD('A','u','t','h'))
            return false;
        if (*((int*)&m_vendor[4])!=MAKE_DWORD('e','n','t','i'))
            return false;
        if (*((int*)&m_vendor[8])!=MAKE_DWORD('c','A','M','D'))
            return false;
        return true;
    }
#undef MAKE_DWORD
    //@}

    /**
     * \name Features reported by the standard flags
     */
    //@{
    /// Returns whether the processor contains an on-chip Floating-Point Unit.
    bool hasFPU() const {
        return (m_std_feat_flags_edx&(1<<0))!=0;
    }

    /// Returns whether Virtual-8086 Mode Extensions are supported.
    bool hasVME() const {
        return (m_std_feat_flags_edx&(1<<1))!=0;
    }

    /// Returns whether Debugging Extensions like I/O breakpoints are supported.
    bool hasDE() const {
        return (m_std_feat_flags_edx&(1<<2))!=0;
    }

    /// Returns whether the processor supports the 4-MiB Page Size Extension.
    bool hasPSE() const {
        return (m_std_feat_flags_edx&(1<<3))!=0;
    }

    /// Returns whether a Time-Stamp Counter is available.
    bool hasTSC() const {
        return (m_std_feat_flags_edx&(1<<4))!=0;
    }

    /// Returns whether Model Specific Registers are implemented.
    bool hasMSR() const {
        return (m_std_feat_flags_edx&(1<<5))!=0;
    }

    /// Returns whether the Physical Address Extension (more than 32 bits) is
    /// supported.
    bool hasPAE() const {
        return (m_std_feat_flags_edx&(1<<6))!=0;
    }

    /// Returns whether the processor supports the Machine Check Exception.
    bool hasMCE() const {
        return (m_std_feat_flags_edx&(1<<7))!=0;
    }

    /// Returns whether the CMPXCHG8B instruction is supported.
    bool hasCX8() const {
        return (m_std_feat_flags_edx&(1<<8))!=0;
    }

    /// Returns whether an Advanced Programmable Interrupt Controller is
    /// available and enabled.
    bool hasAPIC() const {
        return (m_std_feat_flags_edx&(1<<9))!=0;
    }

    // Bit 10 is reserved (on Intel and AMD).

    /// Returns whether the processor supports the fast system call (SYSENTER /
    /// SYSEXIT) instructions.
    bool hasSEP() const {
        return (m_std_feat_flags_edx&(1<<11))!=0;
    }

    /// Returns whether the processor supports the Memory Type Range Registers.
    bool hasMTRR() const {
        return (m_std_feat_flags_edx&(1<<12))!=0;
    }

    /// Returns whether the Page Global Extensions are supported in the page
    /// directory / table entries.
    bool hasPGE() const {
        return (m_std_feat_flags_edx&(1<<13))!=0;
    }

    /// Returns whether the Machine Check Architecture is supported.
    bool hasMCA() const {
        return (m_std_feat_flags_edx&(1<<14))!=0;
    }

    /// Returns whether the processor supports conditional move instructions.
    bool hasCMOV() const {
        return (m_std_feat_flags_edx&(1<<15))!=0;
    }

    /// Returns whether the processor supports the Page Attribute Table.
    bool hasPAT() const {
        return (m_std_feat_flags_edx&(1<<16))!=0;
    }

    /// Returns whether the processor allows the Page Size Extension to address
    /// physical memory beyond 4 GiB.
    bool hasPSE36() const {
        return (m_std_feat_flags_edx&(1<<17))!=0;
    }

    /// Returns whether the processor has a 96-bit serial number (reserved on
    /// AMD).
    bool hasPSN() const {
        return (m_std_feat_flags_edx&(1<<18))!=0;
    }

    /// Returns whether the CLFLUSH instruction is supported.
    bool hasCLFSH() const {
        return (m_std_feat_flags_edx&(1<<19))!=0;
    }

    // Bit 20 is reserved (on Intel and AMD).

    /// Returns whether the processor is able to write a history of branch
    /// addresses into a Debug Store (reserved on AMD).
    bool hasDS() const {
        return (m_std_feat_flags_edx&(1<<21))!=0;
    }

    /// Returns whether the processor implements temperature monitoring and
    /// performance modulation under software control (reserved on AMD).
    bool hasACPI() const {
        return (m_std_feat_flags_edx&(1<<22))!=0;
    }

    /// Returns whether the processor supports the MMX instruction set.
    bool hasMMX() const {
        return (m_std_feat_flags_edx&(1<<23))!=0;
    }

    /// Returns whether the fast floating-point save and restore instructions
    /// (FXSAVE / FXRSTOR) are supported.
    bool hasFXSR() const {
        return (m_std_feat_flags_edx&(1<<24))!=0;
    }

    /// Returns whether the Streaming SIMD Extension instruction set is
    /// supported.
    bool hasSSE() const {
        return (m_std_feat_flags_edx&(1<<25))!=0;
    }

    /// Returns whether the Streaming SIMD Extension 2 instruction set is
    /// supported.
    bool hasSSE2() const {
        return (m_std_feat_flags_edx&(1<<26))!=0;
    }

    /// Returns whether the processor supports Self-Snooping of its cache to
    /// manage conflicting memory types (reserved on AMD).
    bool hasSS() const {
        return (m_std_feat_flags_edx&(1<<27))!=0;
    }

    /// Returns whether Hyper-Threading Technology capability is present,
    /// allowing the processor to operate as multiple logical units.
    bool hasHTT() const {
        return (m_std_feat_flags_edx&(1<<28))!=0;
    }

    /// Returns whether the processor implements the Thermal Monitor automatic
    /// thermal control circuit (reserved on AMD).
    bool hasTM() const {
        return (m_std_feat_flags_edx&(1<<29))!=0;
    }

    /// Returns whether the processor has capabilities of the Intel Itanium
    /// processor family and is currently operating in IA32 emulation mode
    /// (reserved on AMD).
    bool hasIA64() const {
        return (m_std_feat_flags_edx&(1<<30))!=0;
    }

    /// Returns whether the processor supports returning from stop-clock state
    /// to handle interrupts.
    bool hasPBE() const {
        return (m_std_feat_flags_edx&(1<<31))!=0;
    }

    /// Returns whether the Streaming SIMD Extension 3 instruction set is
    /// supported.
    bool hasSSE3() const {
        return (m_std_feat_flags_ecx&(1<<0))!=0;
    }

    // Bits 1 and 2 are reserved (on Intel and AMD).

    bool hasEIST() const { return (m_std_feat_flags_ecx&(1<<7))!=0; }
    //@}

    bool has3DNow() const { return (m_ext_feat_flags_edx&(1<<31))!=0; }
    bool has3DNowExt() const { return (m_ext_feat_flags_edx&(1<<30))!=0; }
    bool hasMMXExt() const { return (m_ext_feat_flags_edx&(1<<22))!=0; }

  private:
    char m_vendor[3*4+1];
    int m_std_feat_flags_edx,m_std_feat_flags_ecx;
    int m_ext_feat_flags_edx,m_ext_feat_flags_ecx;

    CPUInfo();
};

extern CPUInfo& CPU;

} // namespace system

} // namespace gale

#endif // CPUINFO_H
