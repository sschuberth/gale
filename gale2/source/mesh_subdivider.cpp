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

/*
 * Interpolating subdivision schemes
 */

void Mesh::Subdivider::Polyhedral(Mesh& mesh,int steps,float const scale)
{
    while (steps-->0) {
        int const num_verts_orig=mesh.numVertices();

        // Avoid reallocations by setting the capacity to the final size.
        int const num_verts_step=num_verts_orig+mesh.numEdges();
        mesh.vertices.setCapacity(num_verts_step);
        mesh.neighbors.setCapacity(num_verts_step);

        // Insert a new vertex on each edge.
        Mesh::EdgeIterator edge_current=mesh.beginEdges();
        Mesh::EdgeIterator edge_end=mesh.endEdges();
        while (edge_current!=edge_end) {
            int ai=edge_current.indexA();
            int bi=edge_current.indexB();

            if (ai<num_verts_orig && bi<num_verts_orig) {
                // Calculate the new vertex position.
                Vec3f v=edge_current.vertexA()+edge_current.vertexB();

                if (scale==0.0f) {
                    v=v*0.5f;
                }
                else {
                    v=(~v)*scale;
                }

                mesh.insert(v,ai,bi,6);
            }

            ++edge_current;
        }

        interconnectEdgeVertices(mesh,num_verts_orig);
    }
}

void Mesh::Subdivider::Butterfly(Mesh& mesh,int steps)
{
    EdgeVertexList edge_vertices;

    while (steps-->0) {
        // Avoid reallocations by setting the capacity to the final size.
        int const num_verts_step=mesh.numVertices()+mesh.numEdges();
        mesh.vertices.setCapacity(num_verts_step);
        mesh.neighbors.setCapacity(num_verts_step);

        edge_vertices.setSize(mesh.numEdges());
        int i=0;

        // Generate a list of new edge vertices.
        Mesh::EdgeIterator edge_current=mesh.beginEdges();
        Mesh::EdgeIterator edge_end=mesh.endEdges();
        while (edge_current!=edge_end) {
            // Get the neighbor vertex indices.
            EdgeVertex& s=edge_vertices[i++];
            s.a=edge_current.indexA();
            s.b=edge_current.indexB();

            // Calculate the new vertex position.
            s.v = ( edge_current.vertexA()                + edge_current.vertexB()                ) * 0.5f
                + ( mesh.vertices[mesh.nextTo(s.a,s.b)]   + mesh.vertices[mesh.prevTo(s.a,s.b)]   ) * 0.125f
                - ( mesh.vertices[mesh.nextTo(s.a,s.b,2)] + mesh.vertices[mesh.prevTo(s.a,s.b,2)]
                +   mesh.vertices[mesh.nextTo(s.b,s.a,2)] + mesh.vertices[mesh.prevTo(s.b,s.a,2)] ) * 0.0625f;

            ++edge_current;
        }

        spliceEdgeVertices(mesh,edge_vertices);
    }
}

/*
 * Approximating subdivision schemes
 */

void Mesh::Subdivider::Loop(Mesh& mesh,int steps,bool const move)
{
    EdgeVertexList edge_vertices;
    VectorArray avg_neighbors;

    // Pre-calculate weights for common valences.
    static float weights[1+6]={0};
    if (move && !weights[0]) {
        weights[0]=1;
        for (int i=1;i<G_ARRAY_LENGTH(weights);++i) {
            weights[i]=pow(0.375f + 0.25f*cos(2.0f*Constf::PI()/i),2.0f) + 0.375f;
        }
    }

    while (steps-->0) {
        edge_vertices.setSize(mesh.numEdges());
        int i=0;

        if (move) {
            avg_neighbors.setSize(mesh.numVertices());
            memset(avg_neighbors,0,avg_neighbors.getSize()*sizeof(VectorArray::Type));
        }

        // Generate a list of new edge vertices.
        Mesh::EdgeIterator edge_current=mesh.beginEdges();
        Mesh::EdgeIterator edge_end=mesh.endEdges();
        while (edge_current!=edge_end) {
            // Get the neighbor vertex indices.
            EdgeVertex& s=edge_vertices[i++];
            s.a=edge_current.indexA();
            s.b=edge_current.indexB();

            // Calculate the new vertex position.
            Vec3f const& a=edge_current.vertexA();
            Vec3f const& b=edge_current.vertexB();
            s.v = ( a                                   + b              ) * 0.375f
                + ( mesh.vertices[mesh.nextTo(s.a,s.b)] + mesh.vertices[mesh.prevTo(s.a,s.b)] ) * 0.125f;

            if (move) {
                avg_neighbors[s.a]+=b;
                avg_neighbors[s.b]+=a;
            }

            ++edge_current;
        }

        if (move) {
            // Calculate new positions for original vertices.
            for (i=0;i<mesh.numVertices();++i) {
                int const valence=mesh.neighbors[i].getSize();

                // Average the neighboring vertices.
                Vec3f& avg=avg_neighbors[i];
                avg/=static_cast<float>(valence);

                // INFO: If higher valences need to be supported, calculate the weight on-the-fly here.
                G_ASSERT(valence<G_ARRAY_LENGTH(weights))

                Vec3f& v=mesh.vertices[i];
                v=lerp(v,avg,weights[valence]);
            }
        }

        spliceEdgeVertices(mesh,edge_vertices);
    }
}

void Mesh::Subdivider::Sqrt3(Mesh& mesh,int steps,bool const move)
{
    VectorArray avg_neighbors;

    // Pre-calculate weights for common valences.
    static float weights[1+6]={0};
    if (move && !weights[0]) {
        weights[0]=1;
        for (int i=1;i<G_ARRAY_LENGTH(weights);++i) {
            weights[i]=(4.0f - 2.0f*cos(2.0f*Constf::PI()/i)) / 9.0f;
        }
    }

    while (steps-->0) {
        int face_vertices_start=mesh.numVertices();

        if (move) {
            avg_neighbors.setSize(mesh.numVertices());
            memset(avg_neighbors,0,avg_neighbors.getSize()*sizeof(VectorArray::Type));
        }

        // Avoid memory reallocations when adding vertices and their neighbors.
        int num_faces=mesh.numFaces();
        mesh.vertices.setCapacity(mesh.vertices.getSize()+num_faces);
        mesh.neighbors.setCapacity(mesh.neighbors.getSize()+num_faces);

        // Add the new face vertices.
        Mesh::PrimitiveIterator prim_current=mesh.beginPrimitives();
        Mesh::PrimitiveIterator prim_end=mesh.endPrimitives();
        while (prim_current!=prim_end) {
            int prim_vertices=prim_current.indices().getSize();

            if (prim_vertices==3) {
                // Connect the vertex-to-be to its face.
                mesh.neighbors.insert(prim_current.indices());

                // Calculate the vertex by averaging the face vertices.
                mesh.vertices.insert(mesh.average(prim_current.indices()));

                if (move) {
                    for (int i=0;i<prim_vertices;++i) {
                        int pi=prim_current.indices()[i];

                        if (avg_neighbors[pi]==Vec3f::ZERO()) {
                            avg_neighbors[pi]=mesh.average(mesh.neighbors[pi]);
                        }
                    }
                }
            }

            ++prim_current;
        }

        if (move) {
            // Calculate new positions for original vertices.
            for (int i=0;i<face_vertices_start;++i) {
                int const valence=mesh.neighbors[i].getSize();

                // INFO: If higher valences need to be supported, calculate the weight on-the-fly here.
                G_ASSERT(valence<G_ARRAY_LENGTH(weights))

                Vec3f& v=mesh.vertices[i];
                v=lerp(v,avg_neighbors[i],weights[valence]);
            }
        }

        // Connect the faces to the new vertices.
        for (int i=face_vertices_start;i<mesh.vertices.getSize();++i) {
            IndexArray const& n=mesh.neighbors[i];

            mesh.splice(i,n[1],n[0]);
            mesh.splice(i,n[2],n[1]);
            mesh.splice(i,n[0],n[2]);
        }

        // Flip the original edges to connect new vertices from different faces.
        Mesh::EdgeIterator edge_current=mesh.beginEdges();
        Mesh::EdgeIterator edge_end=mesh.endEdges();
        while (edge_current!=edge_end) {
            int ai=edge_current.indexA();
            int bi=edge_current.indexB();

            if (ai<face_vertices_start && bi<face_vertices_start) {
                int ni=mesh.nextTo(ai,bi);
                int pi=mesh.prevTo(ai,bi);

                mesh.splice(ni,ai,pi);
                mesh.splice(pi,bi,ni);

                mesh.separate(ai,bi);
            }

            ++edge_current;
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

                mesh.insert((v+p)*0.375f + (a+b+c+d)*0.0625f,vi,pi);
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
    IndexArray primitive;

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

                orig.orbit(vi,pi,primitive);
                int o=primitive.getSize();

                Vec3f t;

                // The orbit is a quadrilateral.
                if (o==4) {
                    Vec3f const& q=ov[primitive[3]];
                    Vec3f const& r=ov[primitive[2]];
                    t=v*0.5625f + p*0.1875f + q*0.1875f + r*0.0625f;
                }
                // The orbit is an arbitrary polygon.
                else {
                    t=v*(0.25f + 1.25f/o);

                    int i=0;
                    while (++i<o) {
                        Vec3f const& a=ov[primitive[i]];
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

void Mesh::Subdivider::interconnectEdgeVertices(Mesh& mesh,int edge_vertices_start)
{
    for (int i=edge_vertices_start;i<mesh.numVertices();++i) {
        IndexArray& n=mesh.neighbors[i];

        // The edge vertices just have 2 neighbors as yet ...
        int a=n[0];
        int b=n[1];

        /// ... but will get 4 more now.
        n.setSize(6);
        n[3]=b;

        // Connect to vertices on adjacent edges in the "left" face.
        n[1]=mesh.prevTo(i,a);
        n[2]=mesh.nextTo(i,b);

        // Connect to vertices on adjacent edges in the "right" face.
        n[4]=mesh.prevTo(i,b);
        n[5]=mesh.nextTo(i,a);
    }
}

void Mesh::Subdivider::spliceEdgeVertices(Mesh& mesh,EdgeVertexList const& edge_vertices)
{
    int edge_vertices_start=mesh.numVertices();

    // Insert the new edge vertices.
    for (int i=0;i<edge_vertices.getSize();++i) {
        EdgeVertex const& s=edge_vertices[i];
        mesh.insert(s.v,s.a,s.b,6);
    }

    interconnectEdgeVertices(mesh,edge_vertices_start);
}

} // namespace model

} // namespace gale
