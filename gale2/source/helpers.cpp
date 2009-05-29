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

#include "gale/math/color.h"
#include "gale/wrapgl/camera.h"
#include "gale/wrapgl/renderer.h"

namespace gale {

namespace wrapgl {

void pushOrtho2D()
{
    // Save the current matrix mode.
    GLint matrix_mode;
    glGetIntegerv(GL_MATRIX_MODE,&matrix_mode);
    G_ASSERT_OPENGL

    // Save the original modelview matrix, then set the current one to identity.
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    G_ASSERT_OPENGL

    // Save the original projection matrix, then set the current one to identity.
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    G_ASSERT_OPENGL

    // Set the projection matrix to map each raster position to a screen pixel.
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);
    glOrtho(viewport[0],viewport[2],viewport[1],viewport[3],-1,1);
    G_ASSERT_OPENGL

    // Restore the original matrix mode.
    glMatrixMode(matrix_mode);
    G_ASSERT_OPENGL
}

void popOrtho2D()
{
    // Save the current matrix mode.
    GLint matrix_mode;
    glGetIntegerv(GL_MATRIX_MODE,&matrix_mode);
    G_ASSERT_OPENGL

    // Restore the original projection matrix.
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    G_ASSERT_OPENGL

    // Restore the original modelview matrix.
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    G_ASSERT_OPENGL

    // Restore the original matrix mode.
    glMatrixMode(matrix_mode);
    G_ASSERT_OPENGL
}

GLuint drawLogo()
{
    // Apply the required camera settings.
    Camera camera_logo;

    int size=math::min(camera_logo.getScreenSpace().width,camera_logo.getScreenSpace().height)/6;
    camera_logo.setScreenSpaceDimensions(size,size);
    camera_logo.setFOV(90*math::Constd::DEG_TO_RAD());

    float distance=3.0f;
    math::Vec3f normal(distance,distance,distance);
    camera_logo.setPosition(normal);
    camera_logo.setLookTarget(math::Vec3f::ZERO());

    camera_logo.makeCurrent();

    // Prepare a display list to speed up running draw calls.
    static GLuint list=0;

    if (list>0) {
        glCallList(list);
        G_ASSERT_OPENGL
    }
    else {
        list=glGenLists(1);
        G_ASSERT_OPENGL

        glNewList(list,GL_COMPILE_AND_EXECUTE);
        G_ASSERT_OPENGL

        // Render the logo.
        glPushAttrib(GL_COLOR_BUFFER_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        G_ASSERT_OPENGL

        glPushAttrib(GL_POLYGON_BIT);
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        G_ASSERT_OPENGL

        glPushAttrib(GL_ENABLE_BIT);
        glEnable(GL_CULL_FACE);
        glEnable(GL_LINE_SMOOTH);
        glDisable(GL_LIGHTING);
        G_ASSERT_OPENGL

        glColor3fv(math::Col3f::WHITE());
        G_ASSERT_OPENGL

        // Render the cubes.
        model::Mesh::Preparer preparer;
        model::Mesh* cube=model::Mesh::Factory::Hexahedron();

        preparer.compile(cube);

        glTranslatef(1,0,0);
        G_ASSERT_OPENGL
        Renderer::draw(preparer);

        glTranslatef(-1,1,0);
        G_ASSERT_OPENGL
        Renderer::draw(preparer);

        glTranslatef(0,-1,1);
        G_ASSERT_OPENGL
        Renderer::draw(preparer);

        glTranslatef(0,0,-1);
        G_ASSERT_OPENGL

        delete cube;

        // Render the frame.
        float scale_corner=1.8f,scale_length=1.5f;
        math::Vec3f r(scale_corner,0,0),t(0,scale_corner,0),l(0,0,scale_corner),n;

        glBegin(GL_LINE_LOOP);

        glVertex3fv(r);

        n=(~((t-r)^normal))*scale_length;
        glVertex3fv(n+r);
        glVertex3fv(n+t);

        glVertex3fv(t);

        n=(~((l-t)^normal))*scale_length;
        glVertex3fv(n+t);
        glVertex3fv(n+l);

        glVertex3fv(l);

        n=(~((r-l)^normal))*scale_length;
        glVertex3fv(n+l);
        glVertex3fv(n+r);

        glEnd();
        G_ASSERT_OPENGL

        glPopAttrib();
        glPopAttrib();
        glPopAttrib();
        G_ASSERT_OPENGL

        // Complete the display list.
        glEndList();
        G_ASSERT_OPENGL
    }

    return list;
}

} // namespace wrapgl

} // namespace gale
