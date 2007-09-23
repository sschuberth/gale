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

// TODO: Add Linux implementation.
#ifdef _WIN32

RenderWindow::RenderWindow(int client_width,int client_height,AttributeListi const& attribs,LPCTSTR title):
  m_timeout(0)
{
    // Activate the minimal render surface to get a context for OpenGL extension
    // initialization.
    G_ASSERT_CALL(RenderSurface::setCurrentContext());

    if (GLEX_WGL_ARB_pixel_format!=GL_TRUE) {
        // Initialize an OpenGL extension for more sophisticated selection of a
        // pixel format (see http://opengl.org/registry/specs/ARB/wgl_pixel_format.txt).
        G_ASSERT_CALL(GLEX_WGL_ARB_pixel_format_init());
    }

    if (GLEX_ARB_color_buffer_float!=GL_TRUE) {
        // Initialize an OpenGL extension for support of floating-point RGBA
        // pixel formats (see http://opengl.org/registry/specs/ARB/color_buffer_float.txt).
        G_ASSERT_CALL(GLEX_ARB_color_buffer_float_init());
    }

    RECT rect={0,0,client_width,client_height};
    G_ASSERT_CALL(AdjustWindowRect(&rect,WS_OVERLAPPEDWINDOW,FALSE));

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
    assert(m_window!=NULL);

    m_handle.device=GetWindowDC(m_window);
    assert(m_handle.device!=NULL);

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
    G_ASSERT_CALL(wglChoosePixelFormatARB(m_handle.device,attrs,NULL,1,&format,&count));
    assert(count>0);

    G_ASSERT_CALL(SetPixelFormat(m_handle.device,format,NULL));

    // Create and activate a rendering context.
    m_handle.render=wglCreateContext(m_handle.device);
    assert(m_handle.render!=NULL);

    G_ASSERT_CALL(setCurrentContext());
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
    // If there is a timeout set ...
    if (m_timeout>0) {
        double elapsed;
        m_timer.getElapsedSeconds(elapsed);
        // ... and the timeout elapsed, call the event handler.
        if (elapsed>=m_timeout) {
            onTimeout();
            m_timer.reset();
        }
    }

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
            G_ASSERT_OPENGL

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

#endif // _WIN32

} // namespace wrapgl

} // namespace gale
