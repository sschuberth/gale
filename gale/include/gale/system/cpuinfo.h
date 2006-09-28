/*
 * References:
 * - http://www.intel.com/design/xeon/applnots/241618.htm
 * - http://www.amd.com/us-en/assets/content_type/white_papers_and_tech_docs/20734.pdf
 *
 */

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
 * capabilities.
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
    /// Gets the maximum function input value supported.
    unsigned getMaxCPUIDFunc() const;
    /// Gets the maximum extended function input value supported.
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

    // Check for CPUID-reported features.
    bool hasTSC() const { return (m_feat_flags_edx&(1<<4))!=0; }
    bool hasMSR() const { return (m_feat_flags_edx&(1<<5))!=0; }
    bool hasCX8() const { return (m_feat_flags_edx&(1<<8))!=0; }
    bool hasCMOV() const { return (m_feat_flags_edx&(1<<15))!=0; }
    bool hasPSN() const { return (m_feat_flags_edx&(1<<18))!=0; }
    bool hasMMX() const { return (m_feat_flags_edx&(1<<23))!=0; }
    bool hasSSE() const { return (m_feat_flags_edx&(1<<25))!=0; }
    bool hasSSE2() const { return (m_feat_flags_edx&(1<<26))!=0; }
    bool hasHTT() const { return (m_feat_flags_edx&(1<<28))!=0; }

    bool hasSSE3() const { return (m_feat_flags_ecx&(1<<0))!=0; }
    bool hasEIST() const { return (m_feat_flags_ecx&(1<<7))!=0; }

    bool has3DNow() const { return (m_ext_feat_flags_edx&(1<<31))!=0; }
    bool has3DNowExt() const { return (m_ext_feat_flags_edx&(1<<30))!=0; }
    bool hasMMXExt() const { return (m_ext_feat_flags_edx&(1<<22))!=0; }

  private:
    char m_vendor[3*4+1];
    unsigned m_feat_flags_edx,m_feat_flags_ecx,m_ext_feat_flags_edx;

    CPUInfo();
};

extern CPUInfo& CPU;

} // namespace system

} // namespace gale

#endif // CPUINFO_H
