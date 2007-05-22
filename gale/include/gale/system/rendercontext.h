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

#ifndef RENDERCONTEXT_H
#define RENDERCONTEXT_H

/**
 * \file
 * Render context abstraction
 */

#include "../global/platform.h"

namespace gale {

namespace system {

/**
 * This class serves as a base for everything that requires a render context to
 * be present, e.g. initializing OpenGL extensions.
 */
class RenderContext
{
  public:

    /**
     * This structure encapsulates the variables that identify a render context.
     */
    struct Handle {
        /// Constructor to simplify handle initialization.
        Handle(HDC device=NULL,HGLRC render=NULL):
          device(device),render(render) {}

        HDC device;   ///< Handle to the Windows device context.
        HGLRC render; ///< Handle to the Windows render context.
    };

    /// Returns the active render context for the current thread.
    static Handle getCurrent() {
        return Handle(wglGetCurrentDC(),wglGetCurrentContext());
    }

    /// Creates a minimal render context.
    RenderContext();

    /// Frees all resources allocated by the render context.
    ~RenderContext();

    /// Returns the handle for the window associated with this render context.
    virtual HWND getWindowHandle() const {
        return s_window;
    }

    /// Returns the handle for this render context.
    virtual Handle getContextHandle() const {
        return Handle(s_handle.device,s_handle.render);
    }

    /// Sets this to be the active render context for the current thread.
    bool setCurrent() {
        return wglMakeCurrent(s_handle.device,s_handle.render)!=FALSE;
    }

  protected:

    /// Destroys the render context, i.e. frees all resources except the window
    /// class, so the method can be used by derived classes.
    void destroy();

    /// Handles window messages and forwards them to the event handlers.
    virtual LRESULT handleMessage(UINT uMsg,WPARAM wParam,LPARAM lParam);

    static ATOM s_atom;     ///< Identifier for the registered window class.

  private:

    /// Forwards window messages to the window specific message handler.
    static LRESULT CALLBACK WindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

    static int s_instances; ///< Counter for the number of instances.

    static HWND s_window;   ///< Handle to the hidden window.
    static Handle s_handle; ///< Handle to the render context.
};

} // namespace system

} // namespace gale

#endif // RENDERCONTEXT_H
