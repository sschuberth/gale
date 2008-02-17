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
 * Simple timing class with resume capability.
 */
class Timer
{
  public:

    /// Suspends the execution of the current thread for the given amount of
    /// milliseconds.
    static void sleep(unsigned int const milliseconds) {
#ifdef G_OS_WINDOWS
        Sleep(milliseconds);
#else
        usleep(milliseconds*1000);
#endif
    }

    /// The default constructor resets the timing, so there no need to call
    /// reset() manually. On Windows, this also locks the current thread to the
    /// first CPU for compatibility.
    Timer() {
#ifdef G_OS_WINDOWS
        if (s_instances==0) {
            // Work around a bug in Windows on machines with multiple CPUs that
            // do not have a hotfix applied, see <http://support.microsoft.com/?id=896256>.
            s_mask=SetThreadAffinityMask(GetCurrentThread(),1);
        }
        ++s_instances;
#endif
        reset();
    }

#ifdef G_OS_WINDOWS
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

    /// Resets the timing to start from zero.
    bool reset() {
        m_offset=0;
        return start();
    }

    /// Sets a new start time for the timer.
    bool start() {
#ifdef G_OS_WINDOWS
        return QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_start))!=FALSE;
#else
        m_start=times(NULL);
        return m_start!=-1;
#endif
    }

    /// Returns the elapsed time in seconds since reset() was called and stops
    /// the timing. It may be resumed by calling start() again.
    bool stop(double& seconds) {
#ifdef G_OS_WINDOWS
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

    /// Returns the elapsed time in seconds since reset() was called without
    /// stopping the timing.
    bool getElapsedSeconds(double& seconds) {
        return stop(seconds) && start();
    }

  private:

    /// \var m_offset
    /// Stores the total time since the last reset.

    /// \var m_start
    /// Stores the last time when start() was called.

    /// \var m_stop
    /// Stores the last time when stop() was called.

#ifdef G_OS_WINDOWS
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
