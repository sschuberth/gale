/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
 *
 * Copyright (C) 2005-2008  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
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

#include "gale/model/mesh.h"

#include "gale/global/platform.h"

using namespace gale::math;

namespace gale {

namespace model {

void Mesh::Renderer::compile(Mesh const* mesh)
{
    m_mesh=mesh;
    if (!m_mesh) {
        return;
    }

    // Clear all indices as they will be rebuilt now.
    m_points.clear();
    m_lines.clear();
    m_triangles.clear();
    m_quads.clear();
    m_polygons.clear();

    m_normals.setSize(m_mesh->vertices.getSize());
    memset(m_normals,0,m_normals.getSize()*sizeof(VectorArray::Type));

    for (int vi=0;vi<m_mesh->vertices.getSize();++vi) {
        IndexArray const& vn=m_mesh->neighbors[vi];
        Vec3f const& v=m_mesh->vertices[vi];

        if (vn.getSize()==0) {
            // This is just a point with an empty neighborhood.
            m_points.insert(vi);
            continue;
        }

        if (vn.getSize()==1) {
            // This is just a line with a single neighbor.
            m_lines.insert(vi);
            m_lines.insert(vn[0]);
            continue;
        }

        for (int n=0;n<vn.getSize();++n) {
            // Get the orbit vertices starting with the edge from vi to its
            // currently selected neighbor.
            IndexArray polygon;
            int o=m_mesh->orbit(vi,vn[n],polygon);
            if (o<3 || o>5) {
                // Only triangular to pentagonal faces are supported right now.
                continue;
            }

            // Make sure to walk each face only once, i.e. rule out permutations
            // of face indices. Use the address in memory to define a relation
            // on the universe of vertices.
            Vec3f const& a=m_mesh->vertices[polygon[1]];
            Vec3f const& b=m_mesh->vertices[polygon[2]];
            if (&v<&a || &v<&b) {
                continue;
            }

            // The "normal's" length equals the spanned parallelogram's area.
            // While this method is always correct for triangular faces, for
            // other faces it depends on the order of traversal and which of the
            // face vertices are used for calculation. This is accepted for
            // performance reasons. To fix it, normalize the "normal" here.
            Vec3f normal=(a-v)^(b-v);

            if (o==3) {
                m_triangles.insert(polygon);

                // Accumulate the "normal" for all face vertices.
                m_normals[polygon[0]]+=normal;
                m_normals[polygon[1]]+=normal;
                m_normals[polygon[2]]+=normal;
            }
            else {
                // More than 3 vertices require another check.
                Vec3f const& c=m_mesh->vertices[polygon[3]];
                if (&v<&c) {
                    continue;
                }

                if (o==4) {
                    m_quads.insert(polygon);

                    // Accumulate the "normal" for all face vertices.
                    m_normals[polygon[0]]+=normal;
                    m_normals[polygon[1]]+=normal;
                    m_normals[polygon[2]]+=normal;
                    m_normals[polygon[3]]+=normal;
                }
                else {
                    // More than 4 vertices require another check.
                    Vec3f const& d=m_mesh->vertices[polygon[4]];
                    if (&v<&d) {
                        continue;
                    }

                    // Note: For more than 5 vertices more checks are needed.
                    m_polygons.insert(polygon);

                    // Accumulate the "normal" for all face vertices.
                    m_normals[polygon[0]]+=normal;
                    m_normals[polygon[1]]+=normal;
                    m_normals[polygon[2]]+=normal;
                    m_normals[polygon[3]]+=normal;
                    m_normals[polygon[4]]+=normal;
                }
            }
        }
    }

    // Normalize the accumulated face "normals".
    for (int i=0;i<m_normals.getSize();++i) {
        m_normals[i].normalize();
    }
}

void Mesh::Renderer::render()
{
    if (!m_mesh) {
        return;
    }

    // Set-up the arrays to be indexed.
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3,GL_FLOAT,0,m_mesh->vertices);
    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT,0,m_normals);

    // Render the different indexed primitives, if any.
    glDrawElements(GL_POINTS,m_points.getSize(),GL_UNSIGNED_INT,m_points);
    glDrawElements(GL_LINES,m_lines.getSize(),GL_UNSIGNED_INT,m_lines);
    glDrawElements(GL_TRIANGLES,m_triangles.getSize(),GL_UNSIGNED_INT,m_triangles);
    glDrawElements(GL_QUADS,m_quads.getSize(),GL_UNSIGNED_INT,m_quads);

    // As polygons do not have a fixed number of vertices, each one has its own
    // index array instead of a single array for all the primitive's vertices.
    for (int i=0;i<m_polygons.getSize();++i) {
        glDrawElements(GL_POLYGON,m_polygons[i].getSize(),GL_UNSIGNED_INT,m_polygons[i]);
    }
}

} // namespace model

} // namespace gale
