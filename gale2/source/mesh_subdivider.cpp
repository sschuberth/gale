/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <https://sourceforge.net/projects/gale-opengl/>.
 *
 * Copyright (C) 2005-2011  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
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

void Mesh::Subdivider::Polyhedral(Mesh& mesh,int steps,float const scale)
{
    while (steps-->0) {
        Mesh orig=mesh;
        VectorArray const& ov=orig.vertices;

        // Store the index of the first new vertex.
        int x0i=ov.getSize();

        // Loop over all vertices in the base mesh.
        for (int vi=0;vi<x0i;++vi) {
            IndexArray const& vn=orig.neighbors[vi];
            Vec3f const& v=ov[vi];

            // Loop over v's neighborhood.
            for (int n=0;n<vn.getSize();++n) {
                int ui=vn[n];
                Vec3f const& u=ov[ui];

                // Be sure to walk each pair of vertices, i.e. edge, only once.
                // Use the address in memory to define a relation on the
                // universe of vertices.
                if (&u<&v) {
                    continue;
                }

                // Add a new vertex as the arithmetic average of its two neighbors.
                Vec3f a=u+v;
                if (scale==0.0f) {
                    mesh.insert(ui,vi,a*0.5f);
                }
                else {
                    mesh.insert(ui,vi,(~a)*scale);
                }
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
        VectorArray const& ov=orig.vertices;

        // Store the index of the first new vertex.
        int x0i=ov.getSize();

        // Loop over all vertices in the base mesh.
        for (int vi=0;vi<x0i;++vi) {
            IndexArray const& vn=orig.neighbors[vi];
            Vec3f const& v=ov[vi];

            // Loop over v's neighborhood.
            for (int n=0;n<vn.getSize();++n) {
                int ui=vn[n];
                Vec3f const& u=ov[ui];

                // Be sure to walk each pair of vertices, i.e. edge, only once.
                // Use the address in memory to define a relation on the
                // universe of vertices.
                if (&u<&v) {
                    continue;
                }

                Vec3f x = v*0.5f + u*0.5f
                        + ov[orig.nextTo(ui,vi)]   * 0.125f  + ov[orig.prevTo(ui,vi)]   * 0.125f
                        - ov[orig.nextTo(ui,vi,2)] * 0.0625f - ov[orig.prevTo(ui,vi,2)] * 0.0625f
                        - ov[orig.nextTo(vi,ui,2)] * 0.0625f - ov[orig.prevTo(vi,ui,2)] * 0.0625f;

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

void Mesh::Subdivider::Loop(Mesh& mesh,int steps,bool const move)
{
    while (steps-->0) {
        Mesh orig=mesh;
        VectorArray& ov=orig.vertices;

        // Store the index of the first new vertex.
        int x0i=ov.getSize();

        // Loop over all vertices in the base mesh.
        for (int vi=0;vi<x0i;++vi) {
            IndexArray const& vn=orig.neighbors[vi];
            Vec3f const& v=ov[vi];

            // Calculate variables for moving the existing vertices.
            int valence=vn.getSize();
            float weight=pow(0.375f + 0.25f*cos(2.0f*Constf::PI()/valence),2.0f) + 0.375f;

            Vec3f q=Vec3f::ZERO();

            // Loop over v's neighborhood.
            for (int n=0;n<vn.getSize();++n) {
                int ui=vn[n];
                Vec3f const& u=ov[ui];

                q+=u;

                // Be sure to walk each pair of vertices, i.e. edge, only once.
                // Use the address in memory to define a relation on the
                // universe of vertices.
                if (&u<&v) {
                    continue;
                }

                Vec3f x = v*0.375f + u*0.375f
                        + ov[orig.nextTo(ui,vi)]*0.125f
                        + ov[orig.prevTo(ui,vi)]*0.125f;

                // Add a new vertex as calculated from its neighbors.
                mesh.insert(ui,vi,x);
            }

            if (move) {
                q/=static_cast<float>(valence);

                // Move the existing vertices.
                mesh.vertices[vi]=lerp(q,v,weight);
            }
        }

        orig=mesh;
        assignNeighbors(orig,mesh,x0i);
    }
}

void Mesh::Subdivider::Sqrt3(Mesh& mesh,int steps,bool const move)
{
    while (steps-->0) {
        Mesh orig=mesh;
        VectorArray const& ov=orig.vertices;

        // Store the index of the first new vertex.
        int x0i=ov.getSize();

        // Loop over all vertices in the base mesh.
        for (int vi=0;vi<x0i;++vi) {
            IndexArray const& vn=orig.neighbors[vi];
            Vec3f const& v=ov[vi];

            // Calculate variables for moving the existing vertices.
            int valence=vn.getSize();
            float weight=(4.0f - 2.0f*cos(2.0f*Constf::PI()/valence)) / 9.0f;

            if (move) {
                // Move the existing vertices.
                mesh.vertices[vi]*=1.0f-weight;
            }

            // Loop over v's neighborhood.
            for (int n=0;n<vn.getSize();++n) {
                int ui=vn[n];
                Vec3f const& u=ov[ui];

                if (move) {
                    // Move the existing vertices.
                    mesh.vertices[vi]+=u*(weight/valence);
                }

                int ti=orig.nextTo(ui,vi);
                Vec3f const& t=ov[ti];

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
            Vec3f const& v=ov[vi];

            // Loop over v's neighborhood.
            for (int n=0;n<vn.getSize();++n) {
                int ui=vn[n];
                Vec3f const& u=ov[ui];

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

void Mesh::Subdivider::CatmullClark(Mesh& mesh,int steps)
{
    while (steps-->0) {
        Mesh orig=mesh;
        VectorArray const& ov=orig.vertices;

        // Store the index of the first new vertex.
        int x0i=ov.getSize();

        // Loop over all vertices in the base mesh.
        for (int vi=0;vi<x0i;++vi) {
            IndexArray const& vn=orig.neighbors[vi];
            Vec3f const& v=ov[vi];

            int valence=vn.getSize();
            float beta=3.0f/(2.0f*valence);
            float gamma=1.0f/(4.0f*valence);

            // Move the existing vertices.
            mesh.vertices[vi]*=1.0f-beta-gamma;

            beta/=valence;
            gamma/=valence;

            // Loop over v's neighborhood.
            for (int n=0;n<vn.getSize();++n) {
                int pi=vn[n];
                Vec3f const& p=ov[pi];

                // Move the existing vertices.
                mesh.vertices[vi]+=p*beta;
                mesh.vertices[vi]+=ov[orig.nextTo(vi,pi)]*gamma;

                // Be sure to walk each pair of vertices, i.e. edge, only once.
                // Use the address in memory to define a relation on the
                // universe of vertices.
                if (&v<&p) {
                    continue;
                }

                // Insert a new vertex on each base mesh's edge.
                Vec3f const& a=ov[orig.nextTo(pi,vi)];
                Vec3f const& b=ov[orig.prevTo(pi,vi)];
                Vec3f const& c=ov[orig.nextTo(vi,pi)];
                Vec3f const& d=ov[orig.prevTo(vi,pi)];

                mesh.insert(vi,pi,(v+p)*0.375f + (a+b+c+d)*0.0625f);
            }
        }

        // To calculate the face center vertices the base mesh's vertices are
        // needed, but the current mesh's neighborhood is needed to connect them
        // to the edge vertices, so just copy the neighborhood here.
        orig.neighbors=mesh.neighbors;

        // Loop over all vertices in the base mesh.
        for (int vi=0;vi<x0i;++vi) {
            IndexArray const& vn=orig.neighbors[vi];
            Vec3f const& v=ov[vi];

            // Loop over v's neighborhood.
            for (int n=0;n<vn.getSize();++n) {
                int pi=vn[n];

                // Find the vertices that complete the quadrilaterals, but since
                // these paths can fit on the mesh in several ways, test to make
                // sure that the paths terminate at the correct vertices.

                int xi=orig.prevTo(pi,vi);
                int ai=orig.prevTo(vi,xi);
                Vec3f const& a=ov[ai];
                if (&v<&a) {
                    continue;
                }

                int yi=orig.prevTo(xi,ai);
                int bi=orig.prevTo(ai,yi);
                Vec3f const& b=ov[bi];
                if (&v<&b) {
                    continue;
                }

                int zi=orig.prevTo(yi,bi);
                int ci=orig.prevTo(bi,zi);
                Vec3f const& c=ov[ci];
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

void Mesh::Subdivider::DooSabin(Mesh& mesh,int steps)
{
    IndexArray polygon;

    while (steps-->0) {
        Mesh orig=mesh;
        VectorArray const& ov=orig.vertices;

        // Store the index of the first new vertex.
        int x0i=ov.getSize();

        // Loop over all vertices in the base mesh.
        for (int vi=0;vi<x0i;++vi) {
            IndexArray const& vn=orig.neighbors[vi];
            Vec3f const& v=ov[vi];

            // Loop over v's neighborhood.
            for (int n=0;n<vn.getSize();++n) {
                int pi=vn[n];
                Vec3f const& p=ov[pi];

                int o=orig.orbit(vi,pi,polygon);

                Vec3f t;

                // The orbit is a quadrilateral.
                if (o==4) {
                    Vec3f const& q=ov[polygon[3]];
                    Vec3f const& r=ov[polygon[2]];
                    t=v*0.5625f + p*0.1875f + q*0.1875f + r*0.0625f;
                }
                // The orbit is an arbitrary polygon.
                else {
                    t=v*(0.25f + 1.25f/o);

                    int i=0;
                    while (++i<o) {
                        Vec3f const& a=ov[polygon[i]];
                        t+=a*((3.0f + 2.0f*cos(2.0f*Constf::PI()*static_cast<float>(i)/o))/(4.0f*o));
                    }
                }

                // Add the new vertex ...
                int ti=mesh.vertices.insert(t);

                // ... and connect it to the mesh so it can be reached below.
                mesh.splice(ti,pi,vi);
            }
        }

        // Loop over all vertices in the base mesh.
        for (int vi=0;vi<x0i;++vi) {
            IndexArray const& vn=orig.neighbors[vi];

            // Loop over v's neighborhood.
            for (int n=0;n<vn.getSize();++n) {
                int pi=vn[n];

                // Get the index to the newly inserted vertex.
                int ti=mesh.nextTo(pi,vi);

                // Prepare that the base mesh's vertices will be deleted and
                // reduce the indices by the base mesh vertex count.
                unsigned int tn[]={
                    mesh.prevTo(vi,pi)                 - x0i
                ,   mesh.nextTo(vi,mesh.nextTo(ti,vi)) - x0i
                ,   mesh.nextTo(mesh.nextTo(ti,vi),vi) - x0i
                ,   mesh.prevTo(pi,vi)                 - x0i
                };

                mesh.neighbors.insert(tn);
            }
        }

        // Delete all base mesh vertices and their neighborhoods.
        mesh.vertices.remove(0,x0i);
        mesh.neighbors.remove(0,x0i);
    }
}

/*
 * Helper methods
 */

void Mesh::Subdivider::assignNeighbors(Mesh const& orig,Mesh& mesh,int const x0i)
{
    for (int vi=x0i;vi<orig.vertices.getSize();++vi) {
        IndexArray& vn=mesh.neighbors[vi];

        // Get any neighbor of v, just pick the first one.
        int ai=vn[0];
        int bi=orig.nextTo(ai,vi);

        vn.setCapacity(vn.getSize()+4);

        // ai and bi are already neighbors.
        vn.insert(orig.nextTo(vi,ai),0);
        vn.insert(orig.prevTo(vi,ai),2);
        vn.insert(orig.nextTo(vi,bi),3);
        vn.insert(orig.prevTo(vi,bi));
    }
}

} // namespace model

} // namespace gale
