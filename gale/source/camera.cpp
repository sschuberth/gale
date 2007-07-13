/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at http://developer.berlios.de/projects/gale/
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

#include "gale/wrapgl/camera.h"

namespace gale {

namespace wrapgl {

Camera* Camera::s_current=NULL;

void Camera::apply(bool force)
{
    bool camera_changed=(s_current!=this) || force;

    if (camera_changed || m_screen_changed) {
        // Get the window's client area size.
        RECT rect;
        BOOL result=GetClientRect(m_surface.getWindowHandle(),&rect);
        G_ASSERT(result!=FALSE)

        // If the desired screen space is smaller than the window, we need to
        // enable scissoring to avoid glClear to affect the whole window plane.
        if (m_screen.x>0 || (m_screen.x<0 && m_screen.width<rect.right-m_screen.x)
         || m_screen.y>0 || (m_screen.y<0 && m_screen.height<rect.bottom-m_screen.y)) {
            glScissor(m_screen.x,m_screen.y,m_screen.width,m_screen.height);
            G_ASSERT_OPENGL

            glEnable(GL_SCISSOR_TEST);
        } else {
            glDisable(GL_SCISSOR_TEST);
        }
        G_ASSERT_OPENGL

        // Set the viewport OpenGL should render to.
        glViewport(m_screen.x,m_screen.y,m_screen.width,m_screen.height);
        G_ASSERT_OPENGL
        m_screen_changed=false;
    }

    glPushAttrib(GL_TRANSFORM_BIT);
    G_ASSERT_OPENGL

    if (camera_changed || m_modelview_changed) {
        glMatrixMode(GL_MODELVIEW);
        G_ASSERT_OPENGL

        // As the camera is only an imaginary concept and has no geometry,
        // instead of transforming the camera we need to inversely transform all
        // geometry in the scene.
        glLoadMatrixf(!m_modelview);
        G_ASSERT_OPENGL

        m_modelview_changed=false;
    }

    if (camera_changed || m_projection_changed) {
        glMatrixMode(GL_PROJECTION);
        G_ASSERT_OPENGL
        glLoadMatrixd(m_projection);
        G_ASSERT_OPENGL
        m_projection_changed=false;
    }

    glPopAttrib();
    G_ASSERT_OPENGL

    s_current=this;
}

} // namespace wrapgl

} // namespace gale
