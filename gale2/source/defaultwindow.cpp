/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
 *
 * Copyright (C) 2005-2010  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
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

#include "gale/wrapgl/defaultwindow.h"

using namespace gale::math;

namespace gale {

namespace wrapgl {

// TODO: Add Linux implementation.
#ifdef G_OS_WINDOWS

bool DefaultWindow::toggleFullScreen(bool state)
{
    if (state) {
        SetWindowLongPtr(windowHandle(),GWL_STYLE,WS_POPUP|WS_VISIBLE);
        SetWindowPos(windowHandle(),HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED);

        ShowWindow(windowHandle(),SW_MAXIMIZE);
    }
    else {
        ShowWindow(windowHandle(),SW_RESTORE);

        SetWindowLongPtr(windowHandle(),GWL_STYLE,WS_OVERLAPPEDWINDOW|WS_VISIBLE);
        SetWindowPos(windowHandle(),HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED);
    }

    return true;
}

void DefaultWindow::onMouseEvent(int x,int y,int wheel,int event)
{
    G_UNREF_PARAM(wheel)

    static int mouse_prev_x=0,mouse_prev_y=0;

    // This is positive if the mouse cursor was moved to the right.
    int mouse_diff_x=x-mouse_prev_x;

    // This is positive if the mouse cursor was moved downwards.
    int mouse_diff_y=y-mouse_prev_y;

    if (event!=ME_BUTTON_NONE) {
        if (event&ME_BUTTON_LEFT) {
            HMat4f m=m_camera.getModelview();
            m=HMat4f::Factory::Rotation(m.getUpVector(),-mouse_diff_x*Constf::DEG_TO_RAD()*0.5f)*m;
            m=HMat4f::Factory::Rotation(m.getRightVector(),-mouse_diff_y*Constf::DEG_TO_RAD()*0.5f)*m;

            // Get rid of rounding errors if called many times with small mouse
            // cursor position changes.
            m.orthonormalize();

            m_camera.setModelview(m);
        }

        if (event&ME_BUTTON_MIDDLE) {
            m_camera.strafe(static_cast<float>(-mouse_diff_x)/60);
            m_camera.elevate(static_cast<float>(mouse_diff_y)/60);
        }

        if (event&ME_BUTTON_RIGHT) {
            m_camera.approach(-mouse_diff_y*Constf::DEG_TO_RAD()*4);
        }

        repaint();
    }

    mouse_prev_x=x;
    mouse_prev_y=y;
}

LRESULT DefaultWindow::handleMessage(UINT const uMsg,WPARAM const wParam,LPARAM const lParam)
{
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
                    SetCapture(windowHandle());
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

        case WM_SYSCOMMAND: {
            if (wParam==ID_ABOUT_DLG) {
                TCHAR buffer[512];
                wsprintf(
                    buffer
                ,   _T("Vendor: %s\nRenderer: %s\nVersion: %s")
                ,   glGetString(GL_VENDOR)
                ,   glGetString(GL_RENDERER)
                ,   glGetString(GL_VERSION)
                );
                G_ASSERT_OPENGL
                MessageBox(windowHandle(),buffer,_T("About OpenGL"),MB_OK);
                break;
            }
            // No break here!
        }

        default: {
            // Handle more elementary messages in the base class.
            return MinimalWindow::handleMessage(uMsg,wParam,lParam);
        }
    }

    return 0;
}

#endif // G_OS_WINDOWS

} // namespace wrapgl

} // namespace gale
