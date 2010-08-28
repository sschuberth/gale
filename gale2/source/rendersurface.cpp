/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
 *
 * Copyright (C) 2005-2010  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
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

#include "gale/wrapgl/rendersurface.h"

using namespace gale::global;

namespace gale {

namespace wrapgl {

// TODO: Add Linux implementation.
#ifdef G_OS_WINDOWS

PIXELFORMATDESCRIPTOR const RenderSurface::s_pfd={
    sizeof(s_pfd)            // nSize
,   1                        // nVersion
,   PFD_DRAW_TO_WINDOW       // dwFlags
|   PFD_SUPPORT_OPENGL
|   PFD_GENERIC_ACCELERATED
|   PFD_DOUBLEBUFFER
|   PFD_SUPPORT_COMPOSITION
,   PFD_TYPE_RGBA            // iPixelType
,   DEFAULT_COLOR_BITS       // cColorBits
,   0                        // cRedBits
,   0                        // cRedShift
,   0                        // cGreenBits
,   0                        // cGreenShift
,   0                        // cBlueBits
,   0                        // cBlueShift
,   0                        // cAlphaBits
,   0                        // cAlphaShift
,   0                        // cAccumBits
,   0                        // cAccumRedBits
,   0                        // cAccumGreenBits
,   0                        // cAccumBlueBits
,   0                        // cAccumAlphaBits
,   DEFAULT_DEPTH_BITS       // cDepthBits
,   DEFAULT_STENCIL_BITS     // cStencilBits
,   0                        // cAuxBuffers
,   0                        // iLayerType
,   0                        // bReserved
,   0                        // dwLayerMask
,   0                        // dwVisibleMask
,   0                        // dwDamageMask
};

RenderSurface::RenderSurface(
    global::AttributeListi const* const context_attr
,   global::AttributeListi const* const pixel_attr
,   int const samples
)
{
    // Create a surface with a default pixel format.
    G_ASSERT_CALL(createDeviceContext())

    // Create and activate a rendering context for OpenGL extension initialization.
    m_context.render=wglCreateContext(m_context.device);
    G_ASSERT(m_context.render)

    G_ASSERT_CALL(makeCurrent())

    // Try to initialize an extension for more sophisticated selection of a
    // pixel format, see <http://opengl.org/registry/specs/ARB/wgl_pixel_format.txt>.
    GLEX_WGL_ARB_pixel_format_init();

    // Try to initialize an extension to ask for multi-sampled pixel formats, see
    // <http://www.opengl.org/registry/specs/ARB/multisample.txt>.
    GLEX_ARB_multisample_init();

    // Try to initialize an extension required to create a context of a specific
    // version, see <http://www.opengl.org/registry/specs/ARB/wgl_create_context.txt>.
    GLEX_WGL_ARB_create_context_init();

    AttributeListi attr;
    GLint format=0;

    if (GLEX_WGL_ARB_pixel_format) {
        if (pixel_attr) {
            // Either use any given custom pixel format attributes ...
            attr=*pixel_attr;
        }
        else {
            // ... or specify some default pixel format attributes.
            attr.insert(WGL_PIXEL_TYPE_ARB,WGL_TYPE_RGBA_ARB);
            attr.insert(WGL_COLOR_BITS_ARB,DEFAULT_COLOR_BITS);
            attr.insert(WGL_DEPTH_BITS_ARB,DEFAULT_DEPTH_BITS);
            attr.insert(WGL_STENCIL_BITS_ARB,DEFAULT_STENCIL_BITS);
        }

        // Make sure some required attributes are always specified (attributes
        // specified later override earlier ones).
        attr.insert(WGL_DRAW_TO_WINDOW_ARB,TRUE);
        attr.insert(WGL_SUPPORT_OPENGL_ARB,TRUE);
        attr.insert(WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB);
        attr.insert(WGL_DOUBLE_BUFFER_ARB,TRUE);

        // Try to find a matching (one-based) pixel format.
        UINT count;
        wglChoosePixelFormatARB(m_context.device,attr,NULL,1,&format,&count);

        if (samples>0 && GLEX_ARB_multisample) {
            // Try to get the same format again with multi-sampling.
            attr.insert(WGL_SAMPLE_BUFFERS_ARB,TRUE);
            attr.insert(WGL_SAMPLES_ARB,samples);

            GLint format_multisample=0;
            if (wglChoosePixelFormatARB(m_context.device,attr,NULL,1,&format_multisample,&count)!=FALSE) {
                format=format_multisample;
            }
        }

        if (format>0) {
            // Destroy the default surface in order to set a custom pixel format.
            destroyRenderContext();
            destroyDeviceContext();

            // Create a surface with a custom pixel format.
            G_ASSERT_CALL(createDeviceContext(format))
        }
    }

    if (GLEX_WGL_ARB_create_context) {
        if (context_attr) {
            // Either use any given custom context attributes ...
            attr=*context_attr;
        }
        else {
            // ... or clear the attributes to get a default context.
            attr.clear();
        }

        // Destroy the default render context in order to create a more
        // sophisticated one.
        destroyRenderContext();

        m_context.render=wglCreateContextAttribsARB(m_context.device,NULL,attr);
    }

    if (!m_context.render) {
        m_context.render=wglCreateContext(m_context.device);
    }
    G_ASSERT(m_context.render)

    G_ASSERT_CALL(makeCurrent())
}

bool RenderSurface::createDeviceContext(int pixel_format)
{
    // Create a dummy window using a predefined system class to spare the code
    // to create an own class. This has the disadvantage that a custom window
    // procedure can only be set after creation, and thus it will never receive
    // a WM_CREATE or initial WM_SIZE message.
    m_window=CreateWindow(
        "static"      // lpClassName ("edit" is shorter, but has the wrong mouse cursor)
    ,   NULL          // lpWindowName
    ,   WS_POPUP      // dwStyle
    ,   0             // x
    ,   0             // y
    ,   0             // nWidth
    ,   0             // nHeight
    ,   HWND_DESKTOP  // hWndParent
    ,   NULL          // hMenu
    ,   NULL          // hInstance
    ,   NULL          // lpParam
    );

    if (m_window) {
        // Set the user data first and then the window procedure, so it will
        // never be called without the class instance pointer.
        SetWindowLongPtr(m_window,GWLP_USERDATA,(LONG_PTR)this);
        SetWindowLongPtr(m_window,GWLP_WNDPROC,(LONG_PTR)WindowProc);

        m_context.device=GetDC(m_window);
        if (m_context.device) {
            // If no (one-based) pixel format was specified, try to find a
            // matching one for our default pixel format descriptor.
            if (pixel_format<=0) {
                pixel_format=ChoosePixelFormat(m_context.device,&s_pfd);
            }

            // Setting the pixel format is only allowed once per window! Passing
            // NULL as the last argument is undocumented, but seems to work.
            if (SetPixelFormat(m_context.device,pixel_format,NULL)!=FALSE) {
                // Do not create a rendering context yet.
                return true;
            }
        }
    }

    destroyDeviceContext();

    return false;
}

void RenderSurface::destroyDeviceContext()
{
    if (m_context.device) {
        G_ASSERT_CALL(ReleaseDC(m_window,m_context.device))
        m_context.device=NULL;
    }
    if (m_window) {
        // Send WM_DESTROY, which triggers WM_QUIT via PostQuitMessage() to
        // indicate a request to terminate the application.
        G_ASSERT_CALL(DestroyWindow(m_window))
        m_window=NULL;
    }
}

void RenderSurface::destroyRenderContext()
{
    if (m_context.render) {
        G_ASSERT_CALL(wglDeleteContext(m_context.render))
        m_context.render=NULL;
    }
}

LRESULT RenderSurface::handleMessage(UINT const uMsg,WPARAM const wParam,LPARAM const lParam)
{
    switch (uMsg) {
        // This is sent to a window after it is removed from screen.
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }

        // Avoid GDI clearing the background under Windows Vista with Aero
        // enabled, see <http://opengl.org/pipeline/article/vol003_7/>.
        case WM_ERASEBKGND: {
            return 1;
        }

        default: {
            // Let Window's default handler process other messages.
            return DefWindowProc(m_window,uMsg,wParam,lParam);
        }
    }
}

LRESULT CALLBACK RenderSurface::WindowProc(WindowHandle hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    // size_t is 4 byte on 32-bit and 8 bytes on 64-bit, just like pointers.
    size_t ptr=static_cast<size_t>(GetWindowLongPtr(hWnd,GWLP_USERDATA));

    // Using a dynamic_cast here would be safer, but that requires RTTI support.
    RenderSurface *_this=reinterpret_cast<RenderSurface*>(ptr);

    // Should never be NULL as we set the user data before setting the windows
    // procedure.
    G_ASSERT(_this)

    // Dispatch to the class instance's message handler.
    return _this->handleMessage(uMsg,wParam,lParam);
}

#endif // G_OS_WINDOWS

} // namespace wrapgl

} // namespace gale
