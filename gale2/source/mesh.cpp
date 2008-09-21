#include "gale/model/mesh.h"

using namespace gale::math;

namespace gale {

namespace model {

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

int Mesh::insert(int ai,int bi)
{
    Vec3f const& a=vertices[ai];
    Vec3f const& b=vertices[bi];

    int xi=vertices.getSize();
    vertices.insert((a+b)*0.5f);

    //unsigned int xn[]={ai,bi};
    //neighbors.insert(xn);
    IndexList xn;
    xn.insert(ai);
    xn.insert(bi);
    neighbors.insert(xn);

    int n;

    IndexList& an=neighbors[ai];
    for (n=0;n<an.getSize();++n) {
        if (an[n]==bi) {
            an[n]=xi;
            break;
        }
    }

    IndexList& bn=neighbors[bi];
    for (n=0;n<bn.getSize();++n) {
        if (bn[n]==ai) {
            bn[n]=xi;
            break;
        }
    }

    return xi;
}

} // namespace model

} // namespace gale
