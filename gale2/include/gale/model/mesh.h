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
#include "../math/formula.h"
#include "../math/hmatrix4.h"

#ifdef GALE_USE_VBO
    #include "../wrapgl/vertexarrayobject.h"
    #include "../wrapgl/vertexbufferobject.h"
#endif

#include "boundingbox.h"

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

            /// Returns an array of vectors as calculated by the Superformula,
            /// see <http://local.wasp.uwa.edu.au/~pbourke/geometry/supershape/>.
            static void Supershape(VectorArray& shape,int const segs,float const m,float const n1,float const n2,float const n3,float const a=1.0,float const b=1.0);
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

        /// Returns a torus with outer radius \a r1 and inner radius \a r2. The
        /// outer and inner circles are divided into \a r1_segs and \a r2_segs
        /// segments respectively.
        static Mesh* Torus(float const r1,float const r2,int const r1_segs,int const r2_segs);

        /// Returns a torus knot with knot radius \a rk and tube radius \a rt.
        /// The tube consist of \a sp and \a sc segments along the path and
        /// circle respectively. \a w and \a h specify the width and height of
        /// the knot's spiral, which is obtained by looping through the hole
        /// \a p times with \a q revolutions before joining its ends, where \a p
        /// and \a q have to be relatively prime.
        static Mesh* TorusKnot(float const rk,float const rt,int const sp,int const sc,int const p,int const q,float const w=1,float const h=1);

        /// Returns a Möbius Strip with an outer radius width \a r1w and height
        /// \a r1h, and inner radius width \a r2w and height \a r2h, where the
        /// inner and outer ellipses are divided into \a r1_segs and \a r2_segs
        /// segments respectively.
        static Mesh* MoebiusStrip(float const r1w,float const r1h,float const r2w,float const r2h,int const r1_segs,int const r2_segs);

        //@}

        /**
         * \name Miscellaneous surfaces
         * Methods creating meshes mainly by wrapping a surface grid in space.
         */
        //@{

        /// Generates an apple consisting of \a s_sections pieces of longitude
        /// and \a t_section pieces of latitude, for details see
        /// <http://local.wasp.uwa.edu.au/~pbourke/geometry/apple/>.
        static Mesh* Apple(int const s_sections,int const t_sections);

        /// Generates a shell consisting of \a s_sections pieces around the
        /// opening and \a t_section pieces along the side. \a r1 is the radius
        /// of the opening, \a r2 the radius of the inner circle. The height
        /// is given by \a h, the number of spiral turns by \a n. For details,
        /// see <http://local.wasp.uwa.edu.au/~pbourke/geometry/shell/>.
        static Mesh* Shell(int const s_sections,int const t_sections,float const r1,float const r2,float const h,int const n);

        //@}

        /**
         * \name Product meshes
         * Methods based on multiplying formulas to yield a 3D composite, see
         * e.g. <http://local.wasp.uwa.edu.au/~pbourke/geometry/supershape3d/>.
         */
        //@{

        /// Generates a mesh's surface by calculating the spherical product of
        /// the formulas \a r1 and \a r2, evaluated at \a r1_segs and \a r2_segs
        /// samples, respectively.
        static Mesh* SphericalProduct(math::Formula const& r1,int const r1_segs,math::Formula const& r2,int const r2_segs);

        /// Generates a mesh's surface by calculating the toroidal product of
        /// the formulas \a r1 and \a r2, evaluated at \a r1_segs and \a r2_segs
        /// samples, respectively.
        static Mesh* ToroidalProduct(math::Formula const& r1,int const r1_segs,math::Formula const& r2,int const r2_segs);

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
        /// the grid of size \a s_sections by \a t_sections which is defined by
        /// walking from \a s_min to \a s_max in x-direction and from \a t_min
        /// to \a t_max in y-direction. \a s_closed and \a t_closed denote
        /// whether start and end vertices should be shared to close the mesh in
        /// the respective direction.
        static Mesh* GridMapper(
            math::FormulaR2R3 const& eval
        ,   float const s_min
        ,   float const s_max
        ,   int const s_sections
        ,   bool const s_closed
        ,   float const t_min
        ,   float const t_max
        ,   int const t_sections
        ,   bool const t_closed
        );

        /// Generates a mesh consisting of lines only that represent the
        /// compiled mesh's vertex normals stored in the \a renderer, optionally
        /// with the given \a scale applied.
        static Mesh* Normals(Preparer const& geom,float const scale=1.0f);

        //@}

      protected:

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

      protected:

        /// Adds the missing neighbors of newly inserted vertices after a
        /// subdivision step. Starting from vertex index \a x0i, neighbors
        /// missing in the original mesh \a orig are inserted into the given
        /// \a mesh, which usually is a copy of \a orig.
        static void assignNeighbors(Mesh const& orig,Mesh& mesh,int const x0i);
    };

    /// Class to prepare a mesh for rendering.
    class Preparer
    {
      public:

        /// Constructor that simply initializes the mesh to render to NULL.
        Preparer()
        :   m_mesh(NULL) {}

        /// Generates the primitive index arrays from the mesh data structure
        /// and calculates vertex normals from averaged face normals.
        void compile(Mesh const* const mesh);

        /// Returns a pointer to the last compiled mesh.
        Mesh const* getMesh() const {
            return m_mesh;
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

        AABB box; ///< The axis-aligned bounding box.

        /// Names for the primitives stored in the indices table.
        enum PrimitiveIndices {
            PI_POINTS    ///< The first array of indices describes points.
        ,   PI_LINES     ///< The second array of indices describes lines.
        ,   PI_TRIANGLES ///< The third array of indices describes triangles.
        ,   PI_QUADS     ///< The third array of indices describes quadrilaterals.
        ,   PI_COUNT     ///< Special entry to name the number of enum entries.
        };

        static GLenum const GL_PRIM_TYPE[PI_COUNT]; ///< The primitive types as stored in the indices table.

        IndexTable indices;  ///< Table of vertex indices describing primitives.
        IndexTable polygons; ///< Table of vertex indices describing polygons.

#ifdef GALE_USE_VBO
        wrapgl::ArrayBufferObject vbo_vertnorm; ///< Vertices and normals on the GPU.
        wrapgl::IndexBufferObject vbo_indices;  ///< Primitive and polygon indices on the GPU.

        wrapgl::VertexArrayObject vao; ///< Bindable state vector for the render arrays.
#else
        VectorArray normals; ///< Array of vertex normals.
#endif

      protected:

        Mesh const* m_mesh;   ///< Reference to the mesh to render.
    };

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

    /**
     * \name Selection operations
     */
    //@{

    /// Returns the index of the vertex following \a xi in the neighborhood of
    /// \a vi, both given as indices into the vertex array. Optionally, this is
    /// repeated the given number of \a steps.
    int nextTo(int const xi,int const vi,int const steps=1) const;

    /// Returns the index of the vertex preceding \a xi in the neighborhood of
    /// \a vi, both given as indices into the vertex array. Optionally, this is
    /// repeated the given number of \a steps.
    int prevTo(int const xi,int const vi,int const steps=1) const;

    /// Given an oriented edge from \a ai to \a bi, returns the number of
    /// vertices that make up the edge's face, and their indices in \a polygon.
    int orbit(int ai,int bi,IndexArray& polygon) const;

    //@}

    /**
     * \name Editing operations
     */
    //@{

    /// Inserts a new vertex \a x on the edge between \a ai and \a bi and
    /// returns its index in the vertex array.
    int insert(int const ai,int const bi,math::Vec3f const& x);

    /// Inserts \a ai after or before \a xi in the neighborhood of \a vi.
    void splice(int const ai,int const xi,int const vi,bool const after=true);

    /// Removes \a xi from the neighborhood of \a vi.
    void erase(int const xi,int const vi);

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
