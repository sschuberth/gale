/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
 *
 * Copyright (C) 2005-2009  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
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

#ifndef CPUINFO_H
#define CPUINFO_H

/**
 * \file
 * Means to identify CPUs and their features in detail
 */

#include "../global/platform.h"
#include "../global/singleton.h"

namespace gale {

namespace system {

/**
 * Using the x86 CPUID instruction, this class reports detailed processor
 * capabilities. For details see these (or more recent) links:
 *
 * - Intel Processor Identification and the CPUID Instruction
 *   <http://www.intel.com/design/xeon/applnots/241618.htm>
 *
 * - AMD CPUID Specification
 *   <http://www.amd.com/us-en/assets/content_type/white_papers_and_tech_docs/25481.pdf>
 */
class CPUInfo:public global::Singleton<CPUInfo>
{
    friend class global::Singleton<CPUInfo>;

  public:

    /// Checks if this is a little-endian (as opposed to big-endian) machine.
    bool isLittleEndian() const {
        // See <http://sjbaker.org/steve/software/cute_code.html>.
        static int const i=1;
        return *reinterpret_cast<char const*>(&i)!=0;
    }

    /**
     * \name CPUID helper methods
     */
    //@{

    /// Checks for the presence of the CPUID instruction.
    bool hasCPUID() const;

    /// Returns the highest standard function number supported and fills out the
    /// vendor string information.
    unsigned int maxCPUIDStdFunc() const;

    /// Returns the highest extended function number supported.
    unsigned int maxCPUIDExtFunc() const;

    //@}

    /**
     * \name Vendor identification methods
     */
    //@{

    /// Return the CPU vendor string.
    char const* vendorString() const {
        return m_vendor;
    }

    /// Convenience method that returns whether this is an Intel CPU.
    bool isIntel() const {
        int const* vendor=reinterpret_cast<int const*>(vendorString());

        if (*vendor!=*reinterpret_cast<int const*>("Genu")) {
            return false;
        }

        ++vendor;
        if (*vendor!=*reinterpret_cast<int const*>("ineI")) {
            return false;
        }

        ++vendor;
        if (*vendor!=*reinterpret_cast<int const*>("ntel")) {
            return false;
        }

        return true;
    }

    /// Convenience method that returns whether this is an AMD CPU.
    bool isAMD() const {
        int const* vendor=reinterpret_cast<int const*>(vendorString());

        if (*vendor!=*reinterpret_cast<int const*>("Auth")) {
            return false;
        }

        ++vendor;
        if (*vendor!=*reinterpret_cast<int const*>("enti")) {
            return false;
        }

        ++vendor;
        if (*vendor!=*reinterpret_cast<int const*>("cAMD")) {
            return false;
        }

        return true;
    }

    /// Convenience method that returns whether this is a Cyrix / VIA CPU.
    bool isCyrix() const {
        int const* vendor=reinterpret_cast<int const*>(vendorString());

        if (*vendor!=*reinterpret_cast<int const*>("Cyri")) {
            return false;
        }

        ++vendor;
        if (*vendor!=*reinterpret_cast<int const*>("xIns")) {
            return false;
        }

        ++vendor;
        if (*vendor!=*reinterpret_cast<int const*>("tead")) {
            return false;
        }

        return true;
    }

    /// Convenience method that returns whether this is a Centaur / VIA CPU.
    bool isCentaur() const {
        int const* vendor=reinterpret_cast<int const*>(vendorString());

        if (*vendor!=*reinterpret_cast<int const*>("Cent")) {
            return false;
        }

        ++vendor;
        if (*vendor!=*reinterpret_cast<int const*>("aurH")) {
            return false;
        }

        ++vendor;
        if (*vendor!=*reinterpret_cast<int const*>("auls")) {
            return false;
        }

        return true;
    }

    //@}

    /**
     * \name Methods to determine the number of processing units, see
     * <http://cache-www.intel.com/cd/00/00/27/66/276611_276611.txt>
     */
    //@{

    /// Returns the number of processors (i.e. CPU sockets).
    unsigned int processors() const {
        unsigned int count=1;

#ifdef G_OS_LINUX

        // Get the number of "online" processors. This counts processor cores,
        // but not logical processors.
        count=sysconf(_SC_NPROCESSORS_ONLN);

#elif defined G_OS_WINDOWS

        SYSTEM_INFO info;
        GetSystemInfo(&info);

        // This counts all logical processors including processor cores.
        count=info.dwNumberOfProcessors;

        // Divide by the number of logical processors per processor core.
        count/=logicalProcsPerCore();

#endif

        // Divide by the number of processor cores per physical processor.
        count/=coresPerPhysicalProc();

        return count<1?1:count;
    }

    /// Returns the number of processor cores per physical processor.
    unsigned int coresPerPhysicalProc() const {
        unsigned int count=1;

        if (hasHTT()) {
            if (isIntel()) {
                // Get the maximum number of logical processors per physical processor.
                unsigned int max_lpp=(m_00000001_ebx&0x00ff0000UL)>>16;

                // Get the maximum number of processor cores per physical processor.
                unsigned int max_cpp=((m_00000004_eax&0xfc000000UL)>>26)+1;

                // Calculate the maximum number of logical processors per processor core.
                unsigned int max_lpc=max_lpp/max_cpp;

                count=max_cpp/max_lpc;
            }
            else if (isAMD()) {
                count=(m_80000008_ecx&0x000000ffUL)+1;
            }
        }

        return count<1?1:count;
    }

    /// Returns the number of logical processors per processor core.
    unsigned int logicalProcsPerCore() const {
        unsigned int count=1;

        if (isIntel() && hasHTT()) {
            if (maxCPUIDStdFunc()>=0x0b) {

#ifdef G_COMP_MSVC

                // Dirty trick to set the ECX register to 0 ("Count") as MSVC 8.0
                // does not support inline assembly on the x86-64 architecture.
                __stosd(reinterpret_cast<DWORD*>(&count),0,0);

                // Return the number of factory-configured logical processors
                // at the thread level.
                int info[4];
                __cpuid(info,0x0000000b);
                count=info[1]&0x0000ffff;

#elif defined(G_COMP_GNUC) // G_COMP_MSVC

                unsigned int m_0000000b_ebx;

                __asm__(
                    "movl $0x0000000b,%%eax\n\t"
                    "xorl %%ecx,%%ecx\n\t"
                    "movl %%ebx,%%esi\n\t"
                    "cpuid\n\t"
                    "xchg %%ebx,%%esi\n\t"
                    : "=S" (m_0000000b_ebx)         /* Output  */
                    :                               /* Input   */
                    : "%eax", "%ecx", "%edx", "cc"  /* Clobber */
                );

                count=m_0000000b_ebx&0x0000ffff;

#endif

            }
            else {
                // Get the maximum number of logical processors per physical processor.
                unsigned int max_lpp=(m_00000001_ebx&0x00ff0000UL)>>16;

                // Get the maximum number of processor cores per physical processor.
                unsigned int max_cpp=((m_00000004_eax&0xfc000000UL)>>26)+1;

                // Calculate the maximum number of logical processors per processor core.
                count=max_lpp/max_cpp;
            }
        }

        return count<1?1:count;
    }

    //@}

    /**
     * \name Features reported by the standard flags
     */
    //@{

    /* Features returned in the EDX register */

    /// Returns whether the processor contains an on-chip Floating-Point Unit.
    bool hasFPU() const {
        return (m_00000001_edx&(1<<0))!=0;
    }

    /// Returns whether Virtual-8086 Mode Extensions are supported.
    bool hasVME() const {
        return (m_00000001_edx&(1<<1))!=0;
    }

    /// Returns whether Debugging Extensions like I/O breakpoints are supported.
    bool hasDE() const {
        return (m_00000001_edx&(1<<2))!=0;
    }

    /// Returns whether the processor supports the 4-MiB Page Size Extension.
    bool hasPSE() const {
        return (m_00000001_edx&(1<<3))!=0;
    }

    /// Returns whether a Time-Stamp Counter is available.
    bool hasTSC() const {
        return (m_00000001_edx&(1<<4))!=0;
    }

    /// Returns whether Model Specific Registers are implemented.
    bool hasMSR() const {
        return (m_00000001_edx&(1<<5))!=0;
    }

    /// Returns whether the Physical Address Extension (more than 32 bits) is
    /// supported.
    bool hasPAE() const {
        return (m_00000001_edx&(1<<6))!=0;
    }

    /// Returns whether the processor supports the Machine Check Exception.
    bool hasMCE() const {
        return (m_00000001_edx&(1<<7))!=0;
    }

    /// Returns whether the CMPXCHG8B instruction is supported.
    bool hasCX8() const {
        return (m_00000001_edx&(1<<8))!=0;
    }

    /// Returns whether an Advanced Programmable Interrupt Controller is
    /// available and enabled.
    bool hasAPIC() const {
        return (m_00000001_edx&(1<<9))!=0;
    }

    // Bit 10 is reserved (on Intel and AMD).

    /// Returns whether the processor supports the fast system call (SYSENTER /
    /// SYSEXIT) instructions.
    bool hasSEP() const {
        return (m_00000001_edx&(1<<11))!=0;
    }

    /// Returns whether the processor supports the Memory Type Range Registers.
    bool hasMTRR() const {
        return (m_00000001_edx&(1<<12))!=0;
    }

    /// Returns whether the Page Global Extensions are supported in the page
    /// directory / table entries.
    bool hasPGE() const {
        return (m_00000001_edx&(1<<13))!=0;
    }

    /// Returns whether the Machine Check Architecture is supported.
    bool hasMCA() const {
        return (m_00000001_edx&(1<<14))!=0;
    }

    /// Returns whether the processor supports conditional move instructions.
    bool hasCMOV() const {
        return (m_00000001_edx&(1<<15))!=0;
    }

    /// Returns whether the processor supports the Page Attribute Table.
    bool hasPAT() const {
        return (m_00000001_edx&(1<<16))!=0;
    }

    /// Returns whether the processor allows the Page Size Extension to address
    /// physical memory beyond 4 GiB.
    bool hasPSE36() const {
        return (m_00000001_edx&(1<<17))!=0;
    }

    /// Returns whether the processor has a 96-bit serial number (reserved on
    /// AMD).
    bool hasPSN() const {
        return (m_00000001_edx&(1<<18))!=0;
    }

    /// Returns whether the CLFLUSH instruction is supported.
    bool hasCLFSH() const {
        return (m_00000001_edx&(1<<19))!=0;
    }

    // Bit 20 is reserved (on Intel and AMD).

    /// Returns whether the processor is able to write a history of branch
    /// addresses into a Debug Store (reserved on AMD).
    bool hasDS() const {
        return (m_00000001_edx&(1<<21))!=0;
    }

    /// Returns whether the processor implements temperature monitoring and
    /// performance modulation under software control (reserved on AMD).
    bool hasACPI() const {
        return (m_00000001_edx&(1<<22))!=0;
    }

    /// Returns whether the processor supports the MMX instruction set.
    bool hasMMX() const {
        return (m_00000001_edx&(1<<23))!=0;
    }

    /// Returns whether the fast floating-point save and restore instructions
    /// (FXSAVE / FXRSTOR) are supported.
    bool hasFXSR() const {
        return (m_00000001_edx&(1<<24))!=0;
    }

    /// Returns whether the Streaming SIMD Extension instruction set is
    /// supported.
    bool hasSSE() const {
        return (m_00000001_edx&(1<<25))!=0;
    }

    /// Returns whether the Streaming SIMD Extension 2 instruction set is
    /// supported.
    bool hasSSE2() const {
        return (m_00000001_edx&(1<<26))!=0;
    }

    /// Returns whether the processor supports Self-Snooping of its cache to
    /// manage conflicting memory types (reserved on AMD).
    bool hasSS() const {
        return (m_00000001_edx&(1<<27))!=0;
    }

    /// Returns whether Multi-Threading capability is present, allowing the
    /// processor to operate as multiple logical units (because there either is
    /// Hyper-Threading Technology or more than one core per processor available).
    bool hasHTT() const {
        return (m_00000001_edx&(1<<28))!=0;
    }

    /// Returns whether the processor implements the Thermal Monitor automatic
    /// thermal control circuit (reserved on AMD).
    bool hasTM() const {
        return (m_00000001_edx&(1<<29))!=0;
    }

    /// Returns whether the processor has capabilities of the Intel Itanium
    /// processor family and is currently operating in IA32 emulation mode
    /// (reserved on AMD).
    bool hasIA64() const {
        return (m_00000001_edx&(1<<30))!=0;
    }

    /// Returns whether the processor supports returning from stop-clock state
    /// to handle interrupts (reserved on AMD).
    bool hasPBE() const {
        return (m_00000001_edx&(1<<31))!=0;
    }

    /* Features returned in the ECX register */

    /// Returns whether the Streaming SIMD Extension 3 instruction set is
    /// supported.
    bool hasSSE3() const {
        return (m_00000001_ecx&(1<<0))!=0;
    }

    // Bit 1 is reserved (on Intel and AMD).

    /// Returns whether the processor has the ability to write a history of the
    /// 64-bit branch to and from addresses into a memory buffer (reserved on AMD).
    bool hasDTES64() const {
        return (m_00000001_ecx&(1<<2))!=0;
    }

    /// Returns whether the processor supports the MONITOR and MWAIT
    /// instructions.
    bool hasMONITOR() const {
        return (m_00000001_ecx&(1<<3))!=0;
    }

    /// Returns whether the processor supports extensions to the Debug Store
    /// to allow for branch message storage qualified by CPL (reserved on AMD).
    bool hasDSCPL() const {
        return (m_00000001_ecx&(1<<4))!=0;
    }

    /// Returns whether the processor supports Intel Virtualization Technology
    /// (reserved on AMD).
    bool hasVMX() const {
        return (m_00000001_ecx&(1<<5))!=0;
    }

    /// Returns whether the processor supports Intel Trusted Execution
    /// Technology (reserved on AMD).
    bool hasSMX() const {
        return (m_00000001_ecx&(1<<6))!=0;
    }

    /// Returns whether the processor implements second generation Intel
    /// SpeedStep Technology (reserved on AMD).
    bool hasEST() const {
        return (m_00000001_ecx&(1<<7))!=0;
    }

    /// Returns whether the processor implements the Thermal Monitor 2
    /// automatic thermal control circuit (reserved on AMD).
    bool hasTM2() const {
        return (m_00000001_ecx&(1<<8))!=0;
    }

    /// Returns whether the processor supports the Supplemental Streaming SIMD
    /// Extension 3 instructions.
    bool hasSSSE3() const {
        return (m_00000001_ecx&(1<<9))!=0;
    }

    /// Returns whether the L1 data cache mode can be set to either adaptive
    /// mode or shared mode by the BIOS (reserved on AMD).
    bool hasCID() const {
        return (m_00000001_ecx&(1<<10))!=0;
    }

    // Bits 11 and 12 are reserved (on Intel and AMD).

    /// Returns whether the CMPXCHG16B instruction is supported.
    bool hasCX16() const {
        return (m_00000001_ecx&(1<<13))!=0;
    }

    /// Returns whether the processor supports to disable sending Task Priority
    /// messages (reserved on AMD).
    bool hasTPR() const {
        return (m_00000001_ecx&(1<<14))!=0;
    }

    /// Returns whether the processor has performance monitoring and debug
    /// capabilities (reserved on AMD).
    bool hasPDCM() const {
        return (m_00000001_ecx&(1<<15))!=0;
    }

    // Bits 16 and 17 are reserved (on Intel and AMD).

    /// Returns whether the processor supports Direct Cache Access to prefetch
    /// data from a memory mapped device (reserved on AMD).
    bool hasDCA() const {
        return (m_00000001_ecx&(1<<18))!=0;
    }

    /// Returns whether the processor supports the Streaming SIMD Extensions 4.1
    /// instructions.
    bool hasSSE41() const {
        return (m_00000001_ecx&(1<<19))!=0;
    }

    /// Returns whether the processor supports the Streaming SIMD Extensions 4.2
    /// instructions (reserved on AMD).
    bool hasSSE42() const {
        return (m_00000001_ecx&(1<<20))!=0;
    }

    /// Returns whether the processor supports the x2APIC feature (reserved on AMD).
    bool hasX2APIC() const {
        return (m_00000001_ecx&(1<<21))!=0;
    }

    /// Returns whether the processor supports the MOVBE instruction (reserved
    /// on AMD).
    bool hasMOVBE() const {
        return (m_00000001_ecx&(1<<22))!=0;
    }

    /// Returns whether the processor supports the POPCNT instruction.
    bool hasPOPCNT() const {
        return (m_00000001_ecx&(1<<23))!=0;
    }

    // Bits 24 and 25 are reserved (on Intel and AMD).

    /// Returns whether the processor supports the XSAVE / XRSTOR extended
    /// states feature (reserved on AMD).
    bool hasXSAVE() const {
        return (m_00000001_ecx&(1<<26))!=0;
    }

    /// Returns whether the processor was enabled by the OS to support extended
    /// state management using XSAVE / XRSTOR (reserved on AMD).
    bool hasOSXSAVE() const {
        return (m_00000001_ecx&(1<<27))!=0;
    }

    // Bits 28 to 31 are reserved (on Intel and AMD).

    //@}

    /**
     * \name Features reported by the extended flags
     */
    //@{

    /* Features returned in the EDX register */

    // Bits 0 to 9 are reserved on Intel but have the same meanings as in the
    // standard flags on AMD.

    // Bit 10 is reserved (on Intel and AMD).

    /// Returns whether the processor supports the SYSCALL and SYSRET
    /// instructions.
    bool hasSYSCALL() const {
        return (m_80000001_edx&(1<<11))!=0;
    }

    // Bits 12 to 19 are reserved on Intel but have the same meanings as in the
    // standard flags on AMD.

    /// Returns whether the processor supports the XD Bit to support no-execute
    /// page protection in PAE.
    bool hasXDB() const {
        return (m_80000001_edx&(1<<20))!=0;
    }

    // Bit 21 is reserved (on Intel and AMD).

    /// Returns whether the processor supports AMD extensions to MMX
    /// instructions (reserved on Intel).
    bool hasMMXExt() const {
        return (m_80000001_edx&(1<<22))!=0;
    }

    // Bits 23 and 24 are reserved on Intel but have the same meanings as in
    // the standard flags on AMD.

    /// Returns whether the processor implements FXSAVE and FXRSTOR instruction
    /// optimizations (reserved on Intel).
    bool hasFFXSR() const {
        return (m_80000001_edx&(1<<25))!=0;
    }

    // Bit 26 is reserved (on Intel and AMD).

    /// Returns whether the processor supports the RDTSCP instruction (reserved
    /// on Intel).
    bool hasRDTSCP() const {
        return (m_80000001_edx&(1<<27))!=0;
    }

    // Bit 28 is reserved (on Intel and AMD).

    /// Returns whether the processor supports 64-bit extensions to the IA-32
    /// (slightly different meaning on AMD).
    bool hasIntel64() const {
        return (m_80000001_edx&(1<<29))!=0;
    }

    /// Returns whether the processor supports Long Mode to access 64-bit
    /// instructions and registers, running 32-bit and 16-bit programs in
    /// a compatibility sub-mode (slightly different meaning on Intel).
    bool hasAMD64() const {
        return (m_80000001_edx&(1<<29))!=0;
    }

    /// Returns whether the processor supports AMD extensions to 3DNow!
    /// instructions (reserved on Intel).
    bool has3DNowExt() const {
        return (m_80000001_edx&(1<<30))!=0;
    }

    /// Returns whether the 3DNow! instruction set is supported (reserved on
    /// Intel).
    bool has3DNow() const {
        return (m_80000001_edx&(1<<31))!=0;
    }

    /* Features returned in the ECX register */

    /// Returns whether the LAHF / SAHF instructions are supported in 64-bit
    /// sub-mode.
    bool hasLAHF() const {
        return (m_80000001_ecx&(1<<0))!=0;
    }

    /// Returns whether the processor's legacy mode is to do multi-processing,
    /// i.e. if multiple cores are present (reserved on Intel).
    bool hasCmpLegacy() const {
        return (m_80000001_ecx&(1<<1))!=0;
    }

    /// Returns whether the processor supports Secure Virtual Machine feature
    /// (reserved on Intel).
    bool hasSVM() const {
        return (m_80000001_ecx&(1<<2))!=0;
    }

    /// Returns whether the extended APIC register space is present (reserved on
    /// Intel).
    bool hasExtAPICSpace() const {
        return (m_80000001_ecx&(1<<3))!=0;
    }

    /// Returns whether LOCK MOV CR0 means MOV CR8 (reserved on Intel).
    bool hasAltMovCr8() const {
        return (m_80000001_ecx&(1<<4))!=0;
    }

    /// Returns whether advanced bit manipulation via the LZCNT instruction is
    /// supported (reserved on Intel).
    bool hasABM() const {
        return (m_80000001_ecx&(1<<5))!=0;
    }

    /// Returns whether SSE4A instructions are supported (reserved on Intel).
    bool hasSSE4A() const {
        return (m_80000001_ecx&(1<<6))!=0;
    }

    /// Returns whether misaligned access for SSE instructions is supported
    /// (reserved on Intel).
    bool hasMisaligedSSE() const {
        return (m_80000001_ecx&(1<<7))!=0;
    }

    /// Returns whether the PREFETCH and PREFETCHW instructions are supported
    /// (reserved on Intel).
    bool has3DNowPrefetch() const {
        return (m_80000001_ecx&(1<<8))!=0;
    }

    /// Returns whether the OS visible work-around information is supported
    /// (reserved on Intel).
    bool hasOSVW() const {
        return (m_80000001_ecx&(1<<9))!=0;
    }

    /// Returns whether instruction based sampling is supported (reserved on
    /// Intel).
    bool hasIBS() const {
        return (m_80000001_ecx&(1<<10))!=0;
    }

    /// Returns whether SSE5 instructions are supported (reserved on Intel).
    bool hasSSE5() const {
        return (m_80000001_ecx&(1<<11))!=0;
    }

    /// Returns whether SKINIT and STGI are supported (reserved on Intel).
    bool hasSKINIT() const {
        return (m_80000001_ecx&(1<<12))!=0;
    }

    /// Returns whether the watchdog timer is supported (reserved on Intel).
    bool hasWDT() const {
        return (m_80000001_ecx&(1<<13))!=0;
    }

    // Bits 14 to 31 are reserved (on Intel and AMD).

    //@}

  private:

    /// Make the constructor private so the class cannot be derived from and
    /// thus complete the Singleton design pattern.
    CPUInfo();

    mutable char m_vendor[3*4+1]; ///< Stores the vendor string incl. trailing zero.

    unsigned int m_00000001_ebx; ///< CPUID standard miscellaneous flags.
    unsigned int m_00000004_eax; ///< CPUID standard cache parameters.
    unsigned int m_80000008_ecx; ///< CPUID extended address sizes.

    unsigned int m_00000001_edx; ///< CPUID standard feature flags, part 1.
    unsigned int m_00000001_ecx; ///< CPUID standard feature flags, part 2.

    unsigned int m_80000001_edx; ///< CPUID extended feature flags, part 1.
    unsigned int m_80000001_ecx; ///< CPUID extended feature flags, part 2.
};

/// For convenience, offer a predefined instance of the CPUInfo class.
extern CPUInfo& CPU;

} // namespace system

} // namespace gale

#endif // CPUINFO_H
