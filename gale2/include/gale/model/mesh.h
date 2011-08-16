/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
 *
 * Copyright (C) 2005-2011  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
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

#pragma once

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
        /// and J. Clark in <http://www.cs.berkeley.edu/~sequin/CS284/PAPERS/CatmullClark_SDSurf.pdf>.
        static void CatmullClark(Mesh& mesh,int steps=1);

        /// Divides the faces of a mesh as described by D. Doo and M. Sabin in
        /// <http://trac2.assembla.com/DooSabinSurfaces/export/12/trunk/docs/Doo%201978%20Subdivision%20algorithm.pdf>.
        static void DooSabin(Mesh& mesh,int steps=1);

        //@}

      private:

        /// Adds the missing neighbors of newly inserted vertices after a
        /// subdivision step. Starting from vertex index \a x0i, neighbors
        /// missing in the original mesh \a orig are inserted into the given
        /// \a mesh, which usually is a copy of \a orig.
        static void assignNeighbors(Mesh const& orig,Mesh& mesh,int const x0i);
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

    /// Performs a simple brute-force check of the neighborhood information.
    /// Returns -1 on success or the vertex index causing the error.
    int check() const;

    VectorArray vertices; ///< Array of vertex positions.
    IndexTable neighbors; ///< Array of arrays of neighboring vertex indices.
};

} // namespace model

} // namespace gale
