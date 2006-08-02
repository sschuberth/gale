#include <windows.h>
#include <gale/global/defines.h>

class RenderTarget {
  public:
    RenderTarget();

  //protected:
  //  HGLRC m_rendering_context;
};

RenderTarget::RenderTarget() {
    // Register a class that allocates a unique device context for each window.
    WNDCLASS cls;
    ZeroMemory(&cls,sizeof(cls));
    cls.style=CS_OWNDC;
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
}

int main() {
    RenderTarget rt;

    system("pause");
    return 0;
}
