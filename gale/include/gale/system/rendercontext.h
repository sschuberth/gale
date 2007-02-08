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

    /// Returns a handle to the currently active rendering context.
    static HGLRC getCurrent() {
        return wglGetCurrentContext();
    }

    /// Makes this the thread's currently active rendering context.
    bool setCurrent() {
        return wglMakeCurrent(getDeviceHandle(),getRenderHandle())!=FALSE;
    }

    /// Creates a minimal render context.
    RenderContext();

    /// Frees all resources allocated by the render context.
    ~RenderContext();

    /// Returns the handle to the hidden window associated with this render
    /// context.
    virtual HWND getWindowHandle() const {
        return m_wnd;
    }

    /// Returns the handle to the device context associated with this render
    /// context.
    virtual HDC getDeviceHandle() const {
        return m_dc;
    }

    /// Returns the handle to the render context.
    virtual HGLRC getRenderHandle() const {
        return m_rc;
    }

  protected:

    /// Destroys the render context, i.e. frees all resources except the window
    /// class, so the method can be used by derived classes.
    void destroy();

    /// Identifier for the registered window class.
    static ATOM s_atom;

  private:

    /// Counter for the number of instances.
    static int s_instances;

    HWND m_wnd; ///< Handle to the hidden window.
    HDC m_dc;   ///< Handle to the device context.
    HGLRC m_rc; ///< Handle to the render context.
};

} // namespace system

} // namespace gale

#endif // RENDERCONTEXT_H
