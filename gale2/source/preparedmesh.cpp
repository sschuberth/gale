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
    // Clear all indices as they will be rebuilt now.
    indices.clear();
    polygons.clear();

    size_t size=mesh.vertices->getSize()*sizeof(Mesh::VectorArray::Type);

#ifdef GALE_USE_VBO
    // Allocate uninitialized GPU memory for the vertices and normals.
    vbo_vertnorm.setData(size*2,NULL);

    // Copy the vertices to the GPU.
    memcpy(lockVertices(GL_READ_WRITE_ARB),mesh.vertices->data(),size);
    unlockVertices();
#else
    // Allocate memory for as many normals as there are vertices.
    normals.setSize(mesh.vertices->getSize());

    // Shallow-copy the vertices.
    vertices=mesh.vertices;
#endif

    Mesh::VectorArray::Type* normals_ptr=lockNormals(GL_READ_WRITE_ARB);

    // Initialize the memory for the normals to 0.
    memset(normals_ptr,0,size);

    if (mesh.vertices->getSize()<=0) {
        box.min=box.max=Vec3f::ZERO();
        return;
    }

    Mesh::VectorArray const& mv=(*mesh.vertices);
    box.min=box.max=mv[0];

    indices.setSize(G_ARRAY_LENGTH(GL_PRIM_TYPE));
    Mesh::IndexArray polygon;

    for (int vi=0;vi<mesh.vertices->getSize();++vi) {
        Mesh::IndexArray const& vn=mesh.neighbors[vi];
        Vec3f const& v=mv[vi];

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
            indices[PI_POINTS].insert(vi);
            continue;
        }

        if (vn.getSize()==1) {
            // This is just a line with a single neighbor.
            indices[PI_LINES].insert(vi);
            indices[PI_LINES].insert(vn[0]);
            continue;
        }

        for (int n=0;n<vn.getSize();++n) {
            // Get the orbit vertices starting with the edge from vi to its
            // currently selected neighbor.
            int o=mesh.orbit(vi,vn[n],polygon);
            if (o<3 || o>5) {
                // Only triangular to pentagonal faces are supported right now.
                continue;
            }

            // Make sure to walk each face only once, i.e. rule out permutations
            // of face indices. Use the address in memory to define a relation
            // on the universe of vertices.
            Vec3f const& a=mv[polygon[1]];
            Vec3f const& b=mv[polygon[2]];
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
                indices[PI_TRIANGLES].insert(polygon);

                // Accumulate the "normal" for all face vertices.
                normals_ptr[polygon[0]]+=normal;
                normals_ptr[polygon[1]]+=normal;
                normals_ptr[polygon[2]]+=normal;
            }
            else {
                // More than 3 vertices require another check.
                Vec3f const& c=mv[polygon[3]];
                if (&v<&c) {
                    continue;
                }

                if (o==4) {
                    indices[PI_QUADS].insert(polygon);

                    // Accumulate the "normal" for all face vertices.
                    normals_ptr[polygon[0]]+=normal;
                    normals_ptr[polygon[1]]+=normal;
                    normals_ptr[polygon[2]]+=normal;
                    normals_ptr[polygon[3]]+=normal;
                }
                else {
                    // More than 4 vertices require another check.
                    Vec3f const& d=mv[polygon[4]];
                    if (&v<&d) {
                        continue;
                    }

                    // NOTE: For more than 5 vertices more checks are needed.
                    polygons.insert(polygon);

                    // Accumulate the "normal" for all face vertices.
                    normals_ptr[polygon[0]]+=normal;
                    normals_ptr[polygon[1]]+=normal;
                    normals_ptr[polygon[2]]+=normal;
                    normals_ptr[polygon[3]]+=normal;
                    normals_ptr[polygon[4]]+=normal;
                }
            }
        }
    }

    // Normalize the accumulated face "normals" (there are as many normals as vertices).
    for (int i=0;i<mesh.vertices->getSize();++i) {
        normals_ptr[i].normalize();
    }

    unlockNormals();

#ifdef GALE_USE_VBO
    // Allocate uninitialized GPU memory for the indices.
    size=0;
    for (int i=0;i<G_ARRAY_LENGTH(GL_PRIM_TYPE);++i) {
        size+=indices[i].getSize();
    }
    for (int i=0;i<polygons.getSize();++i) {
        size+=polygons[i].getSize();
    }
    vbo_indices.setData(size*sizeof(Mesh::IndexArray::Type),NULL);

    // Copy the indices to the GPU.
    Mesh::IndexArray::Type* indices_ptr=reinterpret_cast<Mesh::IndexArray::Type*>(vbo_indices.map(GL_WRITE_ONLY_ARB));

    for (int i=0;i<G_ARRAY_LENGTH(GL_PRIM_TYPE);++i) {
        memcpy(indices_ptr,indices[i].data(),indices[i].getSize()*sizeof(Mesh::IndexArray::Type));
        indices_ptr+=indices[i].getSize();
    }

    for (int i=0;i<polygons.getSize();++i) {
        memcpy(indices_ptr,polygons[i].data(),polygons[i].getSize()*sizeof(Mesh::IndexArray::Type));
        indices_ptr+=polygons[i].getSize();
    }

    vbo_indices.unmap();

    // Mark the Vertex Array Object as inconsistent.
    vao.setDirtyState(false);
#endif
}

} // namespace wrapgl

} // namespace gale
