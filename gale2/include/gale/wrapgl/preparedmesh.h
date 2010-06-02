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

#ifdef GALE_USE_VBO
    #include "vertexarrayobject.h"
    #include "vertexbufferobject.h"
#endif

namespace gale {

namespace wrapgl {

/**
 * A class to prepare a mesh for optimal rendering on the GPU.
 */
class PreparedMesh
{
    friend struct Renderer;

  public:

    /// Generates the primitive index arrays from the mesh data structure
    /// and calculates vertex normals from averaged face normals.
    void compile(model::Mesh const& mesh);

    /// Returns whether the mesh contains something to render.
    bool hasData() const {
        return pointCount()>0 || lineCount()>0 || triangleCount()>0 || quadCount()>0 || polygonCount()>0;
    }

    /// Returns the number of vertices in the mesh.
    int numVertices() const {
        return m_vertices.getSize();
    }

    /// Returns the number of normals in the mesh.
    int numNormals() const {
        return m_normals.getSize();
    }

    /// Returns a pointer to the vertices as suitable for reading the data.
    model::Mesh::VectorArray::Type const* vertexAccess() const {
        return m_vertices;
    }

    /// Returns a pointer to the normals as suitable for reading the data.
    model::Mesh::VectorArray::Type const* normalAccess() const {
        return m_normals;
    }

    /// Returns a pointer to the vertices as suitable for glVertexPointer().
    model::Mesh::VectorArray::Type const* vertexPointer() const {
#ifdef GALE_USE_VBO
        return reinterpret_cast<model::Mesh::VectorArray::Type const*>(0);
#else
        return vertexAccess();
#endif
    }

    /// Returns a pointer to the normals as suitable for glNormalPointer().
    model::Mesh::VectorArray::Type const* normalPointer() const {
#ifdef GALE_USE_VBO
        return reinterpret_cast<model::Mesh::VectorArray::Type const*>(m_vbo_vertnorm.getSize()/2);
#else
        return normalAccess();
#endif
    }

    /// Returns the number of points in this mesh.
    int pointCount() const {
        return m_primitives[PI_POINTS].getSize();
    }

    /// Returns the number of lines in this mesh.
    int lineCount() const {
        return m_primitives[PI_LINES].getSize()/2;
    }

    /// Returns the number of triangles in this mesh.
    int triangleCount() const {
        return m_primitives[PI_TRIANGLES].getSize()/3;
    }

    /// Returns the number of quadrilaterals in this mesh.
    int quadCount() const {
        return m_primitives[PI_QUADS].getSize()/4;
    }

    /// Returns the number of polygons in this mesh.
    int polygonCount() const {
        return m_polygons.getSize();
    }

    model::AABB box; ///< The mesh's axis-aligned bounding box.

  private:

    /// Names for the primitives stored in the indices table.
    enum PrimitiveIndices {
        PI_POINTS    ///< The first array of indices describes points.
    ,   PI_LINES     ///< The second array of indices describes lines.
    ,   PI_TRIANGLES ///< The third array of indices describes triangles.
    ,   PI_QUADS     ///< The fourth array of indices describes quadrilaterals.
    ,   PI_COUNT     ///< Special entry to name the number of primitive enum entries.
    };

    /// OpenGL enums for the primitive types.
    static GLenum const GL_PRIM_TYPE[PI_COUNT];

    model::Mesh::VectorArray m_vertices; ///< Array of vertex positions.
    model::Mesh::VectorArray m_normals;  ///< Array of vertex normals.

    model::Mesh::IndexTable m_primitives; ///< Table of vertex indices describing primitives.
    model::Mesh::IndexTable m_polygons;   ///< Table of vertex indices describing polygons.

#ifdef GALE_USE_VBO
    ArrayBufferObject m_vbo_vertnorm; ///< Vertices and normals buffer.
    IndexBufferObject m_vbo_primpoly; ///< Primitive and polygon indices buffer.

    VertexArrayObject m_vao; ///< Bindable state vector for the buffer objects.
#endif
};

} // namespace wrapgl

} // namespace gale

#endif // PREPAREDMESH_H
