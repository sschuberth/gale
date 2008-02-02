/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
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

#ifndef DEFAULTWINDOW
#define DEFAULTWINDOW

/**
 * \file
 * Default window and camera navigation implementation
 */

#include "../wrapgl/renderwindow.h"
#include "../wrapgl/camera.h"

#ifndef NDEBUG
    #include <iostream>
#endif

namespace gale {

namespace wrapgl {

// warning C4355: 'this' : used in base member initializer list
#pragma warning(disable:4355)

/**
 * This class provides a convenient default window along with camera navigation
 * using the mouse to be able to easily implement applications that require
 * simple scene interaction.
 */

// TODO: Add Linux implementation.
#ifdef G_OS_WINDOWS

class DefaultWindow:public RenderWindow
{
  public:

    /// Defines bit masks for mouse events.
    enum MouseEvent {
        ME_BUTTON_NONE   = 0x00, ///< No mouse button was pressed.
        ME_BUTTON_LEFT   = 0x01, ///< The left mouse button was pressed.
        ME_BUTTON_MIDDLE = 0x02, ///< The middle mouse button was pressed.
        ME_BUTTON_RIGHT  = 0x04, ///< The right mouse button was pressed.
        ME_WHEEL_SCROLL  = 0x08  ///< The mouse wheel was scrolled.
    };

    /// Returns the default pixel format attributes to use.
    static system::AttributeListi const& getPixelAttributes() {
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

    /// Creates a window with reasonable defaults set.
    DefaultWindow(LPCTSTR title,int client_width=500,int client_height=500)
    :   RenderWindow(client_width,client_height,getPixelAttributes(),title)
    ,   m_camera(*this)
    {
#ifndef NDEBUG
        std::cout << "Vendor   : " << glGetString(GL_VENDOR) << std::endl;
        std::cout << "Renderer : " << glGetString(GL_RENDERER) << std::endl;
        std::cout << "Version  : " << glGetString(GL_VERSION) << std::endl;
#endif
    }

    /// Captures the Escape key to quit the application.
    virtual void onKeyEvent(char key) {
        if (key==VK_ESCAPE) {
            exit(0);
        }
    }

    /// Translates mouse events into camera movements.
    virtual void onMouseEvent(int x,int y,int wheel,int event) {
        using namespace math;

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

            if (event&ME_WHEEL_SCROLL) {
                m_camera.zoom(static_cast<float>(-wheel)/50000);
            }

            // Rotate the camera.

            if (event&ME_BUTTON_LEFT) {
                m_camera.pitch(convDegToRad(-mouse_diff_y)/10);
                m_camera.rotate(Vec3f::Y(),convDegToRad(-mouse_diff_x)/10);
            }

            if (event&ME_BUTTON_RIGHT) {
                m_camera.roll(convDegToRad(-mouse_diff_x)/10);
            }

            repaint();
        }

        mouse_prev_x=x;
        mouse_prev_y=y;
    }

  protected:

    /// Handles window messages and forwards them to the event handlers.
    LRESULT handleMessage(UINT uMsg,WPARAM wParam,LPARAM lParam) {
        static bool capture=false;

        switch (uMsg) {
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

                // Toggle the mouse capture when buttons get pressed or released.
                if (event==ME_BUTTON_NONE) {
                    if (capture) {
                        ReleaseCapture();
                        capture=false;
                    }
                }
                else {
                    if (!capture) {
                        SetCapture(getWindowHandle());
                        capture=true;
                    }
                }

                short x=LOWORD(lParam),y=HIWORD(lParam);
                onMouseEvent(x,y,0,event);
                break;
            }

            // The WM_MOUSEWHEEL message is sent to the focus window when the
            // mouse wheel is rotated. The DefWindowProc function propagates the
            // message to the window's parent.
            case WM_MOUSEWHEEL: {
                short wheel=HIWORD(wParam)*WHEEL_DELTA;
                onMouseEvent(LOWORD(lParam),HIWORD(lParam),wheel,ME_WHEEL_SCROLL);
                break;
            }

            default: {
                // Handle more elementary messages in the base class.
                return RenderWindow::handleMessage(uMsg,wParam,lParam);
            }
        }

        return 0;
    }

    wrapgl::Camera m_camera; ///< The window's default camera.
};

#endif // G_OS_WINDOWS

// warning C4355: 'this' : used in base member initializer list
#pragma warning(default:4355)

} // namespace wrapgl

} // namespace gale

#endif // DEFAULTWINDOW
