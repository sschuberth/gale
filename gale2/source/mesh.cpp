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

int Mesh::orbit(int ai,int bi,IndexArray& polygon)
{
    polygon.clear();
    polygon.insert(ai);

    // Add the vertex immediately following ai in the neighborhood of bi until
    // we return to the starting vertex.
    while (true) {
        polygon.insert(bi);

        int ci=prevTo(ai,bi);
        if (ci==polygon[0]) {
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
