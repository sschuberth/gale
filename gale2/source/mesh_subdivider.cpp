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

/*
 * Interpolating subdivision schemes
 */

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

/*
 * Approximating subdivision schemes
 */

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

            // Only calculate these if needed.
            int valence;
            double weight;

            if (move) {
                valence=vn.getSize();
                weight=::pow(0.375+0.25*::cos(2.0*Constd::PI()/valence),2.0)+0.375;
            }

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

            // Only calculate these if needed.
            int valence;
            float weight;

            if (move) {
                valence=vn.getSize();
                weight=(4.0f-2.0f*::cos(2.0f*Constf::PI()/valence))/9.0f;
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

                // Insert a new vertex at the base mesh's face center.
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

void Mesh::Subdivider::CatmullClark(Mesh& mesh,int steps,bool move)
{
    while (steps-->0) {
        Mesh orig=mesh;

        // Store the index of the first new vertex.
        int x0i=orig.vertices.getSize();

        // Loop over all vertices in the base mesh.
        for (int vi=0;vi<x0i;++vi) {
            IndexArray const& vn=orig.neighbors[vi];
            Vec3f const& v=orig.vertices[vi];

            // Only calculate these if needed.
            int valence;
            float beta;
            float gamma;

            if (move) {
                valence=vn.getSize();
                beta=3.0f/(2.0f*valence);
                gamma=1.0f/(4.0f*valence);

                mesh.vertices[vi]*=1.0f-beta-gamma;

                beta/=valence;
                gamma/=valence;
            }

            // Loop over v's neighborhood.
            for (int n=0;n<vn.getSize();++n) {
                int pi=vn[n];
                Vec3f const& p=orig.vertices[pi];

                // Set the new position of existing vertices.
                if (move) {
                    mesh.vertices[vi]+=p*beta;
                    mesh.vertices[vi]+=orig.vertices[orig.nextTo(vi,pi)]*gamma;
                }

                // Be sure to walk each pair of vertices, i.e. edge, only once.
                // Use the address in memory to define a relation on the
                // universe of vertices.
                if (&v<&p) {
                    continue;
                }

                // Insert a new vertex on each base mesh's edge.
                Vec3f const& a=orig.vertices[orig.nextTo(pi,vi)];
                Vec3f const& b=orig.vertices[orig.prevTo(pi,vi)];
                Vec3f const& c=orig.vertices[orig.nextTo(vi,pi)];
                Vec3f const& d=orig.vertices[orig.prevTo(vi,pi)];

                mesh.insert(vi,pi,(v+p)*0.375f+(a+b+c+d)*0.0625f);
            }
        }

        // To calculate the face center vertices the base mesh's vertices are
        // needed, but the current mesh's neighborhood is needed to connect them
        // to the edge vertices, so just copy the neighborhood here.
        orig.neighbors=mesh.neighbors;

        // Loop over all vertices in the base mesh.
        for (int vi=0;vi<x0i;++vi) {
            IndexArray const& vn=orig.neighbors[vi];
            Vec3f const& v=orig.vertices[vi];

            // Loop over v's neighborhood.
            for (int n=0;n<vn.getSize();++n) {
                int pi=vn[n];

                // Find the vertices that complete the quadrilaterals, but since
                // these paths can fit on the mesh in several ways, test to make
                // sure that the paths terminate at the correct vertices.

                int xi=orig.prevTo(pi,vi);
                int ai=orig.prevTo(vi,xi);
                Vec3f const& a=orig.vertices[ai];
                if (&v<&a) {
                    continue;
                }

                int yi=orig.prevTo(xi,ai);
                int bi=orig.prevTo(ai,yi);
                Vec3f const& b=orig.vertices[bi];
                if (&v<&b) {
                    continue;
                }

                int zi=orig.prevTo(yi,bi);
                int ci=orig.prevTo(bi,zi);
                Vec3f const& c=orig.vertices[ci];
                if (&v<&c) {
                    continue;
                }

                // Insert a new vertex at the base mesh's face center.
                int ui=mesh.vertices.insert((v+a+b+c)*0.25f);

                // Connect the new vertex to the face vertices.
                unsigned int un[]={pi,xi,yi,zi};
                mesh.neighbors.insert(un);

                mesh.splice(ui,ai,xi);
                mesh.splice(ui,bi,yi);
                mesh.splice(ui,ci,zi);
                mesh.splice(ui,vi,pi);
            }
        }
    }
}

/*
 * Helper methods
 */

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
