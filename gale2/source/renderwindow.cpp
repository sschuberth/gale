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

#include "gale/wrapgl/renderwindow.h"

using namespace gale::global;

namespace gale {

namespace wrapgl {

// TODO: Add Linux implementation.
#ifdef G_OS_WINDOWS

RenderWindow::RenderWindow(int width,int height,AttributeListi const& pixel_attr,LPCTSTR title)
:   m_timeout(0)
{
    // Calculate the window size from the desired client area size.
    RECT rect={0,0,width,height};
    G_ASSERT_CALL(AdjustWindowRect(&rect,WS_OVERLAPPEDWINDOW,FALSE));

    width=rect.right-rect.left;
    height=rect.bottom-rect.top;

    // Create a default device context.
    G_ASSERT_CALL(create(0,width,height,title));

    // Create and activate a rendering context for OpenGL extension
    // initialization.
    m_context.render=wglCreateContext(m_context.device);
    assert(m_context.render);

    G_ASSERT_CALL(makeCurrentContext());

    AttributeListi attr;
    GLint format=0;

    // Try to initialize an OpenGL extension for more sophisticated selection of a
    // pixel format, see <http://opengl.org/registry/specs/ARB/wgl_pixel_format.txt>.
    if (GLEX_WGL_ARB_pixel_format_init()) {
        attr=pixel_attr;

        // Make sure some required attributes are specified.
        attr.insert(WGL_DRAW_TO_WINDOW_ARB,TRUE);
        attr.insert(WGL_SUPPORT_OPENGL_ARB,TRUE);
        attr.insert(WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB);
        attr.insert(WGL_DOUBLE_BUFFER_ARB,TRUE);

        // Set the device context to the first pixel format that matches the
        // specified attributes.
        UINT count;
        G_ASSERT_CALL(wglChoosePixelFormatARB(m_context.device,attr,NULL,1,&format,&count));
    }

    // Try to initialize an extension required to create an OpenGL 3.0 compatible
    // context, see <http://www.opengl.org/registry/specs/ARB/wgl_create_context.txt>.
    if (GLEX_WGL_ARB_create_context_init() || format>0) {
        // Note: The code below obviously only works for extensions that do not
        // themselves require an OpenGL context to be active when called.

        // Destroy the default context to create a more sophisticated one.
        destroy();

        // Create a device context with a possibly better pixel format.
        create(format,width,height,title);

        // Create and activate the final rendering context.
        if (GLEX_WGL_ARB_create_context) {
            attr.clear();
            attr.insert(WGL_CONTEXT_MAJOR_VERSION_ARB,3);
            m_context.render=wglCreateContextAttribsARB(m_context.device,0,attr);
        }
        else {
            m_context.render=wglCreateContext(m_context.device);
        }
        assert(m_context.render);

        G_ASSERT_CALL(makeCurrentContext());
    }
}

void RenderWindow::processEvents()
{
    MSG msg;

    for (;;) {
        // We need to use the non-blocking PeekMessage() here which causes high
        // CPU usage instead of the blocking GetMessage() because we want to be
        // able to do something during idle time.
        if (PeekMessage(&msg,windowHandle(),0,0,PM_REMOVE)) {
            if (msg.message==WM_QUIT) {
                // Do not dispatch the quit message.
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            // Idle if there are no messages to process.
            if (onIdle()) {
                repaint();
            }
            else {
                // Relinquish the rest of the time slice.
                Sleep(0);
            }
        }

        // If there is a timeout set ...
        if (m_timeout>0) {
            double elapsed;
            m_timer.elapsed(elapsed);
            // ... and the timeout elapsed, call the event handler.
            if (elapsed>=m_timeout) {
                onTimeout();
                m_timer.reset();
            }
        }
    }
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
            G_ASSERT_OPENGL

            // Due to WGL_DOUBLE_BUFFER_ARB we always should have a double-buffer.
            SwapBuffers(contextHandle().device);

            // Notify Windows that we have finished painting.
            ValidateRect(windowHandle(),NULL);

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

#endif // G_OS_WINDOWS

} // namespace wrapgl

} // namespace gale
