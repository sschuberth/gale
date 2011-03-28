/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
 *
 * Copyright (C) 2005-2010  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
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

#include "gale/wrapgl/preparedmesh.h"

using namespace gale::math;
using namespace gale::model;

namespace gale {

namespace wrapgl {

GLenum const PreparedMesh::GL_PRIM_TYPE[PI_COUNT]={
    GL_POINTS
,   GL_LINES
,   GL_TRIANGLES
,   GL_QUADS
};

void PreparedMesh::compile(Mesh const& mesh)
{
    // Get an own copy of the vertices.
    m_vertices=mesh.vertices;

    // Set the number of normals to the number of vertices; even point and line
    // vertices will have normals that equal the vertices' directions.
    m_normals.setSize(m_vertices.getSize());

    // Initialize the normals to 0 so they can be accumulated.
    size_t size=m_vertices.getSize()*sizeof(Mesh::VectorArray::Type);
    memset(m_normals,0,size);

    // Clear all indices as they will be rebuilt now.
    m_primitives.clear();
    m_polygons.clear();

    // Make room for an index array for each primitive type.
    m_primitives.setSize(PI_COUNT);

    // If there are no vertices, empty the bounding box and return immediately.
    if (m_vertices.getSize()<=0) {
        box.min=box.max=Vec3f::ZERO();
        return;
    }

    // Calculate the axis-aligned bounding box.
    box.min=box.max=m_vertices[0];

    for (int vi=0;vi<m_vertices.getSize();++vi) {
        Vec3f const& v=m_vertices[vi];

        if (v.getX()<box.min.getX()) {
            box.min.setX(v.getX());
        }
        else if (v.getX()>box.max.getX()) {
            box.max.setX(v.getX());
        }

        if (v.getY()<box.min.getY()) {
            box.min.setY(v.getY());
        }
        else if (v.getY()>box.max.getY()) {
            box.max.setY(v.getY());
        }

        if (v.getZ()<box.min.getZ()) {
            box.min.setZ(v.getZ());
        }
        else if (v.getZ()>box.max.getZ()) {
            box.max.setZ(v.getZ());
        }
    }

    // Enumerate the primitives for rendering.
    Mesh::PrimitiveIterator prim_current=mesh.beginPrimitives();
    Mesh::PrimitiveIterator prim_end=mesh.endPrimitives();
    while (prim_current!=prim_end) {
        Mesh::IndexArray const& primitive=prim_current.indices();
        int count=primitive.getSize();

        if (count==1) {
            // This is just a point with an empty neighborhood.
            int index=primitive[0];
            m_primitives[PI_POINTS].insert(index);
            m_normals[index]=mesh.vertices[index];
        }
        else if (count==2) {
            // This is just a line with a single neighbor.
            int index=primitive[0];
            m_primitives[PI_LINES].insert(index);
            m_normals[index]=mesh.vertices[index];

            index=primitive[1];
            m_primitives[PI_LINES].insert(index);
            m_normals[index]=mesh.vertices[index];
        }
        else {
            if (count==3) {
                m_primitives[PI_TRIANGLES].insert(primitive);
            }
            else if (count==4) {
                m_primitives[PI_QUADS].insert(primitive);
            }
            else {
                m_polygons.insert(primitive);
            }

            // The face "normal's" length equals the spanned parallelogram's area.
            // While this method is always correct for triangular faces, for
            // other faces it depends on the order of traversal and which of the
            // face vertices are used for calculation. This is accepted for
            // performance reasons. To fix it, normalize the "normal" here.
            Vec3f const& v=mesh.vertices[primitive[0]];
            Vec3f const& a=mesh.vertices[primitive[1]];
            Vec3f const& b=mesh.vertices[primitive[2]];
            Vec3f normal=(a-v)^(b-v);

            for (int i=0;i<count;++i) {
                m_normals[primitive[i]]+=normal;
            }
        }

        ++prim_current;
    }

    // Normalize the accumulated "normals" (there are as many normals as vertices).
    for (int i=0;i<m_vertices.getSize();++i) {
        m_normals[i].normalize();
    }

#ifdef GALE_USE_VBO
    // Allocate buffer object for the vertices and normals.
    m_vbo_vertnorm.setData(GL_STATIC_DRAW_ARB,size*2,NULL);

    // Copy the vertices and normals to the buffer object.
    m_vbo_vertnorm.setData(size,m_vertices);
    m_vbo_vertnorm.setData(size,m_normals,size);

    // Accumulate the sizes of all primitive and polygon index arrays.
    size=0;

    for (int i=0;i<m_primitives.getSize();++i) {
        size+=m_primitives[i].getSize();
    }

    for (int i=0;i<m_polygons.getSize();++i) {
        size+=m_polygons[i].getSize();
    }

    size*=sizeof(Mesh::IndexArray::Type);

    // Allocate buffer object for the primitive and polygon indices.
    m_vbo_primpoly.setData(GL_STATIC_DRAW_ARB,size,NULL);

    // Copy the primitive and polygon indices to the buffer object.
    GLintptrARB offset=0;

    for (int i=0;i<m_primitives.getSize();++i) {
        size=m_primitives[i].getSize()*sizeof(Mesh::IndexArray::Type);
        m_vbo_primpoly.setData(size,m_primitives[i],offset);
        offset+=size;
    }

    for (int i=0;i<m_polygons.getSize();++i) {
        size=m_polygons[i].getSize()*sizeof(Mesh::IndexArray::Type);
        m_vbo_primpoly.setData(size,m_polygons[i],offset);
        offset+=size;
    }

    // Mark the Vertex Array Object as inconsistent.
    m_vao.setDirtyState(true);
#endif
}

} // namespace wrapgl

} // namespace gale
