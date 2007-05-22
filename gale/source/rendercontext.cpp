#include "gale/system/rendercontext.h"

#include "gale/global/defines.h"

namespace gale {

namespace system {

ATOM RenderContext::s_atom=0;

int RenderContext::s_instances=0;

HWND RenderContext::s_window=NULL;
RenderContext::Handle RenderContext::s_handle=NULL;

LRESULT CALLBACK RenderContext::WindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    // Using a dynamic_cast here would be safer, but that requires RTTI support.
    RenderContext *_this=reinterpret_cast<RenderContext*>(GetWindowLong(hWnd,GWL_USERDATA));

    if (uMsg==WM_CREATE) {
        LPVOID params=reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams;
        _this=static_cast<RenderContext*>(params);
        SetWindowLongA(hWnd,GWL_USERDATA,(LONG)_this);
    }

    if (_this==NULL) {
        return DefWindowProc(hWnd,uMsg,wParam,lParam);
    }

    return _this->handleMessage(uMsg,wParam,lParam);
}

RenderContext::RenderContext()
{
#ifdef _WIN32

    // We only need to create one rendering context once for all instances.
    if (s_instances==0) {
        // Register a minimal class whose windows allocate a unique device context.
        WNDCLASS cls;
        memset(&cls,0,sizeof(cls));
        cls.style=CS_OWNDC;
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
    }
    ++s_instances;

#else

    // TODO: Linux code.

#endif // _WIN32
}

RenderContext::~RenderContext()
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

void RenderContext::destroy()
{
    Handle handle=getContextHandle();

    BOOL result=wglDeleteContext(handle.render);
    G_ASSERT(result!=FALSE)

    result=ReleaseDC(getWindowHandle(),handle.device);
    G_ASSERT(result!=FALSE)

    result=DestroyWindow(getWindowHandle());
    G_ASSERT(result!=FALSE)
}

LRESULT RenderContext::handleMessage(UINT uMsg,WPARAM wParam,LPARAM lParam)
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
