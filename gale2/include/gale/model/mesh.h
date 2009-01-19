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

#ifndef MESH_H
#define MESH_H

/**
 * \file
 * Mesh management related classes
 */

#include "../global/dynamicarray.h"
#include "../math/vector.h"

namespace gale {

namespace model {

/**
 * A vertex-vertex mesh data structure implementation as described at
 * <http://en.wikipedia.org/wiki/Polygon_mesh#Vertex-Vertex_Meshes>.
 */
struct Mesh
{
    /// Array of vectors as usable by OpenGL to specify vertices.
    typedef global::DynamicArray<math::Vec3f> VectorArray;

    /// Array of vertex indices as usable by OpenGL.
    typedef global::DynamicArray<unsigned int> IndexArray;

    /// Array of arrays to store vertex neighbors or polygon indices.
    typedef global::DynamicArray<IndexArray> IndexTable;

    /// Forward declaration for use by inner classes.
    class Preparer;

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

        /// Returns a torus with outer radius \a r1 and inner radius \a r2. The
        /// outer and inner circles are divided into \a r1_segs and \a r2_segs
        /// segments respectively.
        static Mesh* Torus(float r1,float r2,int r1_segs,int r2_segs);

        /// Returns a torus knot with outer radius \a r1 and inner radius \a r2.
        /// The outer and inner circles are consist of \a r1_segs and \a r2_segs
        /// segments respectively. \a w and \a h specify the width and height of
        /// the whole knot, which is obtained by looping through the hole \a p
        /// times with \a q revolutions before joining its ends, where \a p and
        /// \a q have to be relatively prime.
        static Mesh* TorusKnot(float r1,float r2,int r1_segs,int r2_segs,float w,float h,int p,int q);

        /**
         * \name Helper and debug meshes
         * Factory methods to create meshes that are not particularly useful
         * on their own, but are used by other factory methods or for debugging.
         */
        //@{

        /// Generates a mesh by extruding the line loop defined by \a profile
        /// along the given \a path. If \a close is \c true, the end of the path
        /// is connected to its beginning, else cut faces are created.
        static Mesh* Extrude(VectorArray const& path,VectorArray const& profile,bool closed=true);

        /// Generates a mesh consisting of lines only that represent the
        /// compiled mesh's vertex normals stored in the \a renderer, optionally
        /// with the given \a scale applied.
        static Mesh* Normals(Preparer const& geom,float scale=1.0f);

        //@}

      protected:

        /// Makes all vertices in the given \a mesh neighbors that have the
        /// specified \a distance, up to the specified vertex \a valence.
        static void populateNeighborhood(Mesh* mesh,float distance,int valence);
    };

    /// Inner class to subdivide meshes using different algorithms, for an
    /// overview see <http://en.wikipedia.org/wiki/Subdivision_surface>.
    class Subdivider
    {
      public:

        /// Function pointer type definition for subdivision schemes.
        typedef void (*Scheme)(Mesh& mesh,int steps);

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

        /// Divides the triangular faces of a mesh as described by C. T. Loop in
        /// <http://research.microsoft.com/~cloop/thesis.pdf>.
        static void Loop(Mesh& mesh,int steps,bool move);

        /// Convenience wrapper for use with a function pointer that calls
        /// Loop() with \a move set to \c true.
        static void Loop(Mesh& mesh,int steps=1) {
            Loop(mesh,steps,true);
        }

        /// Divides the triangular faces of a mesh as described by L. Kobbelt in
        /// <http://www.graphics.rwth-aachen.de/uploads/media/sqrt3.pdf>.
        static void Sqrt3(Mesh& mesh,int steps,bool move);

        /// Convenience wrapper for use with a function pointer that calls
        /// Sqrt3() with \a move set to \c true.
        static void Sqrt3(Mesh& mesh,int steps=1) {
            Sqrt3(mesh,steps,true);
        }

        /// Divides the quadrangular faces of a mesh as described by E. Catmull
        /// and J. Clark in <http://www.idi.ntnu.no/~fredrior/files/Catmull-Clark%201978%20Recursively%20generated%20surfaces.pdf>.
        static void CatmullClark(Mesh& mesh,int steps=1);

        /// Divides the faces of a mesh as described by D. Doo and M. Sabin in
        /// <http://www.idi.ntnu.no/~fredrior/files/Doo-Sabin%201978%20Recursive%20division.pdf>.
        static void DooSabin(Mesh& mesh,int steps=1);

        //@}

      protected:

        /// Adds the missing neighbors of newly inserted vertices after a
        /// subdivision step. Starting from vertex index \a x0i, neighbors
        /// missing in the original mesh \a orig are inserted into the given
        /// \a mesh, which usually is a copy of \a orig.
        static void assignNeighbors(Mesh const& orig,Mesh& mesh,int x0i);
    };

    /// Inner class to prepare a mesh for rendering.
    class Preparer
    {
      public:

        /// Constructor that simply initializes the mesh to render to NULL.
        Preparer()
        :   m_mesh(NULL) {}

        /// Generates the primitive index arrays from the mesh data structure
        /// and calculates vertex normals from averaged face normals.
        void compile(Mesh const* mesh);

        /// Returns a pointer to the last compiled mesh.
        Mesh const* getCompiledMesh() const {
            return m_mesh;
        }

        IndexArray points;    ///< Array of vertex indices describing points.
        IndexArray lines;     ///< Array of vertex indices describing lines.
        IndexArray triangles; ///< Array of vertex indices describing triangles.
        IndexArray quads;     ///< Array of vertex indices describing quadrilaterals.

        IndexTable polygons;  ///< Table of vertex indices describing polygons.

        VectorArray normals;  ///< Array of vertex normals.

      protected:

        Mesh const* m_mesh;     ///< Reference to the mesh to render.
    };

    /// Creates a mesh with \a num_vertices uninitialized vertices.
    Mesh(int num_vertices=0)
    :   vertices(num_vertices),neighbors(num_vertices) {}

    /// Creates a mesh, copying the vertices from the given dynamic \a vertex_array.
    Mesh(VectorArray const& vertex_array) {
        vertices.insert(vertex_array);
        neighbors.setSize(vertex_array.getSize());
    }

    /// Creates a mesh, copying the vertices from the given static \a vertex_array.
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
    /// vertices, and their indices in \a polygon.
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

    /// Performs a simple brute-force check of the neighborhood information.
    /// Returns -1 on success or the vertex index causing the error.
    int check() const;

    VectorArray vertices; ///< Array of vertex positions.
    IndexTable neighbors; ///< Array of arrays of neighboring vertex indices.
};

} // namespace model

} // namespace gale

#endif // MESH_H
