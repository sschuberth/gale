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
    typedef global::DynamicArray<math::Vec3f> VertexArray;
    typedef global::DynamicArray<unsigned int> IndexArray;
    typedef global::DynamicArray<IndexArray> NeighborArray;

    class Factory
    {
      public:

        /**
         * \name Platonic Solids
         * Factory methods to create the five Platonic Solids, each with unit
         * edge length, see <http://mathworld.wolfram.com/PlatonicSolid.html>.
         */
        //@{

        static Mesh* Tetrahedron();
        static Mesh* Octahedron();
        static Mesh* Hexahedron();
        static Mesh* Icosahedron();
        static Mesh* Dodecahedron();

        //@}

      protected:

        static void populateNeighborhood(Mesh* mesh,float distance,int neighbors);
    };

    class Subdivider
    {
      public:

        Subdivider(Mesh& mesh)
        :   mesh(mesh) {}

        void divPolyhedral(int steps=1);
        void divLoop(int steps=1,bool move=true);

      protected:

        static void assignNeighbors(Mesh const& old,Mesh& mesh,int x0i);

        Mesh& mesh;
    };

    struct Renderer
    {
        Renderer(Mesh& mesh)
        :   mesh(mesh) {}

        void compile();
        void render();

        Mesh& mesh;
        IndexArray triangles;
        IndexArray quads;
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

    /// Returns the index of the vertex following \a xi in the neighborhood of
    /// \a vi, both given as indices into the vertex array.
    int nextTo(int xi,int vi) const;

    /// Returns the index of the vertex preceding \a xi in the neighborhood of
    /// \a vi, both given as indices into the vertex array.
    int prevTo(int xi,int vi) const;

    /// Inserts a new vertex \a x on the edge between \a ai and \a bi and
    /// returns its index in the vertex array.
    int insert(int ai,int bi,math::Vec3f const& x);

    VertexArray vertices;    ///< Array of vertices in the mesh.
    NeighborArray neighbors; ///< Array of arrays of neighboring vertex indices.
};

} // namespace model

} // namespace gale

#endif // MESH_H
