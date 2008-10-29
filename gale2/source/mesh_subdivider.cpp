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

void Mesh::Subdivider::Polyhedral(Mesh& mesh,int steps)
{
    while (steps-->0) {
        Mesh orig=mesh;

        // Store the index of the first new vertex.
        int x0i=orig.vertices.getSize();

        // Loop over all vertices in the base mesh.
        for (int vi=0;vi<x0i;++vi) {
            IndexArray const& vn=orig.neighbors[vi];
            Vec3f const& v=orig.vertices[vi];

            // Loop over v's neighborhood.
            for (int n=0;n<vn.getSize();++n) {
                int ui=vn[n];
                Vec3f const& u=orig.vertices[ui];

                // Be sure to walk each pair of vertices, i.e. edge, only once.
                // Use the address in memory to define a relation on the
                // universe of vertices.
                if (&u<&v) {
                    continue;
                }

                // Add a new vertex as the arithmetic average of its two neighbors.
                mesh.insert(ui,vi,(u+v)*0.5f);
            }
        }

        orig=mesh;
        assignNeighbors(orig,mesh,x0i);
    }
}

void Mesh::Subdivider::Butterfly(Mesh& mesh,int steps)
{
    while (steps-->0) {
        Mesh orig=mesh;

        // Store the index of the first new vertex.
        int x0i=orig.vertices.getSize();

        // Loop over all vertices in the base mesh.
        for (int vi=0;vi<x0i;++vi) {
            IndexArray const& vn=orig.neighbors[vi];
            Vec3f const& v=orig.vertices[vi];

            // Loop over v's neighborhood.
            for (int n=0;n<vn.getSize();++n) {
                int ui=vn[n];
                Vec3f const& u=orig.vertices[ui];

                // Be sure to walk each pair of vertices, i.e. edge, only once.
                // Use the address in memory to define a relation on the
                // universe of vertices.
                if (&u<&v) {
                    continue;
                }

                Vec3f x = v*0.5f + u*0.5f
                        + orig.vertices[orig.nextTo(ui,vi)]   * 0.125f  + orig.vertices[orig.prevTo(ui,vi)]   * 0.125f
                        - orig.vertices[orig.nextTo(ui,vi,2)] * 0.0625f - orig.vertices[orig.prevTo(ui,vi,2)] * 0.0625f
                        - orig.vertices[orig.nextTo(vi,ui,2)] * 0.0625f - orig.vertices[orig.prevTo(vi,ui,2)] * 0.0625f;

                // Add a new vertex as the arithmetic average of its two neighbors.
                mesh.insert(ui,vi,x);
            }
        }

        orig=mesh;
        assignNeighbors(orig,mesh,x0i);
    }
}

void Mesh::Subdivider::Loop(Mesh& mesh,int steps,bool move)
{
    while (steps-->0) {
        Mesh orig=mesh;

        // Store the index of the first new vertex.
        int x0i=orig.vertices.getSize();

        // Loop over all vertices in the base mesh.
        for (int vi=0;vi<x0i;++vi) {
            IndexArray const& vn=orig.neighbors[vi];
            Vec3f const& v=orig.vertices[vi];

            int valence=vn.getSize();
            double weight=::pow(0.375+0.25*::cos(2.0*Constd::PI()/valence),2.0)+0.375;

            Vec3f q=Vec3f::ZERO();

            // Loop over v's neighborhood.
            for (int n=0;n<vn.getSize();++n) {
                int ui=vn[n];
                Vec3f const& u=orig.vertices[ui];

                q+=u;

                // Be sure to walk each pair of vertices, i.e. edge, only once.
                // Use the address in memory to define a relation on the
                // universe of vertices.
                if (&u<&v) {
                    continue;
                }

                Vec3f x = v*0.375f + u*0.375f
                        + orig.vertices[orig.nextTo(ui,vi)]*0.125f
                        + orig.vertices[orig.prevTo(ui,vi)]*0.125f;

                // Add a new vertex as calculated from its neighbors.
                mesh.insert(ui,vi,x);
            }

            if (move) {
                // Modify the existing vertex as calculated from the weighted q
                // vertex.
                q/=static_cast<float>(valence);
                mesh.vertices[vi]=lerp(q,v,weight);
            }
        }

        orig=mesh;
        assignNeighbors(orig,mesh,x0i);
    }
}

void Mesh::Subdivider::Sqrt3(Mesh& mesh,int steps,bool move)
{
    while (steps-->0) {
        Mesh orig=mesh;

        // Store the index of the first new vertex.
        int x0i=orig.vertices.getSize();

        // Loop over all vertices in the base mesh.
        for (int vi=0;vi<x0i;++vi) {
            IndexArray const& vn=orig.neighbors[vi];
            Vec3f const& v=orig.vertices[vi];

            int valence=vn.getSize();
            float weight=(4.0f-2.0f*::cos(2.0f*Constf::PI()/valence))/9.0f;

            if (move) {
                mesh.vertices[vi]*=1.0f-weight;
            }

            // Loop over v's neighborhood.
            for (int n=0;n<vn.getSize();++n) {
                int ui=vn[n];
                Vec3f const& u=orig.vertices[ui];

                if (move) {
                    mesh.vertices[vi]+=u*(weight/valence);
                }

                int ti=orig.nextTo(ui,vi);
                Vec3f const& t=orig.vertices[ti];

                // Be sure to walk each pair of vertices, i.e. edge, only once.
                // Use the address in memory to define a relation on the
                // universe of vertices.
                if (&u<&v || &t<&v) {
                    continue;
                }

                // Insert a new vertex at the face center.
                Vec3f c=(v+u+t)/3.0f;
                int ci=mesh.vertices.insert(c);

                // Connect the new vertex to the face vertices.
                unsigned int cn[]={vi,ui,ti};
                mesh.neighbors.insert(cn);

                mesh.splice(ci,ui,vi);
                mesh.splice(ci,ti,ui);
                mesh.splice(ci,vi,ti);
            }
        }

        // Loop over all vertices in the base mesh.
        for (int vi=0;vi<x0i;++vi) {
            IndexArray const& vn=orig.neighbors[vi];
            Vec3f const& v=orig.vertices[vi];

            // Loop over v's neighborhood.
            for (int n=0;n<vn.getSize();++n) {
                int ui=vn[n];
                Vec3f const& u=orig.vertices[ui];

                // Be sure to walk each pair of vertices, i.e. edge, only once.
                // Use the address in memory to define a relation on the
                // universe of vertices.
                if (&u<&v) {
                    continue;
                }

                // Flip the base mesh's edges in the subdivided mesh.
                int ai=mesh.nextTo(ui,vi);
                int bi=mesh.prevTo(ui,vi);

                mesh.splice(ai,ui,bi);
                mesh.splice(bi,vi,ai);

                mesh.erase(ui,vi);
                mesh.erase(vi,ui);
            }
        }
    }
}

void Mesh::Subdivider::assignNeighbors(Mesh const& orig,Mesh& mesh,int x0i)
{
    for (int vi=x0i;vi<orig.vertices.getSize();++vi) {
        IndexArray& vn=mesh.neighbors[vi];

        // Get any neighbor of v, just pick the first one.
        int ai=vn[0];
        int bi=orig.nextTo(ai,vi);

        // ai and bi are already neighbors.
        vn.insert(orig.nextTo(vi,ai),0);
        vn.insert(orig.prevTo(vi,ai),2);
        vn.insert(orig.nextTo(vi,bi),3);
        vn.insert(orig.prevTo(vi,bi));
    }
}

} // namespace model

} // namespace gale
