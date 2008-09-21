#include "gale/model/mesh.h"

#include "gale/global/platform.h"

using namespace gale::math;

namespace gale {

namespace model {

Mesh* Mesh::Factory::Octahedron()
{
    // http://mathworld.wolfram.com/Octahedron.html
    static float const a=1.0f/(2.0f*::sqrt(2.0f));
    static float const b=0.5f;

    static Vec3f vertices[]={
        // Square base vertices.
        Vec3f(+a, 0,+a),
        Vec3f(+a, 0,-a),
        Vec3f(-a, 0,-a),
        Vec3f(-a, 0,+a),

        // Top and bottom vertices.
        Vec3f( 0,+b, 0),
        Vec3f( 0,-b, 0)
    };

    static unsigned int neighbors[][4]={
        {1,5,3,4},
        {0,4,2,5},
        {1,4,3,5},
        {0,5,2,4},
        {0,3,2,1},
        {0,1,2,3}
    };

    Mesh* m=new Mesh(vertices);

    for (int i=0;i<G_ARRAY_LENGTH(vertices);++i) {
        m->neighbors[i].insert(neighbors[i]);
    }

    return m;
}

void Mesh::Renderer::compile()
{
    triangles.clear();

    for (int vi=0;vi<mesh.vertices.getSize();++vi) {
        IndexList const& vn=mesh.neighbors[vi];

        Vec3f const& v=mesh.vertices[vi];

        for (int n=0;n<vn.getSize();++n) {
            int ai=vn[n];
            int bi=mesh.nextTo(ai,vi);

            // Be sure to walk each edge of a triangle only once.
            Vec3f const& a=mesh.vertices[ai];
            Vec3f const& b=mesh.vertices[bi];
            if (&a<&v || &b<&v) {
                continue;
            }

            triangles.insert(vi);
            triangles.insert(ai);
            triangles.insert(bi);
        }
    }
}

void Mesh::Renderer::render()
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3,GL_FLOAT,0,mesh.vertices);
    glDrawElements(GL_TRIANGLES,triangles.getSize(),GL_UNSIGNED_INT,triangles);
}

int Mesh::nextTo(int xi,int vi) const
{
    IndexList const& vn=neighbors[vi];
    unsigned int const* vnp=&vn[0];

    // Prepare for a trick to compare vertices by their index in the vertex array.
    Vec3f const* xp=&vertices[0]+xi;

    // Search v's neighborhood for x, and return x' successor.
    for (int n=0;n<vn.getSize();++n) {
        if (&vertices[*vnp++]==xp) {
            ++n;
            if (n>=vn.getSize()) {
                // Wrap in the neighborhood.
                n-=vn.getSize();
            }
            return vn[n];
        }
    }

    return -1;
}

int Mesh::prevTo(int xi,int vi) const
{
    IndexList const& vn=neighbors[vi];
    unsigned int const* vnp=&vn[0];

    // Prepare for a trick to compare vertices by their index in the vertex array.
    Vec3f const* xp=&vertices[0]+xi;

    // Search v's neighborhood for x, and return x' predecessor.
    for (int n=0;n<vn.getSize();++n) {
        if (&vertices[*vnp++]==xp) {
            --n;
            if (n<0) {
                // Wrap in the neighborhood.
                n+=vn.getSize();
            }
            return vn[n];
        }
    }

    return -1;
}

} // namespace model

} // namespace gale
