#ifndef DEFAULTWINDOW
#define DEFAULTWINDOW

#include "gale/wrapgl/renderwindow.h"
#include "gale/wrapgl/camera.h"

#ifndef NDEBUG
    #include <iostream>
#endif

namespace gale {

namespace wrapgl {

// warning C4355: 'this' : used in base member initializer list
#pragma warning(disable:4355)

class DefaultWindow:public RenderWindow
{
  public:

    enum MouseEvent {
        ME_BUTTON_NONE   = 0x00,
        ME_BUTTON_LEFT   = 0x01,
        ME_BUTTON_MIDDLE = 0x02,
        ME_BUTTON_RIGHT  = 0x04,
        ME_BUTTON_WHEEL  = 0x08
    };

    static system::AttributeListi const& getDefaultAttributes() {
        static system::AttributeListi attribs;
        if (attribs.getSize()==0) {
            attribs.insert(WGL_PIXEL_TYPE_ARB,WGL_TYPE_RGBA_ARB);
            attribs.insert(WGL_COLOR_BITS_ARB,24);
            attribs.insert(WGL_RED_BITS_ARB,8);
            attribs.insert(WGL_GREEN_BITS_ARB,8);
            attribs.insert(WGL_BLUE_BITS_ARB,8);
            attribs.insert(WGL_ALPHA_BITS_ARB,8);
            attribs.insert(WGL_DEPTH_BITS_ARB,24);
        }
        return attribs;
    }

    DefaultWindow(LPCTSTR title):
      RenderWindow(500,500,getDefaultAttributes(),title),m_camera(*this) {
#ifndef NDEBUG
        std::cout << "Vendor   : " << glGetString(GL_VENDOR) << std::endl;
        std::cout << "Renderer : " << glGetString(GL_RENDERER) << std::endl;
        std::cout << "Version  : " << glGetString(GL_VERSION) << std::endl;
#endif
        m_camera.setPosition(math::Vec3f(0,0,5));
    }

    virtual void onKeyEvent(char key) {
        if (key==VK_ESCAPE) {
            exit(0);
        }
    }

    virtual void onMouseEvent(int x,int y,int wheel,int event) {
        static int mouse_prev_x=0,mouse_prev_y=0;

        // This is positive if the mouse cursor was moved to the right.
        int mouse_diff_x=x-mouse_prev_x;

        // This is positive if the mouse cursor was moved downwards.
        int mouse_diff_y=y-mouse_prev_y;

        if (event!=ME_BUTTON_NONE) {
            // Simulate translating the object by inversely translating the camera.

            if (event&ME_BUTTON_MIDDLE) {
                m_camera.strafe(static_cast<float>(mouse_diff_x)/100);
                m_camera.elevate(static_cast<float>(-mouse_diff_y)/100);
            }

            if (event&ME_BUTTON_WHEEL) {
                m_camera.zoom(static_cast<float>(-wheel)/50000);
            }

            // Rotate the camera.

            if (event&ME_BUTTON_LEFT) {
                m_camera.pitch(math::convDegToRad(-mouse_diff_y)/10);
                m_camera.yaw(math::convDegToRad(-mouse_diff_x)/10);
            }

            if (event&ME_BUTTON_RIGHT) {
                m_camera.roll(math::convDegToRad(-mouse_diff_x)/10);
            }

            repaint();
        }

        mouse_prev_x=x;
        mouse_prev_y=y;
    }

  protected:

    /// Handles window messages and forwards them to the event handlers.
    LRESULT handleMessage(UINT uMsg,WPARAM wParam,LPARAM lParam) {
        switch (uMsg) {
            // The WM_ACTIVATE message is sent to both the window being
            // activated and the window being deactivated. If the windows use
            // the same input queue, the message is sent synchronously, first to
            // the window procedure of the top-level window being deactivated,
            // then to the window procedure of the top-level window being
            // activated. If the windows use different input queues, the message
            // is sent asynchronously, so the window is activated immediately.
            //case WM_ACTIVATE: {
            //    if (LOWORD(wParam)==WA_ACTIVE) {
            //        SetCapture(getWindowHandle());
            //    }
            //    break;
            //}

            // The WM_CHAR message is posted to the window with the keyboard
            // focus when a WM_KEYDOWN message is translated by the
            // TranslateMessage function. The WM_CHAR message contains the
            // character code of the key that was pressed.
            case WM_CHAR: {
                onKeyEvent(LOBYTE(wParam));
                break;
            }

            // The WM_MOUSEMOVE message is posted to a window when the cursor
            // moves. If the mouse is not captured, the message is posted to the
            // window that contains the cursor. Otherwise, the message is posted
            // to the window that has captured the mouse.
            case WM_MOUSEMOVE: {
                int event=ME_BUTTON_NONE;
                if (wParam&MK_LBUTTON) {
                    event|=ME_BUTTON_LEFT;
                }
                if (wParam&MK_MBUTTON) {
                    event|=ME_BUTTON_MIDDLE;
                }
                if (wParam&MK_RBUTTON) {
                    event|=ME_BUTTON_RIGHT;
                }
                onMouseEvent(LOWORD(lParam),HIWORD(lParam),0,event);
                break;
            }

            // The WM_MOUSEWHEEL message is sent to the focus window when the
            // mouse wheel is rotated. The DefWindowProc function propagates the
            // message to the window's parent.
            case WM_MOUSEWHEEL: {
                short wheel=HIWORD(wParam);
                onMouseEvent(LOWORD(lParam),HIWORD(lParam),wheel*WHEEL_DELTA,ME_BUTTON_WHEEL);
            }

            default: {
                // Handle more elementary messages in the base class.
                return RenderWindow::handleMessage(uMsg,wParam,lParam);
            }
        }

        return 0;
    }

    wrapgl::Camera m_camera;
};

// warning C4355: 'this' : used in base member initializer list
#pragma warning(default:4355)

} // namespace wrapgl

} // namespace gale

#endif // DEFAULTWINDOW
