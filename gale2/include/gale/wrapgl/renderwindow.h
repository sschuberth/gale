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

#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

/**
 * \file
 * A render window implementation
 */

#include "../system/attributelist.h"
#include "../system/rendersurface.h"
#include "../system/timer.h"

namespace gale {

namespace wrapgl {

/**
 * This class creates an on-screen window with a system-provided frame buffer to
 * render to using OpenGL. It makes use of the WGL_ARB_pixel_format and
 * WGL_ARB_pixel_format_float extensions to specify the desired pixel format.
 */

// TODO: Add Linux implementation.
#ifdef G_OS_WINDOWS

#include "GLEX_WGL_ARB_pixel_format.h"
#include "GLEX_ARB_color_buffer_float.h"

class RenderWindow:public system::RenderSurface
{
  public:

    /// Creates a window with a client size of the given \a width and \a height
    /// to render to. The properties of the pixel format to use are determined
    /// by \a attr_pixel, and \a title specifies the caption. The window will be
    /// hidden initially, but it will become the current rendering context.
    RenderWindow(int width,int height,system::AttributeListi const& attr_pixel,LPCTSTR title);

    /// Returns the handle to the window associated with this render surface.
    WindowHandle const& getWindowHandle() const {
        return m_window;
    }

    /// Returns the handle to the context associated with this render surface.
    ContextHandle const& getContextHandle() const {
        return m_handle;
    }

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
        InvalidateRect(getWindowHandle(),NULL,FALSE);
        UpdateWindow(getWindowHandle());
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
    virtual void onResize(int width,int height) {}

    /// Event handler that gets called when a window portion should be painted.
    virtual void onPaint() {}

    /// Event handler that gets called when a window should terminate.
    virtual void onClose() {}

  protected:

    /// Handles window messages and forwards them to the event handlers.
    LRESULT handleMessage(UINT uMsg,WPARAM wParam,LPARAM lParam);

    double m_timeout;      ///< Timeout event value in seconds.
    system::Timer m_timer; ///< Timer to trigger the timeout event.

  private:

    WindowHandle m_window;  ///< Handle to the render window.
    ContextHandle m_handle; ///< Handle to the render context.
};

#endif // G_OS_WINDOWS

} // namespace wrapgl

} // namespace gale

#endif // RENDERWINDOW_H
