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

#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

/**
 * \file
 * Render window implementation
 */

#include "gale/system/attributelist.h"
#include "gale/system/rendercontext.h"

#include "GLEX_WGL_ARB_pixel_format.h"
#include "GLEX_ARB_color_buffer_float.h"

namespace gale {

namespace wrapgl {

/**
 * This class creates a window to render to using OpenGL. It makes use of the
 * WGL_ARB_pixel_format and WGL_ARB_pixel_format_float extensions to specify the
 * desired pixel format.
 */
class RenderWindow:private system::RenderContext
{
  public:

    /// Creates a window with a client size of the given \a width and \a height
    /// to render to. The properties of the pixel format to use are determined
    /// by \a attribs, and \a title specifies the caption. The window will be
    /// hidden initially.
    RenderWindow(int client_width,int client_height,system::AttributeListi const& attribs,LPCTSTR title);

    /// Returns the handle to the render window.
    HWND getWindowHandle() const {
        return m_wnd;
    }

    /// Returns the handle to the device context associated with this render
    /// window.
    HDC getDeviceHandle() const {
        return m_dc;
    }

    /// Returns the handle to the render context associated with this render
    /// window.
    HGLRC getRenderHandle() const {
        return m_rc;
    }

    /// Starts the event processing and does not return until the window gets
    /// closed. Event handlers are called accordingly.
    void processEvents();

    /// Event handler that gets called when there are no messages to process.
    /// When it returns \c true a repaint will be triggered, on \c false the
    /// thread's remaining time slice will be relinquished.
    virtual bool onIdle() {
        return false;
    }

    /// Event handler that gets called after a window has changed its size.
    virtual void onSize() {
    }

    /// Event handler that gets called when a window portion should be painted.
    virtual void onPaint() {
    }

    /// Event handler that gets called when a window should terminate.
    virtual void onClose() {
    }

  protected:

    /// Handles window messages and forwards them to the event handlers.
    static LRESULT CALLBACK WindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

  private:

    HWND m_wnd; ///< Handle to the render window.
    HDC m_dc;   ///< Handle to the device context.
    HGLRC m_rc; ///< Handle to the render context.
};

} // namespace wrapgl

} // namespace gale

#endif // RENDERWINDOW_H
