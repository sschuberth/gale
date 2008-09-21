#include "gale/model/mesh.h"

using namespace gale::math;

namespace gale {

namespace model {

void Mesh::Subdivider::polyhedral()
{
    Mesh old=mesh;

    int xi=old.vertices.getSize();

    for (int vi=0;vi<xi;++vi) {
        IndexArray const& vn=old.neighbors[vi];

        Vec3f const& v=old.vertices[vi];

        for (int n=0;n<vn.getSize();++n) {
            int ui=vn[n];

            // Be sure to walk each pair of vertices, i.e. edge, only once.
            Vec3f const& u=old.vertices[ui];
            if (&u<&v) {
                continue;
            }

            mesh.insert(ui,vi);
        }
    }

    old=mesh;

    for (int vi=xi;vi<old.vertices.getSize();++vi) {
        IndexArray& vn=mesh.neighbors[vi];

        // Get any neighbor of v, pick the first one.
        int ai=vn[0];
        int bi=old.nextTo(ai,vi);

        vn.clear();
        vn.insert(old.nextTo(vi,ai));
        vn.insert(ai);
        vn.insert(old.prevTo(vi,ai));
        vn.insert(old.nextTo(vi,bi));
        vn.insert(bi);
        vn.insert(old.prevTo(vi,bi));
    }
}

} // namespace model

} // namespace gale
