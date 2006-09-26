#include "gale/system/rendertarget.h"

#include "gale/global/defines.h"
#include "gale/global/platform.h"

//#include "../../glex/WGL_ARB_pixel_format.h"

namespace gale {

namespace system {

unsigned int RenderTarget::s_instances=0;

RenderTarget::RenderTarget() {
    //if (s_instances==0) {

    // Register a minimal class that allocates a unique device context for each window.
    WNDCLASS cls;
    ZeroMemory(&cls,sizeof(cls));
    cls.style=CS_OWNDC;
    cls.lpfnWndProc=DefWindowProc;
    cls.lpszClassName="G";

    ATOM atom=RegisterClass(&cls);
    G_ASSERT(atom!=0)

    // Create a dummy window and rendering context to access OpenGL extensions.
    HWND wnd=CreateWindow(
    /* lpClassName  */ MAKEINTATOM(atom),
    /* lpWindowName */ NULL,
    /* dwStyle      */ 0,
    /* x            */ 0,
    /* y            */ 0,
    /* nWidth       */ 0,
    /* nHeight      */ 0,
    /* hWndParent   */ HWND_DESKTOP,
    /* hMenu        */ NULL,
    /* hInstance    */ NULL,
    /* lpParam      */ this
    );
    G_ASSERT(wnd!=NULL)

    HDC dc=GetWindowDC(wnd);
    G_ASSERT(dc!=NULL)

    // Set the device context to a pixel format that uses accelerated OpenGL.
    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory(&pfd,sizeof(pfd));
    pfd.nSize=sizeof(pfd);
    pfd.nVersion=1;
    pfd.dwFlags=PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_GENERIC_ACCELERATED;

    int format=ChoosePixelFormat(dc,&pfd);
    G_ASSERT(format!=0)

    BOOL result=SetPixelFormat(dc,format,&pfd);
    G_ASSERT(result!=FALSE)

    HGLRC rc=wglCreateContext(dc);
    G_ASSERT(rc!=NULL)

    result=wglMakeCurrent(dc,rc);
    G_ASSERT(result!=FALSE)

    //GLboolean r=WGL_ARB_pixel_format_init();

    result=wglMakeCurrent(NULL,NULL);
    G_ASSERT(result!=FALSE)

    result=wglDeleteContext(rc);
    G_ASSERT(result!=FALSE)

    result=ReleaseDC(wnd,dc);
    G_ASSERT(result!=FALSE)

    result=DestroyWindow(wnd);
    G_ASSERT(result!=FALSE)

    result=UnregisterClass(MAKEINTATOM(atom),NULL);
    G_ASSERT(result!=FALSE)
}

} // namespace system

} // namespace gale
