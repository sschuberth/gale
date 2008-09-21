#include "gale/model/mesh.h"

using namespace gale::math;

namespace gale {

namespace model {

int Mesh::nextTo(int xi,int vi) const
{
    IndexArray const& vn=neighbors[vi];
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
    IndexArray const& vn=neighbors[vi];
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

int Mesh::insert(int ai,int bi,Vec3f const& x)
{
    // Add a new vertex at index xi.
    int xi=vertices.getSize();
    vertices.insert(x);

    unsigned int xn[]={ai,bi};
    neighbors.insert(xn);

    int n;
    unsigned int* np;

    // In the neighborhood of ai, replace bi with xi.
    IndexArray& an=neighbors[ai];
    for (n=0,np=an.data();n<an.getSize();++n,++np) {
        if (*np==bi) {
            *np=xi;
            break;
        }
    }

    // In the neighborhood of bi, replace ai with xi.
    IndexArray& bn=neighbors[bi];
    for (n=0,np=bn.data();n<bn.getSize();++n,++np) {
        if (*np==ai) {
            *np=xi;
            break;
        }
    }

    return xi;
}

} // namespace model

} // namespace gale
