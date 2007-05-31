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

#ifndef RENDERSURFACE_H
#define RENDERSURFACE_H

/**
 * \file
 * Render surface abstraction
 */

#include "../global/platform.h"

namespace gale {

namespace system {

/**
 * This class serves as a base for everything that requires a render surface,
 * e.g. initializing OpenGL extensions, on- and off-screen render buffers etc.
 */
class RenderSurface
{
  public:

    /// Type definition for a window handle that belongs to the render surface.
    typedef HWND WindowHandle;

    /// Type definition for a context handle that identifies a render surface.
    struct ContextHandle {
        /// Constructor to simplify handle initialization.
        ContextHandle(HDC device=NULL,HGLRC render=NULL):
          device(device),render(render) {}

        HDC device;   ///< Handle to the Windows device context.
        HGLRC render; ///< Handle to the Windows render context.
    };

    /// Simple structure to hold the dimensions of a render surface.
    struct Dimensions {
        /// Constructor to simplify dimensions initialization.
        Dimensions(int width=0,int height=0):
          width(width),height(height) {}

        int width;  ///< Width of the render surface.
        int height; ///< Height of the render surface.
    };

    /// Returns the active render context for the current thread.
    static ContextHandle getCurrentContext() {
        return ContextHandle(wglGetCurrentDC(),wglGetCurrentContext());
    }

    /// Creates a minimal render surface with a hidden window without changing
    /// the current rendering context.
    RenderSurface();

    /// Frees all resources allocated by the render surface.
    ~RenderSurface();

    /// Returns the handle to the window associated with this render surface.
    virtual WindowHandle getWindowHandle() const {
        return s_window;
    }

    /// Returns the handle to the context associated with this render surface.
    virtual ContextHandle getContextHandle() const {
        return ContextHandle(s_handle.device,s_handle.render);
    }

    /// Sets this to be the active render context for the current thread.
    bool setCurrentContext() {
        return wglMakeCurrent(s_handle.device,s_handle.render)!=FALSE;
    }

    /// Returns both the width and height of this render surface.
    virtual Dimensions getDimensions() const {
        RECT rect;
        GetClientRect(getWindowHandle(),&rect);
        return Dimensions(rect.right,rect.bottom);
    }

  protected:

    /// Destroys the render context, i.e. frees all resources except the window
    /// class, so the method can be used by derived classes.
    void destroy();

    /// Handles window messages and forwards them to the event handlers.
    virtual LRESULT handleMessage(UINT uMsg,WPARAM wParam,LPARAM lParam);

    static ATOM s_atom;     ///< Identifier for the registered window class.
    static int s_instances; ///< Counter for the number of instances.

  private:

    /// Forwards window messages to the window specific message handler.
    static LRESULT CALLBACK WindowProc(WindowHandle hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

    static WindowHandle s_window;  ///< Handle to the hidden window.
    static ContextHandle s_handle; ///< Handle to the render context.
};

} // namespace system

} // namespace gale

#endif // RENDERSURFACE_H
