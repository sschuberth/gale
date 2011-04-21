/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
 *
 * Copyright (C) 2005-2010  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
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

#include "gale/model/mesh.h"

using namespace gale::math;

namespace gale {

namespace model {

int Mesh::nextTo(int const oi,int const ci,int const distance) const
{
    IndexArray const& cn=neighbors[ci];

    // Search the center vertex' neighborhood for the orbit vertex.
    int n=cn.find(oi);

    if (n>=0) {
        // Walk the specified distance along the orbit.
        n+=distance;

        // Wrap in the neighborhood (do not use modulo for performance).
        while (n>=cn.getSize()) {
            n-=cn.getSize();
        }

        n=cn[n];
    }

    return n;
}

int Mesh::prevTo(int const oi,int const ci,int const distance) const
{
    IndexArray const& cn=neighbors[ci];

    // Search the center vertex' neighborhood for the orbit vertex.
    int n=cn.find(oi);

    if (n>=0) {
        // Walk the specified distance along the orbit.
        n-=distance;

        // Wrap in the neighborhood (do not use modulo for performance).
        while (n<0) {
            n+=cn.getSize();
        }

        n=cn[n];
    }

    return n;
}

void Mesh::orbit(int ai,int bi,IndexArray& primitive) const
{
    // Assume a triangle, as this is the most common case.
    primitive.setSize(3);

    // The given edge is already known.
    primitive[0]=ai;
    primitive[1]=bi;

    // Return immediately if ai and bi are no neighbors or this is just a line.
    int ci=prevTo(ai,bi);
    if (ci<0 || ci==ai) {
        primitive.setSize(2);
        return;
    }

    primitive[2]=ci;

    // Add the vertex following ai in the neighborhood of bi until we are back
    // at the first vertex.
    for (;;) {
        ai=bi;
        bi=ci;

        ci=prevTo(ai,bi);
        G_ASSERT(ci>=0)

        if (ci==static_cast<int>(primitive[0])) {
            break;
        }

        primitive.insert(ci);
    }
}

Vec3f Mesh::average(IndexArray const& primitive) const
{
    int num=primitive.getSize();
    Vec3f avg=Vec3f::ZERO();

    for (int i=0;i<num;++i) {
        avg+=vertices[primitive[i]];
    }

    avg/=static_cast<float>(num);
    return avg;
}

void Mesh::splice(int const xi,int const oi,int const ci,bool const after)
{
    IndexArray& cn=neighbors[ci];

    // Search the center vertex' neighborhood for the orbit vertex.
    int n=cn.find(oi);
    G_ASSERT(n>=0)
    cn.insert(xi,n+static_cast<int>(after));
}

int Mesh::insert(Vec3f const& v,int const ai,int const bi)
{
    int vi=vertices.insert(v);

    unsigned int const vn[]={ai,bi};
    neighbors.insert(vn);

    int n;

    // In the neighborhood of ai, replace bi with vi.
    IndexArray& an=neighbors[ai];
    n=an.find(bi);
    G_ASSERT(n>=0)
    an[n]=vi;

    // In the neighborhood of bi, replace ai with vi.
    IndexArray& bn=neighbors[bi];
    n=bn.find(ai);
    G_ASSERT(n>=0)
    bn[n]=vi;

    return vi;
}

void Mesh::separate(int const ai,int const bi,bool const oneway)
{
    IndexArray& an=neighbors[ai];
    int n=an.find(bi);
    G_ASSERT(n>=0)
    an.remove(n);

    if (!oneway) {
        separate(bi,ai,true);
    }
}

#ifndef GALE_TINY_CODE

int Mesh::check() const {
    for (int vi=0;vi<vertices.getSize();++vi) {
        IndexArray const& vn=neighbors[vi];
        for (int ni=0;ni<vn.getSize();++ni) {
            // Search for duplicates in the neighbor list.
            for (int di=0;di<ni;++di) {
                if (vn[di]==vn[ni]) {
                    return vi;
                }
            }

            // Check if neighbors are really neighbors of each other.
            if (neighbors[vn[ni]].find(vi)==-1) {
                return vi;
            }
        }
    }

    return -1;
}

#endif

} // namespace model

} // namespace gale
