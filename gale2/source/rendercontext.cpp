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

#include "gale/wrapgl/rendercontext.h"

using namespace gale::global;

namespace gale {

namespace wrapgl {

// TODO: Add Linux implementation.
#ifdef G_OS_WINDOWS

RenderContext::RenderContext(LPCTSTR title,int width,int height,global::AttributeListi const* const pixel_attr,int samples)
:   m_close_requested(false)
,   m_timeout(0)
{
    // Calculate the window size from the desired client area size.
    RECT rect={0,0,width,height};
    G_ASSERT_CALL(AdjustWindowRect(&rect,WS_OVERLAPPEDWINDOW,FALSE))

    width=rect.right-rect.left;
    height=rect.bottom-rect.top;

    // Create a surface with a default pixel format.
    G_ASSERT_CALL(RenderSurface::create(0,width,height,title))

    // Create and activate a rendering context for OpenGL extension
    // initialization.
    m_context.render=wglCreateContext(m_context.device);
    G_ASSERT(m_context.render)

    G_ASSERT_CALL(makeCurrent())

    // Try to initialize an OpenGL extension for more sophisticated selection of a
    // pixel format, see <http://opengl.org/registry/specs/ARB/wgl_pixel_format.txt>.
    GLEX_WGL_ARB_pixel_format_init();

    // Try to initialize an extension required to create an OpenGL 3.0 compatible
    // context, see <http://www.opengl.org/registry/specs/ARB/wgl_create_context.txt>.
    GLEX_WGL_ARB_create_context_init();

    AttributeListi attr;
    GLint format=0;

    if (GLEX_WGL_ARB_pixel_format) {
        // If additional pixel attributes were given, copy them over.
        if (pixel_attr) {
            attr=*pixel_attr;
        }

        // Make sure some required attributes are specified.
        attr.insert(WGL_DRAW_TO_WINDOW_ARB,TRUE);
        attr.insert(WGL_SUPPORT_OPENGL_ARB,TRUE);
        attr.insert(WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB);
        attr.insert(WGL_DOUBLE_BUFFER_ARB,TRUE);

        // Specify some common pixel format attributes.
        attr.insert(WGL_PIXEL_TYPE_ARB,WGL_TYPE_RGBA_ARB);
        attr.insert(WGL_COLOR_BITS_ARB,32);
        attr.insert(WGL_DEPTH_BITS_ARB,24);
        attr.insert(WGL_STENCIL_BITS_ARB,8);

        // Try to find a matching (one-based) pixel format.
        UINT count;
        wglChoosePixelFormatARB(m_context.device,attr,NULL,1,&format,&count);

        // Try to get a multi-sampled pixel format, see
        // <http://www.opengl.org/registry/specs/ARB/multisample.txt>.
        if (GLEX_ARB_multisample_init()) {
            attr.insert(WGL_SAMPLE_BUFFERS_ARB,TRUE);
            attr.insert(WGL_SAMPLES_ARB,samples);

            GLint format_multisample=0;
            if (wglChoosePixelFormatARB(m_context.device,attr,NULL,1,&format_multisample,&count)!=FALSE) {
                format=format_multisample;
            }
        }

        if (format>0) {
            // Destroy the default context and surface in order to set a custom
            // pixel format.
            destroy();
            RenderSurface::destroy();

            // Create a surface with a custom pixel format.
            G_ASSERT_CALL(RenderSurface::create(format,width,height,title))
        }
    }

    if (GLEX_WGL_ARB_create_context) {
        // Destroy the default context in order to create a more sophisticated one.
        destroy();

        // Request an OpenGL 3.0 context.
        attr.clear();
        attr.insert(WGL_CONTEXT_MAJOR_VERSION_ARB,3);
        attr.insert(WGL_CONTEXT_MINOR_VERSION_ARB,0);
        m_context.render=wglCreateContextAttribsARB(m_context.device,0,attr);
    }

    if (!m_context.render) {
        m_context.render=wglCreateContext(m_context.device);
    }
    G_ASSERT(m_context.render)

    G_ASSERT_CALL(makeCurrent())
}

void RenderContext::processEvents()
{
    MSG msg;

    while (!m_close_requested) {
        // We need to use the non-blocking PeekMessage() here which causes high
        // CPU usage instead of the blocking GetMessage() because we want to be
        // able to do something during idle time.
        if (PeekMessage(&msg,m_window,0,0,PM_REMOVE)) {
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

void RenderContext::destroy()
{
    if (m_context.render) {
        G_ASSERT_CALL(wglDeleteContext(m_context.render))
        m_context.render=NULL;
    }
}

LRESULT RenderContext::handleMessage(UINT const uMsg,WPARAM const wParam,LPARAM const lParam)
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

            // Make use of double-buffering.
            SwapBuffers(m_context.device);

            // Notify Windows that we have finished painting.
            ValidateRect(m_window,NULL);

            break;
        }

        // This is sent to a window when it should terminate.
        case WM_CLOSE: {
            onClose();

            // Delay the call to destroy() until class destruction time so other
            // OpenGL wrapper classes can free their resources in their
            // destructors before the rendering context gets deleted.
            m_close_requested=true;

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
