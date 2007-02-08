#include "gale/system/rendercontext.h"

#include "gale/global/defines.h"

namespace gale {

namespace system {

int RenderContext::s_instances=0;
ATOM RenderContext::s_atom=0;

RenderContext::RenderContext()
{
#ifdef _WIN32

    BOOL result;

    // We only need to create one rendering context once for all instances.
    if (s_instances==0) {
        // Register a minimal class whose windows allocate a unique device context.
        WNDCLASS cls;
        ZeroMemory(&cls,sizeof(cls));
        cls.style=CS_OWNDC;
        cls.lpfnWndProc=DefWindowProc;
        cls.hCursor=LoadCursor(NULL,IDC_ARROW);
        cls.lpszClassName="G";

        s_atom=RegisterClass(&cls);
        G_ASSERT(s_atom!=0)

        // Create a dummy window and get its device context.
        m_wnd=CreateWindow(
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
        G_ASSERT(m_wnd!=NULL)

        m_dc=GetWindowDC(m_wnd);
        G_ASSERT(m_dc!=NULL)

        // Set the device context to a pixel format that uses OpenGL acceleration.
        PIXELFORMATDESCRIPTOR pfd;
        ZeroMemory(&pfd,sizeof(pfd));
        pfd.nSize=sizeof(pfd);
        pfd.nVersion=1;
        pfd.dwFlags=PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_GENERIC_ACCELERATED;

        int format=ChoosePixelFormat(m_dc,&pfd);
        G_ASSERT(format!=0)

        result=SetPixelFormat(m_dc,format,&pfd);
        G_ASSERT(result!=FALSE)

        // Create and activate a rendering context.
        m_rc=wglCreateContext(m_dc);
        G_ASSERT(m_rc!=NULL)
    }
    ++s_instances;

    if (getCurrent()==NULL) {
        result=setCurrent();
        G_ASSERT(result!=FALSE)
    }

#else

    // TODO: Linux code.

#endif // _WIN32
}

RenderContext::~RenderContext()
{
    --s_instances;
    if (s_instances<=0) {
        s_instances=0;

        // Clean up again.
        destroy();

        BOOL result=UnregisterClass(MAKEINTATOM(s_atom),NULL);
        G_ASSERT(result!=FALSE)
    }
}

void RenderContext::destroy()
{
    BOOL result;

    // Call the virtual getter-methods instead of accessing the member variables
    // directly so derived classes can use this method with their own handles.
    if (getCurrent()==getRenderHandle()) {
        result=wglMakeCurrent(NULL,NULL);
        G_ASSERT(result!=FALSE)
    }

    result=wglDeleteContext(getRenderHandle());
    G_ASSERT(result!=FALSE)

    result=ReleaseDC(getWindowHandle(),getDeviceHandle());
    G_ASSERT(result!=FALSE)

    result=DestroyWindow(getWindowHandle());
    G_ASSERT(result!=FALSE)
}

} // namespace system

} // namespace gale
