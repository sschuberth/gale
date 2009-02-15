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

namespace gale {

namespace wrapgl {

void Renderer::draw(model::Mesh::Preparer const& geom)
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

void Renderer::draw(model::AABB const& box)
{
    Vec3f v000(box.min.getX(),box.min.getY(),box.min.getZ());
    Vec3f v001(box.min.getX(),box.min.getY(),box.max.getZ());
    Vec3f v010(box.min.getX(),box.max.getY(),box.min.getZ());
    Vec3f v011(box.min.getX(),box.max.getY(),box.max.getZ());
    Vec3f v100(box.max.getX(),box.min.getY(),box.min.getZ());
    Vec3f v101(box.max.getX(),box.min.getY(),box.max.getZ());
    Vec3f v110(box.max.getX(),box.max.getY(),box.min.getZ());
    Vec3f v111(box.max.getX(),box.max.getY(),box.max.getZ());

    // "Back" face outline.
    glBegin(GL_LINE_LOOP);
        glVertex3fv(v000);
        glVertex3fv(v100);
        glVertex3fv(v110);
        glVertex3fv(v010);
    glEnd();

    // "Front" face outline.
    glBegin(GL_LINE_LOOP);
        glVertex3fv(v001);
        glVertex3fv(v101);
        glVertex3fv(v111);
        glVertex3fv(v011);
    glEnd();

    // Lines connecting "back" and "front" faces.
    glBegin(GL_LINES);
        glVertex3fv(v000);
        glVertex3fv(v001);

        glVertex3fv(v100);
        glVertex3fv(v101);

        glVertex3fv(v110);
        glVertex3fv(v111);

        glVertex3fv(v010);
        glVertex3fv(v011);
    glEnd();
}

} // namespace wrapgl

} // namespace gale
