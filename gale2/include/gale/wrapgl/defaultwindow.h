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

#ifndef DEFAULTWINDOW
#define DEFAULTWINDOW

/**
 * \file
 * Default window and camera navigation implementations
 */

#include "minimalwindow.h"
#include "camera.h"
#include "helpers.h"

namespace gale {

namespace wrapgl {

// TODO: Add Linux implementation.
#ifdef G_OS_WINDOWS

// Warning C4355: 'this' used in base member initializer list.
#pragma warning(disable:4355)

/**
 * This class provides a convenient default window along with camera navigation
 * using the mouse to be able to easily implement applications that require
 * simple scene interaction.
 */
class DefaultWindow:public MinimalWindow
{
  public:

    /// Defines bit masks for mouse events.
    enum MouseEvent {
        ME_BUTTON_NONE   = 0,      ///< No mouse button was pressed.
        ME_BUTTON_LEFT   = 1 << 0, ///< The left mouse button was pressed.
        ME_BUTTON_MIDDLE = 1 << 1, ///< The middle mouse button was pressed.
        ME_BUTTON_RIGHT  = 1 << 2, ///< The right mouse button was pressed.
        ME_WHEEL_SCROLL  = 1 << 3  ///< The mouse wheel was scrolled.
    };

    /// Creates a window with reasonable defaults set.
    DefaultWindow(LPCTSTR title,int const client_width=500,int const client_height=500)
    :   MinimalWindow(title,client_width,client_height)
    ,   m_camera(this)
#ifndef GALE_TINY_CODE
    ,   m_logo(0)
#endif
    {
        // Add an "About" entry to the system menu.
        HMENU menu=GetSystemMenu(windowHandle(),FALSE);
        if (menu) {
            AppendMenu(menu,MF_SEPARATOR,NULL,NULL);
            AppendMenu(menu,MF_STRING,ID_ABOUT_DLG,_T("&About ..."));
        }
    }

    void onClose() {
#ifndef GALE_TINY_CODE
        // Free any OpenGL resources, else tools like gDEBugger will report
        // graphic memory leaks.
        glDeleteLists(m_logo,1);
        G_ASSERT_OPENGL
#endif
    }

    /// Adjusts the camera if the window size changes.
    void onResize(int width,int height) {
        static int const init_width=width;
        static double const init_fov=math::Constd::PI()*0.25;

        double fov;

        if (width>=init_width) {
            // Scale the FOV down in the same ration the width increased.
            fov=static_cast<double>(init_width)/width*init_fov;
        }
        else {
            // Scale the FOV so that for a zero window width it becomes PI.
            fov=(init_fov-math::Constd::PI())/init_width*width+math::Constd::PI();
        }

        m_camera.setScreenSpaceDimensions(width,height);
        m_camera.setFOV(fov);

        repaint();
    }

    /// Overrides the default paint event handler to draw a logo.
    void onPaint() {
        onRender();
#ifndef GALE_TINY_CODE
        m_logo=drawLogo();
#endif
    }

    /// Applications that wish to benefit from extra stuff (like logos) being
    /// automatically rendered should override this method instead of onPaint().
    virtual void onRender() {}

    /// Captures the Escape key to quit the application.
    virtual void onKeyEvent(char key) {
        if (key==VK_ESCAPE) {
            ExitProcess(0);
        }
    }

    /// Translates mouse events into camera movements.
    virtual void onMouseEvent(int x,int y,int wheel,int event);

  protected:

    /// Defines a custom ID for the About dialog menu entry.
    static UINT_PTR const ID_ABOUT_DLG=0x00AB;

    /// Handles window messages and forwards them to the event methods.
    LRESULT handleMessage(UINT const uMsg,WPARAM const wParam,LPARAM const lParam);

    wrapgl::Camera m_camera; ///< The window's default camera.

#ifndef GALE_TINY_CODE
    GLuint m_logo; ///< Display list identifier of the logo.
#endif
};

// Warning C4355: 'this' used in base member initializer list.
#pragma warning(default:4355)

#endif // G_OS_WINDOWS

} // namespace wrapgl

} // namespace gale

#endif // DEFAULTWINDOW
