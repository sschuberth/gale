/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
 *
 * Copyright (C) 2005-2009  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
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

void DefaultWindow::onMouseEvent(int x,int y,int wheel,int event)
{
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
            m_camera.approach(static_cast<float>(wheel)/50000);
        }

        // Rotate the camera.
        if (event&ME_BUTTON_LEFT) {
            m_camera.pitch(-mouse_diff_y*Constd::DEG_TO_RAD()/10);
            m_camera.rotate(Vec3f::Y(),-mouse_diff_x*Constd::DEG_TO_RAD()/10);
        }

        if (event&ME_BUTTON_RIGHT) {
            m_camera.roll(-mouse_diff_x*Constd::DEG_TO_RAD()/10);
        }

        repaint();
    }

    mouse_prev_x=x;
    mouse_prev_y=y;
}

LRESULT DefaultWindow::handleMessage(UINT uMsg,WPARAM wParam,LPARAM lParam)
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
                    buffer,
                    _T("Vendor: %s\nRenderer: %s\nVersion: %s"),
                    glGetString(GL_VENDOR),
                    glGetString(GL_RENDERER),
                    glGetString(GL_VERSION)
                );
                MessageBox(windowHandle(),buffer,_T("About OpenGL"),MB_OK);
                break;
            }
            // No break here!
        }

        default: {
            // Handle more elementary messages in the base class.
            return RenderWindow::handleMessage(uMsg,wParam,lParam);
        }
    }

    return 0;
}

#endif // G_OS_WINDOWS

} // namespace wrapgl

} // namespace gale
