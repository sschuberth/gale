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

#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

/**
 * \file
 * A render window implementation
 */

#include "../global/attributelist.h"
#include "../system/timer.h"

#include "rendersurface.h"

namespace gale {

namespace wrapgl {

#include "GLEX_ARB_multisample.h"

// TODO: Add Linux implementation.
#ifdef G_OS_WINDOWS

#include "GLEX_WGL_ARB_pixel_format.h"
#include "GLEX_WGL_ARB_create_context.h"

/**
 * This class creates an on-screen window for OpenGL rendering.
 */
class RenderWindow:public RenderSurface
{
  public:

    /// Creates a window with the specified \a title, a client size of the given
    /// \a width and \a height, and a pixel format determined by \a pixel_attr.
    /// The window will be hidden initially, but it will become the current
    /// rendering context.
    RenderWindow(LPCTSTR title,int width,int height,global::AttributeListi const* pixel_attr=NULL);

    /// Returns the currently set timeout value in seconds.
    double getTimeout() const {
        return m_timeout;
    }

    /// Sets the current \a timeout value in seconds. Any non-positive value
    /// (which includes 0) disables the timeout handler.
    void setTimeout(double timeout) {
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
        UNREFERENCED_PARAMETER(width);
        UNREFERENCED_PARAMETER(height);
    }

    /// Event handler that gets called when a window portion should be painted.
    virtual void onPaint() {}

    /// Event handler that gets called when a window should terminate.
    virtual void onClose() {}

  protected:

    /// Handles window messages and forwards them to the event handlers.
    LRESULT handleMessage(UINT uMsg,WPARAM wParam,LPARAM lParam);

    double m_timeout;      ///< Timeout event value in seconds.
    system::Timer m_timer; ///< Timer to trigger the timeout event.
};

#endif // G_OS_WINDOWS

} // namespace wrapgl

} // namespace gale

#endif // RENDERWINDOW_H
