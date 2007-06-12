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

#include "gale/system/rendersurface.h"

#include "gale/global/defines.h"

namespace gale {

namespace system {

ATOM RenderSurface::s_atom=0;
int RenderSurface::s_instances=0;

RenderSurface::WindowHandle RenderSurface::s_window=NULL;
RenderSurface::ContextHandle RenderSurface::s_handle=NULL;

LRESULT CALLBACK RenderSurface::WindowProc(WindowHandle hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    // Using a dynamic_cast here would be safer, but that requires RTTI support.
    RenderSurface *_this=reinterpret_cast<RenderSurface*>(GetWindowLong(hWnd,GWL_USERDATA));

    if (uMsg==WM_CREATE) {
        LPVOID params=reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams;
        _this=static_cast<RenderSurface*>(params);
        SetWindowLongA(hWnd,GWL_USERDATA,(LONG)_this);
    }

    if (_this==NULL) {
        return DefWindowProc(hWnd,uMsg,wParam,lParam);
    }

    return _this->handleMessage(uMsg,wParam,lParam);
}

RenderSurface::RenderSurface()
{
    // We only need to create one rendering context once for all instances.
    if (s_instances==0) {

#ifdef _WIN32

        // Register a minimal class whose windows allocate a unique device context.
        WNDCLASS cls;
        memset(&cls,0,sizeof(cls));
        cls.style=CS_OWNDC|CS_SAVEBITS;
        cls.lpfnWndProc=WindowProc;
        cls.hCursor=LoadCursor(NULL,IDC_ARROW);
        cls.lpszClassName="G";

        s_atom=RegisterClass(&cls);
        G_ASSERT(s_atom!=0)

        // Create a dummy window and get its device context.
        s_window=CreateWindow(
        /* lpClassName  */ MAKEINTATOM(s_atom),
        /* lpWindowName */ NULL,
        /* dwStyle      */ 0,
        /* x            */ 0,
        /* y            */ 0,
        /* nWidth       */ 0,
        /* nHeight      */ 0,
        /* hWndParent   */ HWND_DESKTOP,
        /* hMenu        */ NULL,
        /* hInstance    */ NULL,
        /* lpParam      */ NULL
        );
        G_ASSERT(s_window!=NULL)

        s_handle.device=GetWindowDC(s_window);
        G_ASSERT(s_handle.device!=NULL)

        // Set the device context to a pixel format that uses OpenGL acceleration.
        PIXELFORMATDESCRIPTOR pfd;
        memset(&pfd,0,sizeof(pfd));
        pfd.nSize=sizeof(pfd);
        pfd.nVersion=1;
        pfd.dwFlags=PFD_DRAW_TO_WINDOW
                  | PFD_SUPPORT_OPENGL
                  | PFD_GENERIC_ACCELERATED
                  | PFD_SUPPORT_COMPOSITION
        ;

        int format=ChoosePixelFormat(s_handle.device,&pfd);
        G_ASSERT(format!=0)

        BOOL result=SetPixelFormat(s_handle.device,format,&pfd);
        G_ASSERT(result!=FALSE)

        // Create and activate a rendering context.
        s_handle.render=wglCreateContext(s_handle.device);
        G_ASSERT(s_handle.render!=NULL)

#else

        // TODO: Linux code.

#endif // _WIN32

    }
    ++s_instances;
}

RenderSurface::~RenderSurface()
{
    --s_instances;
    if (s_instances<=0) {
        G_ASSERT(s_instances==0)

        // Clean up again.
        destroy();

        BOOL result=UnregisterClass(MAKEINTATOM(s_atom),NULL);
        G_ASSERT(result!=FALSE)
    }
}

void RenderSurface::destroy()
{
    ContextHandle handle=getContextHandle();

    BOOL result=wglDeleteContext(handle.render);
    G_ASSERT(result!=FALSE)

    result=ReleaseDC(getWindowHandle(),handle.device);
    G_ASSERT(result!=FALSE)

    result=DestroyWindow(getWindowHandle());
    G_ASSERT(result!=FALSE)
}

LRESULT RenderSurface::handleMessage(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    switch (uMsg) {
        // This is sent to a window after it is removed from screen.
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }

        // Avoid GDI clearing the background under Windows Vista with Aero enabled
        // (see http://www.opengl.org/pipeline/article/vol003_7/).
        case WM_ERASEBKGND: {
            return 1;
        }
    }

    return DefWindowProc(getWindowHandle(),uMsg,wParam,lParam);
}

} // namespace system

} // namespace gale
