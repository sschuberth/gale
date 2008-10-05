#include "gale/model/mesh.h"

#include "gale/global/platform.h"

using namespace gale::math;

namespace gale {

namespace model {

void Mesh::Renderer::compile()
{
    triangles.clear();

    for (int vi=0;vi<mesh.vertices.getSize();++vi) {
        IndexArray const& vn=mesh.neighbors[vi];
        Vec3f const& v=mesh.vertices[vi];

        for (int n=0;n<vn.getSize();++n) {
            int ai=vn[n];
            int bi=mesh.nextTo(ai,vi);

            // Be sure to walk each pair of vertices, i.e. edge, only once. Use
            // the address in memory to define a relation on the universe of
            // vertices.
            Vec3f const& a=mesh.vertices[ai];
            Vec3f const& b=mesh.vertices[bi];
            if (&a<&v || &b<&v) {
                continue;
            }

            IndexArray* prim=&triangles;

            // Check whether this face is a triangle or quad.
            int qi=mesh.prevTo(vi,ai);
            if (qi!=bi) {
                prim=&quads;
            }

            prim->insert(vi);
            prim->insert(ai);
            if (qi!=bi) {
                prim->insert(qi);
            }
            prim->insert(bi);
        }
    }
}

void Mesh::Renderer::render()
{
    glEnableClientState(GL_VERTEX_ARRAY);

    glVertexPointer(3,GL_FLOAT,0,mesh.vertices);

    glDrawElements(GL_TRIANGLES,triangles.getSize(),GL_UNSIGNED_INT,triangles);
    glDrawElements(GL_QUADS,quads.getSize(),GL_UNSIGNED_INT,quads);
}

} // namespace model

} // namespace gale
