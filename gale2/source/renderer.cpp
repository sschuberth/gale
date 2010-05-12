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

#include "gale/wrapgl/renderer.h"

#include "gale/global/platform.h"

using namespace gale::math;
using namespace gale::model;

namespace gale {

namespace wrapgl {

void Renderer::draw(PreparedMesh const& prep)
{
    if (!prep.getMesh()) {
        return;
    }

#ifdef GALE_USE_VBO
    if (prep.vao.isValidHandle()) {
        prep.vao.makeCurrent();
    }

    if (!prep.vao.isDirtyState() || !prep.vao.isValidHandle()) {
        // Set-up the arrays to be indexed.
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        G_ASSERT_OPENGL

        prep.vbo_vertnorm.makeCurrent();

        Mesh::VectorArray::Type const* arrays_ptr=NULL;
        glVertexPointer(3,GL_FLOAT,0,arrays_ptr);
        G_ASSERT_OPENGL

        arrays_ptr+=prep.getMesh()->vertices.getSize();
        glNormalPointer(GL_FLOAT,0,arrays_ptr);
        G_ASSERT_OPENGL

        prep.vbo_indices.makeCurrent();

        // Mark the Vertex Array Object as consistent.
        prep.vao.setDirtyState(true);
    }

    Mesh::IndexArray::Type const* indices_ptr=NULL;
#else
    // Set-up the arrays to be indexed.
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    G_ASSERT_OPENGL

    glVertexPointer(3,GL_FLOAT,0,prep.getMesh()->vertices);
    glNormalPointer(GL_FLOAT,0,prep.normals);
    G_ASSERT_OPENGL
#endif

    // Render the different indexed primitives, if any.
    for (int i=0;i<G_ARRAY_LENGTH(PreparedMesh::GL_PRIM_TYPE);++i) {
        if (prep.indices[i].getSize()>0) {
#ifdef GALE_USE_VBO
            glDrawElements(PreparedMesh::GL_PRIM_TYPE[i],prep.indices[i].getSize(),GL_UNSIGNED_INT,indices_ptr);
            indices_ptr+=prep.indices[i].getSize();
#else
            glDrawElements(PreparedMesh::GL_PRIM_TYPE[i],prep.indices[i].getSize(),GL_UNSIGNED_INT,prep.indices[i]);
#endif
            G_ASSERT_OPENGL
        }
    }

    // As polygons do not have a fixed number of vertices, each one has its own
    // index array instead of a single array for all the primitive's vertices.
    for (int i=0;i<prep.polygons.getSize();++i) {
        if (prep.polygons[i].getSize()>0) {
#ifdef GALE_USE_VBO
            glDrawElements(GL_POLYGON,prep.polygons[i].getSize(),GL_UNSIGNED_INT,indices_ptr);
            indices_ptr+=prep.polygons[i].getSize();
#else
            glDrawElements(GL_POLYGON,prep.polygons[i].getSize(),GL_UNSIGNED_INT,prep.polygons[i]);
#endif
            G_ASSERT_OPENGL
        }
    }

#ifdef GALE_USE_VBO
    if (prep.vao.isValidHandle()) {
        // Make sure no other changes accidently modify the state vector.
        prep.vao.setCurrent(0);
    }
#endif
}

void Renderer::draw(AABB const& box)
{
    // Get the boxes' corner vertices.
    AABB::Vertices v;
    box.vertices(v);

    // "Back" face outline.
    glBegin(GL_LINE_LOOP);
        glVertex3fv(v[0]);
        glVertex3fv(v[4]);
        glVertex3fv(v[6]);
        glVertex3fv(v[2]);
    glEnd();
    G_ASSERT_OPENGL

    // "Front" face outline.
    glBegin(GL_LINE_LOOP);
        glVertex3fv(v[1]);
        glVertex3fv(v[5]);
        glVertex3fv(v[7]);
        glVertex3fv(v[3]);
    glEnd();
    G_ASSERT_OPENGL

    // Lines connecting "back" and "front" faces.
    glBegin(GL_LINES);
        glVertex3fv(v[0]);
        glVertex3fv(v[1]);

        glVertex3fv(v[4]);
        glVertex3fv(v[5]);

        glVertex3fv(v[6]);
        glVertex3fv(v[7]);

        glVertex3fv(v[2]);
        glVertex3fv(v[3]);
    glEnd();
    G_ASSERT_OPENGL
}

void Renderer::draw(Camera const& camera)
{
    double t=::tan(camera.getFOV()/2);

    // Get the modelview transformation and aspect ratio.
    HMat4f const& m=camera.getModelview();
    float r=static_cast<float>(camera.getScreenSpace().width)/camera.getScreenSpace().height;

    // Calculate the half clipping plane heights.
    float height_n=static_cast<float>(t*camera.getNearClipping());
    float height_f=static_cast<float>(t*camera.getFarClipping());

    // Calculate the spanning vectors on the clipping planes that point from the
    // plane center to the right / top edges.
    Vec3f span_w_n=m.getRightVector()*height_n*r;
    Vec3f span_h_n=m.getUpVector()*height_n;

    Vec3f span_w_f=m.getRightVector()*height_f*r;
    Vec3f span_h_f=m.getUpVector()*height_f;

    // Calculate the diagonal vectors on the clipping planes that point from the
    // plane center to the right side top / bottom corners.
    Vec3f diag_p_n=span_w_n+span_h_n;
    Vec3f diag_m_n=span_w_n-span_h_n;

    Vec3f diag_p_f=span_w_f+span_h_f;
    Vec3f diag_m_f=span_w_f-span_h_f;

    // Calculate the vectors from the camera's position to the clipping plane centers.
    Vec3f dist_n=m.getBackwardVector()*static_cast<float>(-camera.getNearClipping())+camera.getPosition();
    Vec3f dist_f=m.getBackwardVector()*static_cast<float>(-camera.getFarClipping())+camera.getPosition();

    // Put the vertices into an array.
    Vec3f cone[]={
        dist_n - diag_p_n, dist_n + diag_m_n, dist_n + diag_p_n, dist_n - diag_m_n
    ,   dist_f - diag_p_f, dist_f + diag_m_f, dist_f + diag_p_f, dist_f - diag_m_f
    };

    // Draw the clipping plane outlines.
    glBegin(GL_LINE_STRIP);
        glVertex3fv(cone[0]);
        glVertex3fv(cone[1]);
        glVertex3fv(cone[2]);
        glVertex3fv(cone[3]);

        glVertex3fv(cone[0]);
        glVertex3fv(cone[4]);

        glVertex3fv(cone[5]);
        glVertex3fv(cone[6]);
        glVertex3fv(cone[7]);
        glVertex3fv(cone[4]);
    glEnd();
    G_ASSERT_OPENGL

    // Draw the remaining frustum lines.
    glBegin(GL_LINES);
        glVertex3fv(cone[1]);
        glVertex3fv(cone[5]);

        glVertex3fv(cone[2]);
        glVertex3fv(cone[6]);

        glVertex3fv(cone[3]);
        glVertex3fv(cone[7]);
    glEnd();
    G_ASSERT_OPENGL
}

} // namespace wrapgl

} // namespace gale
