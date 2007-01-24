#include "gale/wrapgl/renderwindow.h"

using namespace gale::system;

namespace gale {

namespace wrapgl {

LRESULT CALLBACK RenderWindow::WindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    // Using a dynamic_cast here would be safer, but that requires RTTI support.
    RenderWindow *_this=reinterpret_cast<RenderWindow*>(GetWindowLong(hWnd,GWL_USERDATA));
    if (!_this) {
        return DefWindowProc(hWnd,uMsg,wParam,lParam);
    }

    switch (uMsg) {
        // This is sent to a window after its size has changed.
        //case WM_SIZE: {
        //    _this->onSize();
        //    break;
        //}

        // This is sent to a window when a portion should be painted.
        //case WM_PAINT: {
        //    _this->onPaint();
        //    glFlush();
        //    SwapBuffers(_this->getDeviceHandle());
        //    ValidateRect(hWnd,NULL);
        //    break;
        //}

        // This is sent to a window when it should terminate.
        case WM_CLOSE: {
            _this->onClose();
            _this->destroy();
            break;
        }

        // This is sent to a window after it is removed from screen.
        case WM_DESTROY: {
            PostQuitMessage(0);
            break;
        }

        default: {
            return DefWindowProc(hWnd,uMsg,wParam,lParam);
        }
    }

    return 0;
}

RenderWindow::RenderWindow(int width,int height,AttributeListi const& attribs,LPCTSTR title)
{
    if (GLEX_WGL_ARB_pixel_format!=GL_TRUE) {
        // Initialize the needed OpenGL extensions.
        GLEX_WGL_ARB_pixel_format_init();
        G_ASSERT(GLEX_WGL_ARB_pixel_format!=GL_FALSE)
    }

    // Create a render window and get its device context.
    m_wnd=CreateWindow(
    /* lpClassName  */ MAKEINTATOM(s_atom),
    /* lpWindowName */ title,
    /* dwStyle      */ WS_OVERLAPPEDWINDOW,
    /* x            */ CW_USEDEFAULT,
    /* y            */ 0,
    /* nWidth       */ width,
    /* nHeight      */ height,
    /* hWndParent   */ HWND_DESKTOP,
    /* hMenu        */ NULL,
    /* hInstance    */ NULL,
    /* lpParam      */ NULL
    );
    G_ASSERT(m_wnd!=NULL)

    m_dc=GetWindowDC(m_wnd);
    G_ASSERT(m_dc!=NULL)

    // Set the window user data to a pointer to this object instance.
    SetWindowLong(m_wnd,GWL_USERDATA,reinterpret_cast<LONG>(this));

    // Subclass the window class by setting a new window procedure.
    SetWindowLong(m_wnd,GWL_WNDPROC,reinterpret_cast<LONG>(WindowProc));

    // Make sure some required attributes are specified.
    AttributeListi attrs=attribs;
    attrs.insert(WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB);
    attrs.insert(WGL_SUPPORT_OPENGL_ARB,TRUE);
    attrs.insert(WGL_DRAW_TO_WINDOW_ARB,TRUE);
    attrs.insert(WGL_DOUBLE_BUFFER_ARB,TRUE);

    // Set the device context to the first pixel format that matches the
    // specified attributes.
    GLint format;
    UINT count;
    BOOL result=wglChoosePixelFormatARB(m_dc,attrs,NULL,1,&format,&count);
    G_ASSERT(result!=FALSE)

    result=SetPixelFormat(m_dc,format,NULL);
    G_ASSERT(result!=FALSE)

    // Create and activate a rendering context.
    m_rc=wglCreateContext(m_dc);
    G_ASSERT(m_rc!=NULL)

    result=wglMakeCurrent(m_dc,m_rc);
    G_ASSERT(result!=FALSE)
}

} // namespace wrapgl

} // namespace gale
