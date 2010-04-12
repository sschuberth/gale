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

#include "gale/wrapgl/rendersurface.h"

#include "gale/global/defines.h"

namespace gale {

namespace wrapgl {

// TODO: Add Linux implementation.
#ifdef G_OS_WINDOWS

int RenderSurface::s_instances=0;
ATOM RenderSurface::s_atom=0;

RenderSurface::RenderSurface()
{
    // We only need to register the window class once.
    if (!s_instances++) {
        // Register a minimal window class used by all surfaces we want to create.
        WNDCLASS cls;
        memset(&cls,0,sizeof(cls));
        cls.style=CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
        cls.lpfnWndProc=WindowProc;
        cls.hCursor=LoadCursor(NULL,IDC_ARROW);
        cls.lpszClassName=_T("G");

        s_atom=RegisterClass(&cls);
        G_ASSERT(s_atom!=0)
    }
}

RenderSurface::~RenderSurface()
{
    if (--s_instances>0) {
        return;
    }
    G_ASSERT(s_instances==0)

    G_ASSERT_CALL(UnregisterClass(MAKEINTATOM(s_atom),NULL))
}

bool RenderSurface::create(int pixel_format,int const width,int const height,LPCTSTR title)
{
    // Create a default window and get its device context.
    m_window=CreateWindow(
        MAKEINTATOM(s_atom) // lpClassName
    ,   title               // lpWindowName
    ,   WS_OVERLAPPEDWINDOW // dwStyle
    ,   CW_USEDEFAULT       // x
    ,   0                   // y
    ,   width               // nWidth
    ,   height              // nHeight
    ,   HWND_DESKTOP        // hWndParent
    ,   NULL                // hMenu
    ,   NULL                // hInstance
    ,   this                // lpParam
    );

    if (m_window) {
        m_context.device=GetDC(m_window);
        if (m_context.device) {
            // If no pixel format was specified, get one.
            if (pixel_format<=0) {
                PIXELFORMATDESCRIPTOR pfd;
                memset(&pfd,0,sizeof(pfd));

                // Make sure some required attributes are specified.
                pfd.nSize=sizeof(pfd);
                pfd.nVersion=1;
                pfd.dwFlags=PFD_DRAW_TO_WINDOW
                          | PFD_SUPPORT_OPENGL
                          | PFD_GENERIC_ACCELERATED
                          | PFD_DOUBLEBUFFER
                          | PFD_SUPPORT_COMPOSITION
                ;

                // Specify some common pixel format attributes.
                pfd.iPixelType=PFD_TYPE_RGBA;
                pfd.cColorBits=32;
                pfd.cDepthBits=24;
                pfd.cStencilBits=8;

                // Try to find a matching (one-based) pixel format.
                pixel_format=ChoosePixelFormat(m_context.device,&pfd);
            }

            // Setting the pixel format is only allowed once per window! Passing
            // NULL as the last argument is undocumented, but seems to work.
            if (SetPixelFormat(m_context.device,pixel_format,NULL)!=FALSE) {
                // Do not create a rendering context yet.
                return true;
            }
        }
    }

    destroy();

    return false;
}

void RenderSurface::destroy()
{
    if (m_context.device) {
        G_ASSERT_CALL(ReleaseDC(m_window,m_context.device))
        m_context.device=NULL;
    }
    if (m_window) {
        // Send WM_DESTROY, which triggers WM_QUIT via PostQuitMessage() to
        // break out of the message loop.
        G_ASSERT_CALL(DestroyWindow(m_window))
        m_window=NULL;
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
    // Using a dynamic_cast here would be safer, but that requires RTTI support.
    size_t ptr=static_cast<size_t>(GetWindowLongPtr(hWnd,GWLP_USERDATA));
    RenderSurface *_this=reinterpret_cast<RenderSurface*>(ptr);

    if (uMsg==WM_CREATE) {
        LPVOID params=reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams;
        _this=static_cast<RenderSurface*>(params);
        ptr=reinterpret_cast<size_t>(_this);
#ifdef G_ARCH_X86_64
        SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)ptr);
#else
        SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG)ptr);
#endif
    }

    if (_this==NULL) {
        return DefWindowProc(hWnd,uMsg,wParam,lParam);
    }

    return _this->handleMessage(uMsg,wParam,lParam);
}

#endif // G_OS_WINDOWS

} // namespace wrapgl

} // namespace gale
