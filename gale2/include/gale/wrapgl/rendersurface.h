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

// TODO: Add Linux implementation.
#ifdef G_OS_WINDOWS

/**
 * This class serves as a base for everything that requires an OpenGL context,
 * e.g. on-screen windows or off-screen buffers. It creates a default context
 * that is used to initialize OpenGL extensions required to create a more
 * sophisticated context which replaces the default context.
 */
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

    /// Returns the active OpenGL context for the current thread.
    static ContextHandle getCurrentContext() {
        return ContextHandle(wglGetCurrentDC(),wglGetCurrentContext());
    }

    /// Sets the active OpenGL context for the current thread.
    static bool setCurrentContext(ContextHandle const& handle) {
        return wglMakeCurrent(handle.device,handle.render)!=FALSE;
    }

    /// Registers a common window class for all OpenGL contexts.
    RenderSurface();

    /// Unregisters the window class if it is not used anymore.
    ~RenderSurface();

    /// Creates a minimal OpenGL context with either a default or the specified
    /// \a pixel_format which is attached to a hidden window. The current OpenGL
    /// context is not changed.
    bool create(int pixel_format=0);

    /// Destroys the render surface, i.e. frees all resources except the window
    /// class, so the method can be used by derived classes.
    void destroy();

    /// Returns the handle to the window associated with this render surface.
    WindowHandle const& windowHandle() const {
        return m_window;
    }

    /// Returns the handle to the OpenGL context associated with this render
    /// surface.
    ContextHandle const& contextHandle() const {
        return m_context;
    }

    /// Makes this the active OpenGL context for the current thread.
    bool makeCurrentContext() {
        return setCurrentContext(m_context);
    }

    /// Returns the width and height of this render surface.
    Dimensions dimensions() const {
        RECT rect;
        G_ASSERT_CALL(GetClientRect(m_window,&rect));
        return Dimensions(rect.right,rect.bottom);
    }

  protected:

    /// Handles window messages and forwards them to the event handlers.
    virtual LRESULT handleMessage(UINT uMsg,WPARAM wParam,LPARAM lParam);

    static int s_instances;  ///< Counter for the number of instances.
    static ATOM s_atom;      ///< Identifier for the registered window class.

    WindowHandle m_window;   ///< Handle to the window owning the OpenGL context.
    ContextHandle m_context; ///< Handle to the OpenGL context.

  private:

    /// Forwards window messages to the window specific message handler.
    static LRESULT CALLBACK WindowProc(WindowHandle hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
};

#endif // G_OS_WINDOWS

} // namespace wrapgl

} // namespace gale

#endif // RENDERSURFACE_H
