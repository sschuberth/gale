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

int Mesh::nextTo(int const xi,int const vi,int const steps) const
{
    // Search v's neighborhood for x, and return x' successor.
    IndexArray const& vn=neighbors[vi];
    int n=vn.find(xi);

    if (n>=0) {
        n+=steps;
        while (n>=vn.getSize()) {
            // Wrap in the neighborhood.
            n-=vn.getSize();
        }
        n=vn[n];
    }

    return n;
}

int Mesh::prevTo(int const xi,int const vi,int const steps) const
{
    // Search v's neighborhood for x, and return x' predecessor.
    IndexArray const& vn=neighbors[vi];
    int n=vn.find(xi);

    if (n>=0) {
        n-=steps;
        while (n<0) {
            // Wrap in the neighborhood.
            n+=vn.getSize();
        }
        n=vn[n];
    }

    return n;
}

int Mesh::orbit(int ai,int bi,IndexArray& polygon) const
{
    // Optimize for triangle faces.
    polygon.setSize(3);

    polygon[0]=ai;
    polygon[1]=bi;

    int ci=prevTo(ai,bi);
    if (ci<0 || static_cast<unsigned int>(ci)==polygon[0]) {
        polygon.setSize(2);
        return polygon.getSize();
    }

    polygon[2]=ci;

    // Add the vertex immediately following ai in the neighborhood of bi until
    // we return to the starting vertex.
    for (;;) {
        ai=bi;
        bi=ci;

        ci=prevTo(ai,bi);
        if (ci<0 || static_cast<unsigned int>(ci)==polygon[0]) {
            break;
        }

        polygon.insert(ci);
    }

    return polygon.getSize();
}

int Mesh::insert(int const ai,int const bi,Vec3f const& x)
{
    // Add a new vertex at index xi.
    int xi=vertices.insert(x);

    unsigned int const xn[]={ai,bi};
    neighbors.insert(xn);

    int n;

    // In the neighborhood of ai, replace bi with xi.
    IndexArray& an=neighbors[ai];
    n=an.find(bi);
    if (n>=0) {
        an[n]=xi;
    }

    // In the neighborhood of bi, replace ai with xi.
    IndexArray& bn=neighbors[bi];
    n=bn.find(ai);
    if (n>=0) {
        bn[n]=xi;
    }

    return xi;
}

void Mesh::splice(int const ai,int const xi,int const vi,bool const after)
{
    // Insert ai after or before xi in vi's neighborhood.
    IndexArray& vn=neighbors[vi];
    int n=vn.find(xi);
    if (n>=0) {
        vn.insert(ai,n+int(after));
    }
}

void Mesh::erase(int const xi,int const vi)
{
    // Remove xi from vi's neighborhood.
    IndexArray& vn=neighbors[vi];
    int n=vn.find(xi);
    if (n>=0) {
        vn.remove(n);
    }
}

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

} // namespace model

} // namespace gale
