/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at https://github.com/sschuberth/gale/.
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

#pragma once

/**
 * \file
 * A render surface implementation
 */

#include "../global/attributelist.h"
#include "../global/platform.h"

namespace gale {

namespace wrapgl {

#include "GLEX_ARB_multisample.h"

// TODO: Add Linux implementation.
#ifdef G_OS_WINDOWS

#include "GLEX_WGL_ARB_multisample.h"

#include "GLEX_WGL_ARB_pixel_format.h"
#include "GLEX_WGL_ARB_create_context.h"

/**
 * This class provides a minimal pixel surface that is suitable for OpenGL
 * rendering. It is hidden and has no dimensions initially which predestines it
 * for tasks such as initializing OpenGL extensions.
 */
class RenderSurface
{
  public:

    static int const DEFAULT_COLOR_BITS   = 24; ///< The color bits to use if not further specified.
    static int const DEFAULT_DEPTH_BITS   = 24; ///< The depth bits to use if not further specified.
    static int const DEFAULT_STENCIL_BITS = 8;  ///< The stencil bits to use if not further specified.

    /// Type definition for a window handle the render surface belongs to.
    typedef HWND WindowHandle;

    /// A context handle uniquely identifies a render surface.
    struct ContextHandle {
        /// Constructor to simplify member variable initialization.
        ContextHandle(HDC device=NULL,HGLRC render=NULL)
        :   device(device)
        ,   render(render)
        {}

        HDC device;   ///< Handle to the device context.
        HGLRC render; ///< Handle to the render context.
    };

    /// Simple structure to hold the dimensions of a render surface.
    struct Dimensions {
        /// Constructor to simplify member variable initialization.
        Dimensions(int width=0,int height=0)
        :   width(width)
        ,   height(height)
        {}

        int width;  ///< Width of the render surface.
        int height; ///< Height of the render surface.
    };

    /// Returns the active OpenGL context for the current thread.
    static ContextHandle getCurrent() {
        return ContextHandle(wglGetCurrentDC(),wglGetCurrentContext());
    }

    /// Sets the active OpenGL context for the current thread.
    static bool setCurrent(ContextHandle const& handle) {
        return wglMakeCurrent(handle.device,handle.render)!=FALSE;
    }

    /// Creates a minimal render surface and current render context with the
    /// attributes in \a context_attr. Either a default pixel format or one that
    /// matches \a pixel_attr is used. If multi-sampling is available, the
    /// specified number of \a samples is used.
    RenderSurface(
        global::AttributeListi const* const context_attr=NULL
    ,   global::AttributeListi const* const pixel_attr=NULL
    ,   int const samples=8
    );

    /// Destroys render surface.
    ~RenderSurface() {
        destroyRenderContext();
        destroyDeviceContext();
    }

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
    bool makeCurrent() {
        return setCurrent(m_context);
    }

    /// Returns the width and height of this render surface.
    Dimensions dimensions() const {
        RECT rect;
        G_ASSERT_CALL(GetClientRect(m_window,&rect))
        return Dimensions(rect.right,rect.bottom);
    }

  protected:

    /// Creates a minimal device context and associated window with either a
    /// default or the specified \a pixel_format. The window is initially hidden
    /// and has no dimensions. No rendering context is created yet to allow
    /// creating one using OpenGL extensions.
    bool createDeviceContext(int pixel_format=0);

    /// Releases the device context and destroys the associated window. This is
    /// e.g. required if another pixel format should be set for a device context
    /// as an application can only set the pixel format of a window one time.
    void destroyDeviceContext();

    /// Deletes the render context only.
    void destroyRenderContext();

    /// Handles window messages and forwards them to the event methods.
    virtual LRESULT handleMessage(UINT const uMsg,WPARAM const wParam,LPARAM const lParam);

    /// Pixel format description for the default format.
    static PIXELFORMATDESCRIPTOR const s_pfd;

    WindowHandle m_window;   ///< Handle to the window owning the OpenGL context.
    ContextHandle m_context; ///< Handle to the OpenGL context.

  private:

    /// Forwards window messages to the class instance's message handler.
    static LRESULT CALLBACK WindowProc(WindowHandle hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
};

#endif // G_OS_WINDOWS

} // namespace wrapgl

} // namespace gale
