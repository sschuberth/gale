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

void Renderer::draw(Mesh::Preparer const& geom)
{
    if (!geom.getMesh()) {
        return;
    }

    // Set-up the arrays to be indexed.
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3,GL_FLOAT,0,geom.getMesh()->vertices);
    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT,0,geom.normals);

    // Render the different indexed primitives, if any.
    glDrawElements(GL_POINTS,geom.points.getSize(),GL_UNSIGNED_INT,geom.points);
    glDrawElements(GL_LINES,geom.lines.getSize(),GL_UNSIGNED_INT,geom.lines);
    glDrawElements(GL_TRIANGLES,geom.triangles.getSize(),GL_UNSIGNED_INT,geom.triangles);
    glDrawElements(GL_QUADS,geom.quads.getSize(),GL_UNSIGNED_INT,geom.quads);

    // As polygons do not have a fixed number of vertices, each one has its own
    // index array instead of a single array for all the primitive's vertices.
    for (int i=0;i<geom.polygons.getSize();++i) {
        glDrawElements(GL_POLYGON,geom.polygons[i].getSize(),GL_UNSIGNED_INT,geom.polygons[i]);
    }
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

    // "Front" face outline.
    glBegin(GL_LINE_LOOP);
        glVertex3fv(v[1]);
        glVertex3fv(v[5]);
        glVertex3fv(v[7]);
        glVertex3fv(v[3]);
    glEnd();

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
}

void Renderer::draw(Camera const& camera)
{
    double t=::tan(camera.getFOV()/2);

    HMat4f const& m=camera.getModelview();
    float r=static_cast<float>(camera.getScreenSpace().width)/camera.getScreenSpace().height;

    // Calculate x- and y-offsets from the look direction on the near plane.
    float near_s=static_cast<float>(t*camera.getNearClipping());
    Vec3f near_y=m.getUpVector()*near_s;
    Vec3f near_x=m.getRightVector()*near_s*r;

    Vec3f near_d=~m.getBackwardVector()*static_cast<float>(-camera.getNearClipping())+camera.getPosition();

    Vec3f near_a=near_x+near_y;
    Vec3f near_b=near_x-near_y;

    // Calculate x- and y-offsets from the look direction on the far plane.
    float far_s=static_cast<float>(t*camera.getFarClipping());
    Vec3f far_y=m.getUpVector()*far_s;
    Vec3f far_x=m.getRightVector()*far_s*r;

    Vec3f far_d=~m.getBackwardVector()*static_cast<float>(-camera.getFarClipping())+camera.getPosition();

    Vec3f far_a=far_x+far_y;
    Vec3f far_b=far_x-far_y;

    // Put the vertices into an array.
    Vec3f cone[]={
        near_d - near_a , near_d + near_b , near_d + near_a , near_d - near_b
    ,   far_d  - far_a  , far_d  + far_b  , far_d  + far_a  , far_d  - far_b
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

    // Draw the remaining frustum lines.
    glBegin(GL_LINES);
        glVertex3fv(cone[1]);
        glVertex3fv(cone[5]);

        glVertex3fv(cone[2]);
        glVertex3fv(cone[6]);

        glVertex3fv(cone[3]);
        glVertex3fv(cone[7]);
    glEnd();
}

} // namespace wrapgl

} // namespace gale
