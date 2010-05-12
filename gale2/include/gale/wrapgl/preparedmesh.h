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

#ifndef PREPAREDMESH_H
#define PREPAREDMESH_H

/**
 * \file
 * Mesh rendering management classes
 */

#include "../model/mesh.h"

#include "vertexarrayobject.h"
#include "vertexbufferobject.h"

namespace gale {

namespace wrapgl {

/**
 * A class to prepare a mesh for optimal rendering on the GPU.
 */
class PreparedMesh
{
  public:

    /// Generates the primitive index arrays from the mesh data structure
    /// and calculates vertex normals from averaged face normals.
    void compile(model::Mesh const& mesh);

    /// Returns whether the mesh contains vertex data.
    bool hasData() const {
#ifdef GALE_USE_VBO
        return vbo_vertnorm.getSize()>0;
#else
        return vertices->getSize()>0;
#endif
    }

    // Returns the number of vertices in the mesh.
    int numVertices() const {
#ifdef GALE_USE_VBO
        return vbo_vertnorm.getSize()/2;
#else
        return vertices->getSize();
#endif
    }

    // Returns the number of normals in the mesh.
    int numNormals() const {
#ifdef GALE_USE_VBO
        return vbo_vertnorm.getSize()/2;
#else
        return normals.getSize();
#endif
    }

    /// Locks the vertices for direct access and returns a pointer to them.
    model::Mesh::VectorArray::Type* lockVertices(GLenum access=GL_READ_ONLY_ARB) {
#ifdef GALE_USE_VBO
        return reinterpret_cast<model::Mesh::VectorArray::Type*>(vbo_vertnorm.map(access));
#else
        G_UNREF_PARAM(access)
        return vertices->data();
#endif
    }

    /// Unlocks the vertices, disabling direct access to them.
    void unlockVertices() const {
#ifdef GALE_USE_VBO
        vbo_vertnorm.unmap();
#endif
    }

    /// Locks the normals for direct access and returns a pointer to them.
    model::Mesh::VectorArray::Type* lockNormals(GLenum access=GL_READ_ONLY_ARB) {
#ifdef GALE_USE_VBO
        return reinterpret_cast<model::Mesh::VectorArray::Type*>(vbo_vertnorm.map(access)+vbo_vertnorm.getSize()/2);
#else
        G_UNREF_PARAM(access)
        return normals;
#endif
    }

    /// Unlocks the normals, disabling direct access to them.
    void unlockNormals() const {
#ifdef GALE_USE_VBO
        vbo_vertnorm.unmap();
#endif
    }

    /// Returns a pointer to the vertices as suitable for glVertexPointer().
    model::Mesh::VectorArray::Type const* vertexPointer() const {
#ifdef GALE_USE_VBO
        vbo_vertnorm.makeCurrent();
        return reinterpret_cast<model::Mesh::VectorArray::Type const*>(0);
#else
        return vertices->data();
#endif
    }

    /// Returns a pointer to the normals as suitable for glNormalPointer().
    model::Mesh::VectorArray::Type const* normalPointer() const {
#ifdef GALE_USE_VBO
        vbo_vertnorm.makeCurrent();
        return reinterpret_cast<model::Mesh::VectorArray::Type const*>(vbo_vertnorm.getSize()/2);
#else
        return normals;
#endif
    }

    /// Returns whether this mesh contains point primitives.
    bool hasPoints() const {
        return indices[PI_POINTS].getSize()>0;
    }

    /// Returns whether this mesh contains line primitives.
    bool hasLines() const {
        return indices[PI_LINES].getSize()>0;
    }

    /// Returns whether this mesh contains triangle primitives.
    bool hasTriangles() const {
        return indices[PI_TRIANGLES].getSize()>0;
    }

    /// Returns whether this mesh contains quadrilateral primitives.
    bool hasQuads() const {
        return indices[PI_QUADS].getSize()>0;
    }

    /// Returns whether this mesh contains polygon primitives.
    bool hasPolygons() const {
        return polygons.getSize()>0;
    }

    model::AABB box; ///< The axis-aligned bounding box.

    /// Names for the primitives stored in the indices table.
    enum PrimitiveIndices {
        PI_POINTS    ///< The first array of indices describes points.
    ,   PI_LINES     ///< The second array of indices describes lines.
    ,   PI_TRIANGLES ///< The third array of indices describes triangles.
    ,   PI_QUADS     ///< The third array of indices describes quadrilaterals.
    ,   PI_COUNT     ///< Special entry to name the number of enum entries.
    };

    static GLenum const GL_PRIM_TYPE[PI_COUNT]; ///< The primitive types as stored in the indices table.

    model::Mesh::IndexTable indices;  ///< Table of vertex indices describing primitives.
    model::Mesh::IndexTable polygons; ///< Table of vertex indices describing polygons.

#ifdef GALE_USE_VBO
    IndexBufferObject vbo_indices; ///< Primitive and polygon indices on the GPU.

    VertexArrayObject vao; ///< Bindable state vector for the render arrays.
#endif

  private:

#ifdef GALE_USE_VBO
    ArrayBufferObject vbo_vertnorm; ///< Vertices and normals on the GPU.
#else
    model::Mesh::VectorArrayPtr vertices; ///< Shared array of vertex positions.
    model::Mesh::VectorArray normals;     ///< Array of vertex normals.
#endif
};

} // namespace wrapgl

} // namespace gale

#endif // PREPAREDMESH_H
