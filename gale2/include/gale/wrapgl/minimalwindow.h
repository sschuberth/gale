/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
 *
 * Copyright (C) 2005-2011  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
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

#ifndef MINIMALWINDOW_H
#define MINIMALWINDOW_H

/**
 * \file
 * An OpenGL window implementation
 */

#include "../system/timer.h"

#include "rendersurface.h"

namespace gale {

namespace wrapgl {

// TODO: Add Linux implementation.
#ifdef G_OS_WINDOWS

/**
 * This class creates minimal window suitable for OpenGL rendering.
 */
class MinimalWindow:public RenderSurface
{
  public:

    /// Creates a window with the specified \a title, a client size of
    /// the given \a width and \a height, an OpenGL context with the attributes
    /// in \a context_attr attached to it and a pixel format that matches
    /// \a pixel_attr. The window will be hidden initially, but it will become
    /// the current render context.
    MinimalWindow(
        LPCTSTR title
    ,   int width
    ,   int height
    ,   global::AttributeListi const* const context_attr=NULL
    ,   global::AttributeListi const* const pixel_attr=NULL
    ,   int const samples=8
    );

    /// Returns the currently set timeout value in seconds.
    double getTimeout() const {
        return m_timeout;
    }

    /// Sets the current \a timeout value in seconds. Any non-positive value
    /// (which includes 0) disables the timeout handler.
    void setTimeout(double const timeout) {
        m_timeout=timeout;
    }

    /// Starts the event processing and does not return until the window gets
    /// closed. Event handlers are called accordingly.
    void processEvents();

    /// Invalidates the whole window and requests to repaint its contents.
    void repaint() {
        InvalidateRect(m_window,NULL,FALSE);
        UpdateWindow(m_window);
    }

    /// Event handler that gets called when there are no messages to process.
    /// When it returns \c true a repaint will be triggered, on \c false the
    /// thread's remaining time slice will be relinquished.
    virtual bool onIdle() {
        return false;
    }

    /// Event handler that gets called when the timeout value has been reached.
    virtual void onTimeout() {}

    /// Event handler that gets called after a window has changed its size.
    virtual void onResize(int width,int height) {
        // When a GL context is first attached to a window, the viewport width and
        // height are set to the dimensions of that window, which is 0, 0 for the
        // render surface's dummy window, so we need to adjust the viewport.
        glViewport(0,0,width,height);
        repaint();
    }

    /// Event handler that gets called when a window portion should be painted.
    virtual void onPaint() {}

    /// Event handler that gets called when a window should terminate.
    virtual void onClose() {}

  protected:

    /// Handles window messages and forwards them to the event methods.
    LRESULT handleMessage(UINT const uMsg,WPARAM const wParam,LPARAM const lParam);

    bool m_close_requested; ///< Indicates whether the window should be closed.

    double m_timeout;      ///< Timeout event value in seconds.
    system::Timer m_timer; ///< Timer to trigger the timeout event.
};

#endif // G_OS_WINDOWS

} // namespace wrapgl

} // namespace gale

#endif // MINIMALWINDOW_H
