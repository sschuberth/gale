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

#include "gale/wrapgl/renderwindow.h"

using namespace gale::system;

namespace gale {

namespace wrapgl {

RenderWindow::RenderWindow(int client_width,int client_height,AttributeListi const& attribs,LPCTSTR title)
{
    // Activate the minimal render surface to get a context for OpenGL extension
    // initialization.
    BOOL result=RenderSurface::setCurrentContext();
    G_ASSERT(result!=FALSE)

    if (GLEX_WGL_ARB_pixel_format!=GL_TRUE) {
        // Initialize an OpenGL extension for more sophisticated selection of a
        // pixel format (see http://www.opengl.org/registry/specs/ARB/wgl_pixel_format.txt).
        GLEX_WGL_ARB_pixel_format_init();
        G_ASSERT(GLEX_WGL_ARB_pixel_format!=GL_FALSE)
    }

    if (GLEX_ARB_color_buffer_float!=GL_TRUE) {
        // Initialize an OpenGL extension for support of floating-point RGBA
        // pixel formats (see http://www.opengl.org/registry/specs/ARB/color_buffer_float.txt).
        GLEX_ARB_color_buffer_float_init();
        G_ASSERT(GLEX_ARB_color_buffer_float!=GL_FALSE)
    }

    RECT rect={0,0,client_width,client_height};
    result=AdjustWindowRect(&rect,WS_OVERLAPPEDWINDOW,FALSE);
    G_ASSERT(result!=FALSE)

    // Create a render window and get its device context.
    m_window=CreateWindow(
    /* lpClassName  */ MAKEINTATOM(s_atom),
    /* lpWindowName */ title,
    /* dwStyle      */ WS_OVERLAPPEDWINDOW,
    /* x            */ CW_USEDEFAULT,
    /* y            */ 0,
    /* nWidth       */ rect.right-rect.left,
    /* nHeight      */ rect.bottom-rect.top,
    /* hWndParent   */ HWND_DESKTOP,
    /* hMenu        */ NULL,
    /* hInstance    */ NULL,
    /* lpParam      */ this
    );
    G_ASSERT(m_window!=NULL)

    m_handle.device=GetWindowDC(m_window);
    G_ASSERT(m_handle.device!=NULL)

    // Make sure some required attributes are specified.
    AttributeListi attrs=attribs;
    attrs.insert(WGL_DRAW_TO_WINDOW_ARB,TRUE);
    attrs.insert(WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB);
    attrs.insert(WGL_SUPPORT_OPENGL_ARB,TRUE);
    attrs.insert(WGL_DOUBLE_BUFFER_ARB,TRUE);

    // Set the device context to the first pixel format that matches the
    // specified attributes.
    GLint format;
    UINT count;
    result=wglChoosePixelFormatARB(m_handle.device,attrs,NULL,1,&format,&count);
    G_ASSERT(result!=FALSE)

    result=SetPixelFormat(m_handle.device,format,NULL);
    G_ASSERT(result!=FALSE)

    // Create and activate a rendering context.
    m_handle.render=wglCreateContext(m_handle.device);
    G_ASSERT(m_handle.render!=NULL)

    result=setCurrentContext();
    G_ASSERT(result!=FALSE)
}

void RenderWindow::processEvents()
{
    MSG msg;

    do {
        // Idle once in the beginning to initialize and then if there are no
        // messages to process.
        if (onIdle()) {
            repaint();
        } else {
            // Relinquish the rest of the time slice.
            Sleep(0);
        }

        // Dispatch all messages in the queue.
        while (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    } while (msg.message!=WM_QUIT);
}

LRESULT RenderWindow::handleMessage(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    switch (uMsg) {
        // This is sent to a window after its size has changed.
        case WM_SIZE: {
            onResize(LOWORD(lParam),HIWORD(lParam));
            break;
        }

        // This is sent to a window when a portion should be painted.
        case WM_PAINT: {
            onPaint();
            glFlush();

            // Due to WGL_DOUBLE_BUFFER_ARB we always should have a double-buffer.
            SwapBuffers(getContextHandle().device);

            // Notify Windows that we have finished painting.
            ValidateRect(getWindowHandle(),NULL);

            break;
        }

        // This is sent to a window when it should terminate.
        case WM_CLOSE: {
            onClose();
            destroy();
            break;
        }

        default: {
            // Handle more elementary messages in the base class.
            return RenderSurface::handleMessage(uMsg,wParam,lParam);
        }
    }

    return 0;
}

} // namespace wrapgl

} // namespace gale
