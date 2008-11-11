/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
 *
 * Copyright (C) 2005-2008  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
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

int Mesh::nextTo(int xi,int vi,int steps) const
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

int Mesh::prevTo(int xi,int vi,int steps) const
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
    polygon.clear();
    polygon.insert(ai);

    // Add the vertex immediately following ai in the neighborhood of bi until
    // we return to the starting vertex.
    for (;;) {
        polygon.insert(bi);

        int ci=prevTo(ai,bi);
        if (ci<0 || static_cast<unsigned int>(ci)==polygon[0]) {
            break;
        }

        ai=bi;
        bi=ci;
    }

    return polygon.getSize();
}

int Mesh::insert(int ai,int bi,Vec3f const& x)
{
    // Add a new vertex at index xi.
    int xi=vertices.insert(x);

    unsigned int xn[]={ai,bi};
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

void Mesh::splice(int ai,int xi,int vi,bool after)
{
    // Insert ai after or before xi in vi's neighborhood.
    IndexArray& vn=neighbors[vi];
    int n=vn.find(xi);
    if (n>=0) {
        vn.insert(ai,n+int(after));
    }
}

void Mesh::erase(int xi,int vi)
{
    // Remove xi from vi's neighborhood.
    IndexArray& vn=neighbors[vi];
    int n=vn.find(xi);
    if (n>=0) {
        vn.remove(n);
    }
}

} // namespace model

} // namespace gale
