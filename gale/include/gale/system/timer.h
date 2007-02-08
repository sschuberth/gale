/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at http://developer.berlios.de/projects/gale/
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

#ifndef TIMER_H
#define TIMER_H

/**
 * \file
 * Code for timing and benchmarking
 */

#include "../global/platform.h"

namespace gale {

namespace system {

/**
 * This is a simple timing class with pause / resume capability.
 */
class Timer
{
  public:

    /// Suspends the execution of the current thread for the given amount of
    /// milliseconds.
    static void sleep(unsigned int milliseconds) {
#ifdef _WIN32
        Sleep(milliseconds);
#else
        usleep(milliseconds*1000);
#endif
    }

    /// The default constructor starts the timing, so there no need to call
    /// start() manually. On Windows, this also locks the current thread to the
    /// first CPU for compatibility.
    Timer() {
#ifdef _WIN32
        if (s_instances==0) {
            // Work around a bug in Windows on machines with multiple CPUs that
            // do not have a hotfix (http://support.microsoft.com/?id=896256)
            // applied.
            s_mask=SetThreadAffinityMask(GetCurrentThread(),1);
        }
        ++s_instances;
#endif
        start();
    }

#ifdef _WIN32
    /// On Windows, the last destructor of any Timer instance unlocks the
    /// current from running on the first CPU only.
    ~Timer() {
        --s_instances;
        if (s_instances==0) {
            // We do not care if getting the affinity mask failed previously and
            // it thus may be zero.
            SetThreadAffinityMask(GetCurrentThread(),s_mask);
        }
    }
#endif

    /// Starts the timing from zero.
    bool start() {
        m_offset=0;
        return resume();
    }

    /// Stops the timing and returns the elapsed amount of seconds. Timing may
    /// be continued by calling resume().
    bool stop(double& seconds) {
#ifdef _WIN32
        bool result=(QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_stop))!=FALSE);

        // On modern CPUs with power-saving capabilities the frequency may vary,
        // so get it each time here.
        long long frequency;
        result=result && (QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&frequency))!=FALSE);

        // Convert the counter difference to the number of seconds elapsed.
        m_offset+=m_stop-m_start;
        seconds=static_cast<double>(m_offset)/frequency;
#else
        m_stop=times(NULL);
        bool result=(m_stop!=-1);

        // On modern CPUs with power-saving capabilities the clock ticks may
        // vary, so get it each time here.
        long ticks=sysconf(_SC_CLK_TCK);
        result=result && (ticks!=-1);

        m_offset+=m_stop-m_start;
        seconds=static_cast<double>(m_offset)/ticks;
#endif

        return result;
    }

    /// Resumes the timing without resetting the timer.
    bool resume() {
#ifdef _WIN32
        return QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_start))!=FALSE;
#else
        m_start=times(NULL);
        return m_start!=-1;
#endif
    }

  private:

    /// \var m_offset
    /// Stores the total time since the last start(), needed for resume().

    /// \var m_start
    /// Stores the time when start() was called the last time.

    /// \var m_stop
    /// Stores the time when stop() was called the last time.

#ifdef _WIN32
    /// This is a reference counter for the instances of this class.
    static unsigned int s_instances;
    /// Needed restore the affinity mask after the last instance's destruction.
    static DWORD_PTR s_mask;

    long long m_offset,m_start,m_stop;
#else
    clock_t m_offset,m_start,m_stop;
#endif
};

} // namespace system

} // namespace gale

#endif // TIMER_H
