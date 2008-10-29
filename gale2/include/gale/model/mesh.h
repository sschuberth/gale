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

#ifndef MESH_H
#define MESH_H

#include "../math/vector.h"

#include "../global/dynamicarray.h"

namespace gale {

namespace model {

/**
 * A vertex-vertex mesh data structure implementation as described at
 * <http://en.wikipedia.org/wiki/Polygon_mesh#Vertex-Vertex_Meshes>.
 */
struct Mesh
{
    /// Array of vertices as usable by OpenGL.
    typedef global::DynamicArray<math::Vec3f> VertexArray;

    /// Array of vertex indices as usable by OpenGL.
    typedef global::DynamicArray<unsigned int> IndexArray;

    /// Array of arrays to store vertex neighbors or polygon indices.
    typedef global::DynamicArray<IndexArray> IndexTable;

    /// Inner factory class to create some predefined meshes.
    class Factory
    {
      public:

        /**
         * \name Platonic Solids
         * Factory methods to create the five Platonic Solids, each with unit
         * edge length, see <http://mathworld.wolfram.com/PlatonicSolid.html>.
         */
        //@{

        /// Returns a Tetrahedron consisting of 4 vertices, 6 edges and 4
        /// triangle faces.
        static Mesh* Tetrahedron();

        /// Returns a Octahedron consisting of 6 vertices, 12 edges and 8
        /// triangle faces.
        static Mesh* Octahedron();

        /// Returns a Hexahedron consisting of 8 vertices, 12 edges and 6
        /// quadrilateral faces.
        static Mesh* Hexahedron();

        /// Returns a Icosahedron consisting of 12 vertices, 30 edges and 20
        /// triangle faces.
        static Mesh* Icosahedron();

        /// Returns a Dodecahedron consisting of 20 vertices, 30 edges and 12
        /// pentagonal faces.
        static Mesh* Dodecahedron();

        //@}

      protected:

        /// Makes all vertices in the given \a mesh neighbors that have the
        /// specified \a distance, up to the specified vertex \a valence.
        static void populateNeighborhood(Mesh* mesh,float distance,int valence);
    };

    /// Inner class to subdivide meshes using different algorithms.
    class Subdivider
    {
      public:

        /**
         * \name Interpolating schemes
         * This type of subdivision does not modify the vertices of the base
         * mesh. Newly generated vertices are interpolated on the limit surface.
         */
        //@{

        /// Divides the triangular faces of a mesh into further triangles.
        static void Polyhedral(Mesh& mesh,int steps=1);

        /// Divides the triangular faces of a mesh as described by Dyn et al. in
        /// <http://www.math.tau.ac.il/~niradyn/papers/butterfly.pdf>.
        static void Butterfly(Mesh& mesh,int steps=1);

        //@}

        /**
         * \name Approximating schemes
         * This type of subdivision modifies both the vertices of the base mesh
         * and newly generated vertices to approximate the limit surface.
         */
        //@{

        /// Divides the triangular faces of a mesh as described by Charles
        /// Teorell Loop in <http://research.microsoft.com/~cloop/thesis.pdf>.
        static void Loop(Mesh& mesh,int steps=1,bool move=true);

        /// Divides the triangular faces of a mesh as described by Leif Kobbelt
        /// in <http://www.graphics.rwth-aachen.de/uploads/media/sqrt3.pdf>.
        static void Sqrt3(Mesh& mesh,int steps=1,bool move=true);

        //@}

      protected:

        /// Adds the missing neighbors of newly inserted vertices after a
        /// subdivision step. Starting from vertex index \a x0i, neighbors
        /// missing in the original mesh \a orig are inserted into the given
        /// \a mesh, which usually is a copy of \a orig.
        static void assignNeighbors(Mesh const& orig,Mesh& mesh,int x0i);
    };

    /// Inner class to prepare / render meshes for / using OpenGL.
    class Renderer
    {
      public:

        /// Constructor that simply stores a reference to the mesh to render.
        Renderer(Mesh* const& mesh)
        :   mesh(mesh) {}

        /// Generates the primitive index arrays from the mesh data structure.
        void compile();

        /// Renders the mesh primitives using OpenGL.
        void render();

      protected:

        Mesh* const& mesh; ///< Reference to the mesh to render.

        IndexArray triangles; ///< Array of vertex indices describing triangles.
        IndexArray quads; ///< Array of vertex indices describing quadrilaterals.

        IndexTable polygons;  ///< Table of vertex indices describing polygons.
    };

    /// Creates a mesh with \a num_vertices uninitialized vertices.
    Mesh(int num_vertices=0)
    :   vertices(num_vertices),neighbors(num_vertices) {}

    /// Creates a mesh, copying the vertices from the static \a vertex_array.
    template<size_t size>
    Mesh(math::Vec3f const (&vertex_array)[size]) {
        vertices.insert(vertex_array);
        neighbors.setSize(size);
    }

    /**
     * \name Selection operations
     */
    //@{

    /// Returns the index of the vertex following \a xi in the neighborhood of
    /// \a vi, both given as indices into the vertex array. Optionally, this is
    /// repeated the given number of \a steps.
    int nextTo(int xi,int vi,int steps=1) const;

    /// Returns the index of the vertex preceding \a xi in the neighborhood of
    /// \a vi, both given as indices into the vertex array. Optionally, this is
    /// repeated the given number of \a steps.
    int prevTo(int xi,int vi,int steps=1) const;

    /// Given an oriented edge from \a ai to \a bi, returns the number of
    /// vertices and their indices in \a polygon.
    int orbit(int ai,int bi,IndexArray& polygon) const;

    //@}

    /**
     * \name Editing operations
     */
    //@{

    /// Inserts a new vertex \a x on the edge between \a ai and \a bi and
    /// returns its index in the vertex array.
    int insert(int ai,int bi,math::Vec3f const& x);

    /// Inserts \a ai after or before \a xi in the neighborhood of \a vi.
    void splice(int ai,int xi,int vi,bool after=true);

    /// Removes \a xi from the neighborhood of \a vi.
    void erase(int xi,int vi);

    //@}

    VertexArray vertices; ///< Array of vertices in the mesh.
    IndexTable neighbors; ///< Array of arrays of neighboring vertex indices.
};

} // namespace model

} // namespace gale

#endif // MESH_H
