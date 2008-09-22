#include "gale/model/mesh.h"

using namespace gale::math;

namespace gale {

namespace model {

void Mesh::Subdivider::divPolyhedral(int steps)
{
    while (steps-->0) {
        Mesh old=mesh;

        // Store the index of the first new vertex.
        int x0i=old.vertices.getSize();

        // Loop over all vertices in the base mesh.
        for (int vi=0;vi<x0i;++vi) {
            IndexArray const& vn=old.neighbors[vi];
            Vec3f const& v=old.vertices[vi];

            // Loop over v's neighborhood.
            for (int n=0;n<vn.getSize();++n) {
                int ui=vn[n];

                // Be sure to walk each pair of vertices, i.e. edge, only once. Use
                // the address in memory to define a relation on the universe of
                // vertices.
                Vec3f const& u=old.vertices[ui];
                if (&u<&v) {
                    continue;
                }

                // Add a new vertex as the arithmetic average of its two neighbors.
                mesh.insert(ui,vi,(u+v)*0.5f);
            }
        }

        old=mesh;
        assignNeighbors(old,mesh,x0i);
    }
}

void Mesh::Subdivider::divLoop(int steps)
{
    while (steps-->0) {
        Mesh old=mesh;

        // Store the index of the first new vertex.
        int x0i=old.vertices.getSize();

        // Loop over all vertices in the base mesh.
        for (int vi=0;vi<x0i;++vi) {
            IndexArray const& vn=old.neighbors[vi];
            Vec3f const& v=old.vertices[vi];

            int valence=vn.getSize();
            double weight=::pow(0.375+0.25*::cos(2.0*Constd::PI()/valence),2.0)+0.375;

            Vec3f q=Vec3f::ZERO();

            // Loop over v's neighborhood.
            for (int n=0;n<vn.getSize();++n) {
                int ui=vn[n];

                Vec3f const& u=old.vertices[ui];
                q+=u;

                // Be sure to walk each pair of vertices, i.e. edge, only once. Use
                // the address in memory to define a relation on the universe of
                // vertices.
                if (&u<&v) {
                    continue;
                }

                Vec3f x = v*0.375f + u*0.375f
                        + old.vertices[old.nextTo(ui,vi)]*0.125f
                        + old.vertices[old.prevTo(ui,vi)]*0.125f;

                // Add a new vertex as calculated from its neighbors.
                mesh.insert(ui,vi,x);
            }

            // Modify the existing vertex as calculated from the weighted q vertex.
            q/=static_cast<float>(valence);
            mesh.vertices[vi]=lerp(q,mesh.vertices[vi],weight);
        }

        old=mesh;
        assignNeighbors(old,mesh,x0i);
    }
}

void Mesh::Subdivider::assignNeighbors(Mesh const& old,Mesh& mesh,int x0i)
{
    for (int vi=x0i;vi<old.vertices.getSize();++vi) {
        IndexArray& vn=mesh.neighbors[vi];

        // Get any neighbor of v, just pick the first one.
        int ai=vn[0];
        int bi=old.nextTo(ai,vi);

        // ai and bi are already neighbors.
        vn.insert(old.nextTo(vi,ai),0);
        vn.insert(old.prevTo(vi,ai),2);
        vn.insert(old.nextTo(vi,bi),3);
        vn.insert(old.prevTo(vi,bi));
    }
}

} // namespace model

} // namespace gale
