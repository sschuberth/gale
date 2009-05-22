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

#include "gale/model/mesh.h"

using namespace gale::math;

namespace gale {

namespace model {

void Mesh::Preparer::compile(Mesh const* mesh)
{
    m_mesh=mesh;
    if (!m_mesh) {
        return;
    }

    // Clear all indices as they will be rebuilt now.
    points.clear();
    lines.clear();
    triangles.clear();
    quads.clear();
    polygons.clear();

    // Allocate uninitialized GPU memory for the vertices and normals.
    wrapgl::GLsizeiptrARB size=m_mesh->vertices.getSize()*sizeof(VectorArray::Type);
    arrays.setData(size*2,NULL);

    // Copy the vertices to the GPU.
    VectorArray::Type* arrays_ptr=static_cast<VectorArray::Type*>(arrays.map(GL_READ_WRITE_ARB));
    memcpy(arrays_ptr,m_mesh->vertices.data(),size);

    // Map the GPU memory for the normals and initialize it to 0.
    arrays_ptr+=m_mesh->vertices.getSize();
    memset(arrays_ptr,0,size);

    if (m_mesh->vertices.getSize()<=0) {
        box.min=box.max=Vec3f::ZERO();
        return;
    }

    box.min=box.max=m_mesh->vertices[0];

    IndexArray polygon;

    for (int vi=0;vi<m_mesh->vertices.getSize();++vi) {
        IndexArray const& vn=m_mesh->neighbors[vi];
        Vec3f const& v=m_mesh->vertices[vi];

        // Update the bounding box extents.
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

        // Check for non-face primitives.
        if (vn.getSize()==0) {
            // This is just a point with an empty neighborhood.
            points.insert(vi);
            continue;
        }

        if (vn.getSize()==1) {
            // This is just a line with a single neighbor.
            lines.insert(vi);
            lines.insert(vn[0]);
            continue;
        }

        for (int n=0;n<vn.getSize();++n) {
            // Get the orbit vertices starting with the edge from vi to its
            // currently selected neighbor.
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
                triangles.insert(polygon);

                // Accumulate the "normal" for all face vertices.
                arrays_ptr[polygon[0]]+=normal;
                arrays_ptr[polygon[1]]+=normal;
                arrays_ptr[polygon[2]]+=normal;
            }
            else {
                // More than 3 vertices require another check.
                Vec3f const& c=m_mesh->vertices[polygon[3]];
                if (&v<&c) {
                    continue;
                }

                if (o==4) {
                    quads.insert(polygon);

                    // Accumulate the "normal" for all face vertices.
                    arrays_ptr[polygon[0]]+=normal;
                    arrays_ptr[polygon[1]]+=normal;
                    arrays_ptr[polygon[2]]+=normal;
                    arrays_ptr[polygon[3]]+=normal;
                }
                else {
                    // More than 4 vertices require another check.
                    Vec3f const& d=m_mesh->vertices[polygon[4]];
                    if (&v<&d) {
                        continue;
                    }

                    // Note: For more than 5 vertices more checks are needed.
                    polygons.insert(polygon);

                    // Accumulate the "normal" for all face vertices.
                    arrays_ptr[polygon[0]]+=normal;
                    arrays_ptr[polygon[1]]+=normal;
                    arrays_ptr[polygon[2]]+=normal;
                    arrays_ptr[polygon[3]]+=normal;
                    arrays_ptr[polygon[4]]+=normal;
                }
            }
        }
    }

    // Normalize the accumulated face "normals".
    for (int i=0;i<m_mesh->vertices.getSize();++i) {
        arrays_ptr[i].normalize();
    }

    arrays.unmap();

    // Allocate uninitialized GPU memory for the indices.
    size=points.getSize()+lines.getSize()+triangles.getSize()+quads.getSize();
    for (int i=0;i<polygons.getSize();++i) {
        size+=polygons[i].getSize();
    }
    indices.setData(size*sizeof(IndexArray::Type),NULL);

    // Copy the indices to the GPU.
    IndexArray::Type* indices_ptr=static_cast<IndexArray::Type*>(indices.map(GL_WRITE_ONLY_ARB));

    memcpy(indices_ptr,points.data(),points.getSize()*sizeof(IndexArray::Type));
    indices_ptr+=points.getSize();

    memcpy(indices_ptr,lines.data(),lines.getSize()*sizeof(IndexArray::Type));
    indices_ptr+=lines.getSize();

    memcpy(indices_ptr,triangles.data(),triangles.getSize()*sizeof(IndexArray::Type));
    indices_ptr+=triangles.getSize();

    memcpy(indices_ptr,quads.data(),quads.getSize()*sizeof(IndexArray::Type));
    indices_ptr+=quads.getSize();

    for (int i=0;i<polygons.getSize();++i) {
        memcpy(indices_ptr,polygons[i].data(),polygons[i].getSize()*sizeof(IndexArray::Type));
        indices_ptr+=polygons[i].getSize();
    }

    indices.unmap();
}

} // namespace model

} // namespace gale
