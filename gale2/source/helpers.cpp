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

#include "gale/wrapgl/helpers.h"

#include "gale/math/color.h"

namespace gale {

namespace wrapgl {

void Helper::pushOrtho2D()
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

void Helper::popOrtho2D()
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

Logo::Logo(double const fov,float const distance)
:   m_camera(NULL,fov)
,   m_cube(model::Mesh::Factory::Hexahedron())
{
    math::Vec3f const pos(distance,distance,distance);

    m_modelview=math::HMat4f::Factory::LookAt(math::Vec3f::ZERO(),pos,math::Vec3f::Y());
    m_camera.setModelview(m_modelview);

    m_list_range=glGenLists(LIST_COUNT);
    G_ASSERT_OPENGL

    // Compile the prologue display list.
    glNewList(m_list_range+LIST_PROLOGUE,GL_COMPILE);
    G_ASSERT_OPENGL

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
    glDisable(GL_TEXTURE_1D);
    glDisable(GL_TEXTURE_2D);
    G_ASSERT_OPENGL

    glColor3fv(math::Col3f::WHITE());
    G_ASSERT_OPENGL

    glEndList();
    G_ASSERT_OPENGL

    // Compile the epilogue display list.
    glNewList(m_list_range+LIST_EPILOGUE,GL_COMPILE);
    G_ASSERT_OPENGL

    glPopAttrib();
    glPopAttrib();
    glPopAttrib();
    G_ASSERT_OPENGL

    glEndList();
    G_ASSERT_OPENGL

    // Compile the frame display list.
    glNewList(m_list_range+LIST_FRAME,GL_COMPILE);
    G_ASSERT_OPENGL

    float const scale_corner=1.8f,scale_length=1.5f;
    math::Vec3f const r(scale_corner,0,0),t(0,scale_corner,0),l(0,0,scale_corner);
    math::Vec3f n;

    glBegin(GL_LINE_LOOP);

    glVertex3fv(r);

    n=(~((t-r)^pos))*scale_length;
    glVertex3fv(n+r);
    glVertex3fv(n+t);

    glVertex3fv(t);

    n=(~((l-t)^pos))*scale_length;
    glVertex3fv(n+t);
    glVertex3fv(n+l);

    glVertex3fv(l);

    n=(~((r-l)^pos))*scale_length;
    glVertex3fv(n+l);
    glVertex3fv(n+r);

    glEnd();
    G_ASSERT_OPENGL

    glEndList();
    G_ASSERT_OPENGL

    // Prepare the mesh.
    m_cube_prep.compile(m_cube);
}

void Logo::draw(Corner corner)
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);

    GLint size=math::min(viewport[2],viewport[3])/6;

    // Adjust the camera's screen space origin.
    GLint x=viewport[0],y=viewport[1];

    if (corner==CORNER_LR || corner==CORNER_UR) {
        x+=viewport[2]-size;
    }

    if (corner==CORNER_UL || corner==CORNER_UR) {
        y+=viewport[3]-size;
    }

    if (m_camera.getScreenSpace().x!=x || m_camera.getScreenSpace().y!=y) {
        m_camera.setScreenSpaceOrigin(x,y);
    }

    // Adjust the camera's screen space dimensions.
    if (m_camera.getScreenSpace().width!=size || m_camera.getScreenSpace().height!=size) {
        m_camera.setScreenSpaceDimensions(size,size);
        m_camera.setProjection(
            math::Mat4d::Factory::PerspectiveProjection(
                size,size,m_camera.getFOV(),m_camera.getNearClipping(),m_camera.getFarClipping()
            )
        );
    }

    m_camera.makeCurrent();

    // Render the logo.
    glCallList(m_list_range+LIST_PROLOGUE);
    G_ASSERT_OPENGL

    glCallList(m_list_range+LIST_FRAME);
    G_ASSERT_OPENGL

    glTranslatef(1,0,0);
    Renderer::draw(m_cube_prep);

    glTranslatef(-1,1,0);
    Renderer::draw(m_cube_prep);

    glTranslatef(0,-1,1);
    Renderer::draw(m_cube_prep);

    // Reset the transformation.
    glTranslatef(0,0,-1);

    glCallList(m_list_range+LIST_EPILOGUE);
    G_ASSERT_OPENGL
}

Logo::~Logo ()
{
    delete m_cube;

    glDeleteLists(m_list_range,LIST_COUNT);
    G_ASSERT_OPENGL
}

} // namespace wrapgl

} // namespace gale
