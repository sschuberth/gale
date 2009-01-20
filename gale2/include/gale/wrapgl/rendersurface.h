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

#ifndef RENDERSURFACE_H
#define RENDERSURFACE_H

/**
 * \file
 * A render surface abstraction
 */

#include "../global/platform.h"

namespace gale {

namespace wrapgl {

/**
 * This class serves as a base for everything that requires a render surface,
 * e.g. on-screen windows or off-screen buffers to render to using OpenGL. It
 * creates a dummy render surface that is used to initialize OpenGL extension
 * to create more powerful render surfaces later.
 */

// TODO: Add Linux implementation.
#ifdef G_OS_WINDOWS

class RenderSurface
{
  public:

    /// Type definition for a window handle the render surface belongs to.
    typedef HWND WindowHandle;

    /// A context handle uniquely identifies a render surface.
    struct ContextHandle {
        /// Constructor to simplify handle initialization.
        ContextHandle(HDC device=NULL,HGLRC render=NULL)
        :   device(device),render(render) {}

        HDC device;   ///< Handle to the device context.
        HGLRC render; ///< Handle to the render context.
    };

    /// Simple structure to hold the dimensions of a render surface.
    struct Dimensions {
        /// Constructor to simplify dimensions initialization.
        Dimensions(int width=0,int height=0)
        :   width(width),height(height) {}

        int width;  ///< Width of the render surface.
        int height; ///< Height of the render surface.
    };

    /// Returns the active context for the current thread.
    static ContextHandle getCurrentContext() {
        return ContextHandle(wglGetCurrentDC(),wglGetCurrentContext());
    }

    /// Sets the active context for the current thread.
    static bool setCurrentContext(ContextHandle const& handle) {
        return wglMakeCurrent(handle.device,handle.render)!=FALSE;
    }

    /// Creates a minimal render surface with a hidden window without changing
    /// the current rendering context.
    RenderSurface();

    /// Frees all resources allocated by the render surface.
    ~RenderSurface();

    /// Returns the handle to the window associated with this render surface.
    virtual WindowHandle const& windowHandle() const {
        return s_window;
    }

    /// Returns the handle to the context associated with this render surface.
    virtual ContextHandle const& contextHandle() const {
        return s_handle;
    }

    /// Sets this to be the active context for the current thread.
    bool setCurrentContext() {
        return setCurrentContext(contextHandle());
    }

    /// Returns the width and height of this render surface.
    Dimensions dimensions() const {
        RECT rect;
        G_ASSERT_CALL(GetClientRect(windowHandle(),&rect));
        return Dimensions(rect.right,rect.bottom);
    }

  protected:

    /// Destroys the render context, i.e. frees all resources except the window
    /// class, so the method can be used by derived classes.
    void destroy();

    /// Handles window messages and forwards them to the event handlers.
    virtual LRESULT handleMessage(UINT uMsg,WPARAM wParam,LPARAM lParam);

    static int s_instances; ///< Counter for the number of instances.
    static ATOM s_atom;     ///< Identifier for the registered window class.

  private:

    /// Forwards window messages to the window specific message handler.
    static LRESULT CALLBACK WindowProc(WindowHandle hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

    static WindowHandle s_window;  ///< Handle to the window owning the context.
    static ContextHandle s_handle; ///< Handle to the context.
};

#endif // G_OS_WINDOWS

} // namespace wrapgl

} // namespace gale

#endif // RENDERSURFACE_H