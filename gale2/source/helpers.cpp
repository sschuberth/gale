/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at https://github.com/sschuberth/gale/.
 *
 * Copyright (C) 2005-2011  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
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

using namespace gale::math;

namespace gale {

namespace wrapgl {

void Helper::pushOrtho2D()
{
    // Set the projection matrix to map each raster position to a viewport pixel.
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);
    G_ASSERT_OPENGL

    pushOrtho2D(viewport[0],viewport[1],viewport[2]-1,viewport[3]-1);
}

void Helper::pushOrtho2D(GLdouble ll_x,GLdouble ll_y,GLdouble ur_x,GLdouble ur_y)
{
    // Save the current matrix mode.
    glPushAttrib(GL_TRANSFORM_BIT);
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

    glOrtho(ll_x,ur_x+1,ll_y,ur_y+1,-1,1);
    G_ASSERT_OPENGL

    // Restore the original matrix mode.
    glPopAttrib();
    G_ASSERT_OPENGL
}

void Helper::popOrtho2D()
{
    // Save the current matrix mode.
    glPushAttrib(GL_TRANSFORM_BIT);
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
    glPopAttrib();
    G_ASSERT_OPENGL
}

Logo::Logo(double const fov,float const distance)
:   m_camera(NULL,fov)
{
    // Prepare the camera for rendering the cubes.
    Vec3f const pos(distance,distance,distance);

    HMat4f modelview=HMat4f::Factory::LookAt(Vec3f::ZERO(),pos,Vec3f::Y());
    m_camera.setModelview(modelview);

    // Prepare the mesh for rendering the cubes.
    model::Mesh* cube=model::Mesh::Factory::Hexahedron();
    m_cube_prep.compile(*cube);
    delete cube;

    // Generate display lists to render various parts of the logo.
    m_list_range=glGenLists(LIST_COUNT);
    G_ASSERT_OPENGL

    // Compile the prologue display list.
    glNewList(m_list_range+LIST_PROLOGUE,GL_COMPILE);
    G_ASSERT_OPENGL

    // Save states that need to be changed for rendering the logo.
    glPushAttrib(GL_CURRENT_BIT);
    glColor3fv(Col3f::WHITE());
    G_ASSERT_OPENGL

    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_1D);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    G_ASSERT_OPENGL

    glPushAttrib(GL_LINE_BIT);
    glLineWidth(1.0f);
    G_ASSERT_OPENGL

    glPushAttrib(GL_POLYGON_BIT);
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    G_ASSERT_OPENGL

    glEndList();
    G_ASSERT_OPENGL

    // Compile the epilogue display list.
    glNewList(m_list_range+LIST_EPILOGUE,GL_COMPILE);
    G_ASSERT_OPENGL

    glPopAttrib();
    glPopAttrib();
    glPopAttrib();
    glPopAttrib();
    G_ASSERT_OPENGL

    glEndList();
    G_ASSERT_OPENGL

    // Compile the display list for rendering the frame.
    glNewList(m_list_range+LIST_FRAME,GL_COMPILE);
    G_ASSERT_OPENGL

    float const scale_corner=1.8f,scale_length=1.5f;
    Vec3f const r(scale_corner,0,0),t(0,scale_corner,0),l(0,0,scale_corner);
    Vec3f n;

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
}

Logo::~Logo()
{
    glDeleteLists(m_list_range,LIST_COUNT);
    G_ASSERT_OPENGL
}

void Logo::draw(Corner corner)
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);

    GLint size=min(viewport[2],viewport[3])/6;

    // Do not render anything if the viewport has not yet been initialized.
    if (size<=0) {
        return;
    }

    // Adjust the camera's screen space origin.
    GLint x=viewport[0],y=viewport[1];

    if (corner==CORNER_LR || corner==CORNER_UR) {
        x+=viewport[2]-size;
    }

    if (corner==CORNER_UL || corner==CORNER_UR) {
        y+=viewport[3]-size;
    }

    // Adjust the camera's screen space (always set it to force applying it).
    m_camera.setScreenSpaceOrigin(x,y);
    m_camera.setScreenSpaceDimensions(size,size);

    m_camera.setProjection(
        Mat4d::Factory::PerspectiveProjection(
            size,size,m_camera.getFOV(),m_camera.getNearClipping(),m_camera.getFarClipping()
        )
    );

    m_camera.apply();

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

    // Restore the original viewport.
    glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);
}

} // namespace wrapgl

} // namespace gale
