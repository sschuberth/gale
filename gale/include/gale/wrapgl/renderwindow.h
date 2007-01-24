#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

#include "gale/system/attributelist.h"
#include "gale/system/rendercontext.h"

#include "GLEX_WGL_ARB_pixel_format.h"

namespace gale {

namespace wrapgl {

class RenderWindow:private system::RenderContext
{
  public:

    RenderWindow(int width,int height,system::AttributeListi const& attribs,LPCTSTR title);

    HWND getWindowHandle() const {
        return m_wnd;
    }

    HDC getDeviceHandle() const {
        return m_dc;
    }

    HGLRC getRenderHandle() const {
        return m_rc;
    }

    void handleEvents() {
        MSG msg;

        do {
            if (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            } else {
                // Idle if there are no messages to process.
                onIdle();
            }
        } while (msg.message!=WM_QUIT);
    }

    virtual void onIdle() {
        Sleep(0);
    }

    virtual void onSize() {
    }

    virtual void onPaint() {
        static int i=0;
        i=(i+1)%256;
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0,0,float(i)/255.0f,0);
    }

    virtual void onClose() {
    }

  protected:

    static LRESULT CALLBACK WindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

    HWND m_wnd;
    HDC m_dc;
    HGLRC m_rc;
};

} // namespace wrapgl

} // namespace gale

#endif // RENDERWINDOW_H
