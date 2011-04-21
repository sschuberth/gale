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

#ifndef MESH_H
#define MESH_H

/**
 * \file
 * Mesh data structure management classes
 */

#include "../global/dynamicarray.h"
#include "../math/formula.h"
#include "../math/hmatrix4.h"

#include "boundingbox.h"

namespace gale {

namespace model {

/**
 * A vertex-vertex mesh data structure implementation as described at
 * <http://en.wikipedia.org/wiki/Polygon_mesh#Vertex-vertex_meshes>.
 */
struct Mesh
{
    /// Array of vectors as usable e.g. by glVertexPointer().
    typedef global::DynamicArray<math::Vec3f> VectorArray;

    /// Array of indices as usable e.g. by glDrawElements().
    typedef global::DynamicArray<unsigned int> IndexArray;

    /// Array of arrays to store vertex neighbors or polygon indices.
    typedef global::DynamicArray<IndexArray> IndexTable;

    /// %Factory class to create procedural meshes.
    class Factory
    {
      public:

        /// Methods to create shapes, e.g. for use as extrusion contours.
        struct Shape
        {
            /// Returns an array of vectors on an ellipse with width \a w and
            /// height \a h, divided into \a segs segments.
            static void Ellipse(VectorArray& shape,int const segs,float const w,float const h);

            /// Returns an array of vectors on a heart shape as published at
            /// <http://iquilezles.org/blog/?p=1181>.
            static void Heart(VectorArray& shape,int const segs);

            /// Returns an array of vectors as calculated by the Superformula,
            /// see <http://local.wasp.uwa.edu.au/~pbourke/geometry/supershape/>.
            static void Supershape(VectorArray& shape,int const segs,float const m,float const n1,float const n2,float const n3,float const a=1.0f,float const b=1.0f);
        };

        /**
         * \name Platonic Solids
         * Methods to create the five Platonic Solids, each with unit edge
         * length, see <http://mathworld.wolfram.com/PlatonicSolid.html>.
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

        /// Returns a uniformly tesselated sphere of radius \a r, created using
        /// a number of polyhedral subdivision \a steps from an Icosahedron.
        static Mesh* Sphere(float const r,int const steps);

        /**
         * \name Ellipse-based meshes
         * Methods to create meshes that are based on extruded ellipses.
         */
        //@{

        /// Returns a torus with ring radius \a rr and tube radius \a rt. The
        /// ring and tube circles are divided into \a sr and \a st segments
        /// respectively.
        static Mesh* Torus(float const rr,float const rt,int const sr,int const st);

        /// Returns a torus knot with knot radius \a rk and tube radius \a rt.
        /// The tube consist of \a sk and \a st segments along the knot and
        /// tube respectively. \a w and \a h specify the width and height of
        /// the knot's spiral, which is obtained by looping through the hole
        /// \a p times with \a q revolutions before joining its ends, where \a p
        /// and \a q have to be relatively prime.
        static Mesh* TorusKnot(float const rk,float const rt,int const sk,int const st,int const p,int const q,float const w=1.0f,float const h=1.0f);

        /// Returns a Möbius Strip with a ring radius width \a rrw and height
        /// \a rrh, and tube radius width \a rtw and height \a rth, where the
        /// ring and tube ellipses are divided into \a sr and \a st segments
        /// respectively.
        static Mesh* MoebiusStrip(float const rrw,float const rrh,float const rtw,float const rth,int const sr,int const st);

        //@}

        /**
         * \name Miscellaneous surfaces
         * Methods creating meshes mainly by wrapping a surface grid in space.
         */
        //@{

        /// Generates a shell with shell radius \a rs and tube radius \a rt at
        /// opening, consisting of \a ss segments along the shell and
        // \a st segments around the tube. The height
        /// is given by \a h, the number of spiral turns by \a n. For details,
        /// see <http://local.wasp.uwa.edu.au/~pbourke/geometry/shell/>.
        static Mesh* Shell(float const rs,float const rt,int const ss,int const st,float const h,int const n);

        //@}

        /**
         * \name Product meshes
         * Methods based on multiplying formulas to yield a 3D composite, see
         * e.g. <http://local.wasp.uwa.edu.au/~pbourke/geometry/supershape3d/>.
         * On Paul Bourke's site, "SuperShape 1" is the longitudinal shape, and
         * "SuperShape 2" is the latitudinal shape.
         */
        //@{

        /// Generates a mesh's surface by calculating the spherical product of
        /// the formulas \a long_form and \a lat_form, evaluated at \a long_segs
        /// and \a lat_segs samples along the longitude and latitude, respectively.
        static Mesh* SphericalMapper(math::Formula const& long_form,int const long_segs,math::Formula const& lat_form,int const lat_segs);

        /// Generates a mesh's surface by calculating the toroidal product of
        /// the formulas \a long_form and \a lat_form, evaluated at \a long_segs
        /// and \a lat_segs samples along the longitude and latitude, respectively.
        static Mesh* ToroidalMapper(math::Formula const& long_form,int const long_segs,math::Formula const& lat_form,int const lat_segs);

        //@}

        /**
         * \name Helper and debug meshes
         * Methods to create meshes that are not particularly useful on their
         * own, but are used by other factory methods or for debugging.
         */
        //@{

        /// Array of transformation matrices.
        typedef global::DynamicArray<math::HMat4f> MatrixArray;

        /// Generates a mesh by extruding the line loop defined by \a contour
        /// along the given \a path. If \a close is \c true, the end of the path
        /// is connected to its beginning, else cut faces are created.
        static Mesh* Extruder(VectorArray const& path,VectorArray const& contour,bool const closed=true,MatrixArray const* const trans=NULL);

        /// Generates a mesh's surface by calculating \a eval at every point on
        /// the grid of size \a s_segs by \a t_segs which is defined by walking
        /// from \a s_min to \a s_max in x-direction and from \a t_min to
        /// \a t_max in y-direction. \a s_closed and \a t_closed denote whether
        /// start and end vertices should be shared to close the mesh in the
        /// respective direction.
        static Mesh* GridMapper(
            math::FormulaR2R3 const& eval
        ,   float const s_min
        ,   float const s_max
        ,   int const s_segs
        ,   bool const s_closed
        ,   float const t_min
        ,   float const t_max
        ,   int const t_segs
        ,   bool const t_closed
        );

        /// Generates a mesh representing the given \a normals by \a n lines starting
        /// at the given \a vertices, optionally with the given \a scale applied.
        static Mesh* Normals(int n,math::Vec3f const* vertices,math::Vec3f const* normals,float const scale=1.0f);

        //@}

      private:

        /// Makes all vertices in the given \a mesh neighbors that have the
        /// specified \a distance, up to the specified vertex \a valence.
        static void populateNeighborhood(Mesh* const mesh,float distance,int const valence);
    };

    /// Class to subdivide meshes using different algorithms, for an overview
    /// see <http://en.wikipedia.org/wiki/Subdivision_surface>.
    class Subdivider
    {
      public:

        /// Function pointer type definition for subdivision schemes that
        /// perform the given number of subdivision \a steps on \a mesh.
        typedef void (*Scheme)(Mesh& mesh,int steps);

        /**
         * \name Interpolating schemes
         * This type of subdivision does not modify the vertices of the base
         * mesh. Newly generated vertices are interpolated on the limit surface.
         */
        //@{

        /// Divides the triangular faces of a mesh into further triangles. If
        /// \a scale is non-zero, the new vertices are scaled to have that length.
        static void Polyhedral(Mesh& mesh,int steps,float const scale);

        /// Convenience wrapper for use with a function pointer that calls
        /// Polyhedral() with \a scale set to \c 0.
        static void Polyhedral(Mesh& mesh,int const steps=1) {
            Polyhedral(mesh,steps,0.0f);
        }

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
        static void Loop(Mesh& mesh,int steps,bool const move);

        /// Convenience wrapper for use with a function pointer that calls
        /// Loop() with \a move set to \c true.
        static void Loop(Mesh& mesh,int const steps=1) {
            Loop(mesh,steps,true);
        }

        /// Divides the triangular faces of a mesh as described by L. Kobbelt in
        /// <http://www.graphics.rwth-aachen.de/uploads/media/sqrt3.pdf>.
        static void Sqrt3(Mesh& mesh,int steps,bool const move);

        /// Convenience wrapper for use with a function pointer that calls
        /// Sqrt3() with \a move set to \c true.
        static void Sqrt3(Mesh& mesh,int const steps=1) {
            Sqrt3(mesh,steps,true);
        }

        /// Divides the quadrangular faces of a mesh as described by E. Catmull
        /// and J. Clark in <http://www.idi.ntnu.no/~fredrior/files/Catmull-Clark%201978%20Recursively%20generated%20surfaces.pdf>.
        static void CatmullClark(Mesh& mesh,int steps=1);

        /// Divides the faces of a mesh as described by D. Doo and M. Sabin in
        /// <http://www.idi.ntnu.no/~fredrior/files/Doo-Sabin%201978%20Recursive%20division.pdf>.
        static void DooSabin(Mesh& mesh,int steps=1);

        //@}

      private:

        /// Structure for inserting a vertex on an existing edge.
        struct EdgeVertex {
            int a,b;       ///< Indices of the adjacent vertices.
            math::Vec3f v; ///< Position of the edge vertex.
        };

        /// List of edge vertices to be inserted after a subdivision step.
        typedef global::DynamicArray<EdgeVertex> EdgeVertexList;

        /// Connects the edge vertices starting from index \a edge_vertices_start
        /// to vertices on adjacent edges in the specified \a mesh.
        static void interconnectEdgeVertices(Mesh& mesh,int edge_vertices_start);

        /// Inserts the given \a edge_vertices into the specified \a mesh and
        /// connects them to vertices on adjacent edges.
        static void spliceEdgeVertices(Mesh& mesh,EdgeVertexList const& edge_vertices);

        /// Adds the missing neighbors of newly inserted vertices after a
        /// subdivision step. Starting from vertex index \a x0i, neighbors
        /// missing in the original mesh \a orig are inserted into the given
        /// \a mesh, which usually is a copy of \a orig.
        static void assignNeighbors(Mesh const& orig,Mesh& mesh,int const x0i);
    };

    /// A simple class to iterate over the edges in a mesh.
    class EdgeIterator
    {
      public:

        /// Associates the iterator with the given \a mesh and the edge defined
        /// by vertex index \vi and its neighbor index \a ni.
        EdgeIterator(Mesh const& mesh,int vi=0,int ni=0)
        :   m_mesh(mesh)
        ,   m_num_vertices(mesh.vertices.getSize())
        ,   m_vi(vi)
        ,   m_ni(ni)
        {}

        /// Returns \c true if the two iterators' vertex and neighbor indices
        /// are equal, \c false otherwise.
        bool operator==(EdgeIterator const& other) {
            // For performance reasons, this does not compare the actual mesh.
            return m_vi==other.m_vi && m_ni==other.m_ni;
        }

        /// Returns \c true if the two iterators' vertex and neighbor indices
        /// are not equal, \c false otherwise.
        bool operator!=(EdgeIterator const& other) {
            return !(*this==other);
        }

        /// Moves the iterator forward to the next edge in the mesh.
        EdgeIterator operator++() {
            IndexArray const* neighbors=&m_mesh.neighbors[m_vi];

            // The below break criterion defines a (somewhat arbitrary) relation on
            // the vertex indices to ensure walking each edge only in one direction.
            do {
                // Get the next neighbor index.
                if (++m_ni==neighbors->getSize()) {
                    m_ni=0;

                    // Get the next vertex index.
                    if (++m_vi==m_num_vertices) {
                        break;
                    }

                    // Refer to the current neighbor indices.
                    neighbors=&m_mesh.neighbors[m_vi];
                }
            } while ((*neighbors)[m_ni]<static_cast<unsigned int>(m_vi));

            return *this;
        }

        /// Returns the index of the edge's first vertex.
        int indexA() const {
            return m_vi;
        }

        /// Returns the index of the edge's second vertex.
        int indexB() const {
            return m_mesh.neighbors[m_vi][m_ni];
        }

        /// Returns the edge's first vertex.
        math::Vec3f const& vertexA() const {
            return m_mesh.vertices[indexA()];
        }

        /// Returns the edge's second vertex.
        math::Vec3f const& vertexB() const {
            return m_mesh.vertices[indexB()];
        }

      private:

        Mesh const& m_mesh; ///< The mesh being iterated over.
        int m_num_vertices; ///< Number of vertices in the mesh.

        int m_vi; ///< Index of the first vertex.
        int m_ni; ///< Index of the first vertex' neighbor.
    };

    /// A simple class to iterate over the primitives in a mesh.
    class PrimitiveIterator
    {
      public:

        /// Associates the iterator with the given \a mesh and the primitive defined
        /// by the edge from vertex index \vi to its neighbor index \a ni.
        PrimitiveIterator(Mesh const& mesh,int vi=0,int ni=0)
            :   m_mesh(mesh)
            ,   m_num_vertices(mesh.vertices.getSize())
            ,   m_vi(vi)
            ,   m_ni(ni)
        {
            if (m_vi<m_mesh.vertices.getSize()) {
                if (m_mesh.neighbors[m_vi].getSize()==0) {
                    m_primitive.setSize(1);
                    m_primitive[0]=m_vi;
                }
                else {
                    m_mesh.orbit(m_vi,m_mesh.neighbors[m_vi][m_ni],m_primitive);
                }
            }
        }

        /// Returns \c true if the two iterators' vertex and neighbor indices
        /// are equal, \c false otherwise.
        bool operator==(PrimitiveIterator const& other) {
            // For performance reasons, this does not compare the actual mesh.
            return m_vi==other.m_vi && m_ni==other.m_ni;
        }

        /// Returns \c true if the two iterators' vertex and neighbor indices
        /// are not equal, \c false otherwise.
        bool operator!=(PrimitiveIterator const& other) {
            return !(*this==other);
        }

        /// Moves the iterator forward to the next primitive in the mesh.
        PrimitiveIterator operator++() {
            IndexArray const* neighbors=&m_mesh.neighbors[m_vi];
            bool ordered;

            do {
                // The below break criterion defines a (somewhat arbitrary) relation on
                // the vertex indices to ensure walking each edge only in one direction.
                do {
                    int num_neighbors=neighbors->getSize();

                    // Handle point primitives.
                    if (num_neighbors==0) {
                        m_primitive.setSize(1);
                        m_primitive[0]=++m_vi;
                        return *this;
                    }

                    // Get the next neighbor index.
                    if (++m_ni==num_neighbors) {
                        m_ni=0;

                        // Get the next vertex index.
                        if (++m_vi==m_num_vertices) {
                            break;
                        }

                        // Refer to the current neighbor indices.
                        neighbors=&m_mesh.neighbors[m_vi];
                    }
                } while ((*neighbors)[m_ni]<static_cast<unsigned int>(m_vi));

                // Only accept the primitive indices if they are "in order" to avoid index
                // permutations of the same primitive.
                ordered=true;
                m_mesh.orbit(m_vi,(*neighbors)[m_ni],m_primitive);
                for (int i=2;i<m_primitive.getSize();++i) {
                    if (m_primitive[i]<static_cast<unsigned int>(m_vi)) {
                        ordered=false;
                        break;
                    }
                }
            } while (!ordered);

            return *this;
        }

        /// Returns the primitive's vertex indices.
        IndexArray const& indices() const {
            return m_primitive;
        }

      private:

        Mesh const& m_mesh; ///< The mesh being iterated over.
        int m_num_vertices; ///< Number of vertices in the mesh.

        int m_vi; ///< Index of the first vertex.
        int m_ni; ///< Index of the first vertex' neighbor.

        IndexArray m_primitive;        ///< The current primitive's indices.
    };

    /**
     * \name Constructors
     */
    //@{

    /// Creates a mesh with \a num_vertices uninitialized vertices.
    Mesh(int const num_vertices=0)
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

    //@}

    /**
     * \name Iterators
     */
    //@{

    /// Returns an iterator that marks the begin of the mesh's set of edges.
    EdgeIterator beginEdges() const {
        return EdgeIterator(*this);
    }

    /// Returns an iterator that marks the end of the mesh's set of edges. Note
    /// that incrementing the end iterator is undefined.
    EdgeIterator endEdges() const {
        return EdgeIterator(*this,numEdges()?vertices.getSize():0,0);
    }

    /// Returns an iterator that marks the begin of the mesh's set of primitives.
    PrimitiveIterator beginPrimitives() const {
        return PrimitiveIterator(*this);
    }

    /// Returns an iterator that marks the end of the mesh's set of primitives.
    /// Note that incrementing the end iterator is undefined.
    PrimitiveIterator endPrimitives() const {
        return PrimitiveIterator(*this,vertices.getSize(),0);
    }

    //@}

    /**
     * \name Walking operations
     */
    //@{

    /// Returns the index of the vertex following the vertex with index \a oi in
    /// the neighborhood of the vertex with index \a ci at the given \a distance.
    int nextTo(int const oi,int const ci,int const distance=1) const;

    /// Returns the index of the vertex preceding the vertex with index \a oi in
    /// the neighborhood of the vertex with index \a ci at the given \a distance.
    int prevTo(int const oi,int const ci,int const distance=1) const;

    /// Returns the \a primitive and its number of vertices the oriented edge
    /// from \a ai to \a bi belongs to.
    void orbit(int ai,int bi,IndexArray& primitive) const;

    //@}

    /**
     * \name Editing operations
     */
    //@{

    /// Returns the average of the vertices indexed by \a primitive.
    math::Vec3f average(IndexArray const& primitive) const;

    /// Inserts \a xi after or before \a oi in the neighborhood of \a ci.
    void splice(int const xi,int const oi,int const ci,bool const after=true);

    /// Inserts a new vertex \a v on the edge between \a ai and \a bi, and
    /// returns its index in the vertex array.
    int insert(math::Vec3f const& v,int const ai,int const bi);

    /// Removes \a bi from the neighborhood of \a ai and vice versa unless
    /// \a oneway is set to \c true.
    void separate(int const ai,int const bi,bool const oneway=false);

    //@}

    /**
     * \name Topological properties
     */
    //@{

    /// Returns the number of vertices in the mesh.
    int numVertices() const {
        return vertices.getSize();
    }

    /// Returns the number of edges in the mesh.
    int numEdges() const {
        int e=0;

        for (int i=0;i<neighbors.getSize();++i) {
            e+=neighbors[i].getSize();
        }

        return e/2;
    }

    /// Returns the number of faces in the mesh. As the Euler formula is used,
    /// the result is only valid for polyhedra that are topologically equivalent
    /// to a sphere (i.e. that do not contain any holes).
    int numFaces() const {
        return numEdges()-numVertices()+2;
    }

    //@}

#ifndef GALE_TINY_CODE
    /// Performs a simple brute-force check of the neighborhood information.
    /// Returns -1 on success or the vertex index causing the error.
    int check() const;
#endif

    VectorArray vertices; ///< Array of vertex positions.
    IndexTable neighbors; ///< Array of arrays of neighboring vertex indices.
};

} // namespace model

} // namespace gale

#endif // MESH_H
