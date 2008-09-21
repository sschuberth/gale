#ifndef MESH_H
#define MESH_H

#include "../math/vector.h"

#include "../global/dynamicarray.h"

namespace gale {

namespace model {

struct Mesh
{
    typedef global::DynamicArray<math::Vec3f> VertexList;
    typedef global::DynamicArray<unsigned int> IndexList;
    typedef global::DynamicArray<IndexList> NeighborList;

    struct Factory
    {
        static Mesh* Octahedron();
    };

    struct Renderer
    {
        Renderer(Mesh& mesh)
        :   mesh(mesh) {}

        void compile();
        void render();

        Mesh& mesh;
        IndexList triangles;
    };

    Mesh(int num_vertices=0)
    :   vertices(num_vertices),neighbors(num_vertices) {}

    template<size_t size>
    Mesh(math::Vec3f const (&vertex_array)[size]) {
        vertices.insert(vertex_array);
        neighbors.setSize(size);
    }

    /// Returns the index of the vertex following \a xi in the neighborhood of \a vi.
    int nextTo(int xi,int vi);

    /// Returns the index of the vertex preceding \a xi in the neighborhood of \a vi.
    int prevTo(int xi,int vi);

    VertexList vertices;
    NeighborList neighbors;
};

} // namespace model

} // namespace gale

#endif // MESH_H
