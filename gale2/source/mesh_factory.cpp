/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
 *
 * Copyright (C) 2005-2009  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
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

#include "gale/math/hmatrix4.h"

using namespace gale::math;

namespace gale {

namespace model {

Mesh* Mesh::Factory::Tetrahedron()
{
    // http://mathworld.wolfram.com/Tetrahedron.html
    static float const a=0.5f;

    static Vec3f const vertices[]={
        // Vertices on the lower edge, which is orthogonal to the upper edge.
        Vec3f(-a,-a,+a),
        Vec3f(+a,-a,-a),

        // Vertices on the upper edge, which is orthogonal to the lower edge.
        Vec3f(+a,+a,+a),
        Vec3f(-a,+a,-a)
    };

    // Create a mesh from the static arrays.
    Mesh* m=new Mesh(vertices);

    // Make all vertices neighbors whose distance matches the required edge length.
    static float const e=::sqrt(8*a*a);
    populateNeighborhood(m,e,3);

    return m;
}

Mesh* Mesh::Factory::Octahedron()
{
    // http://mathworld.wolfram.com/Octahedron.html
    static float const a=0.5f;
    static float const b=1.0f/(2.0f*::sqrt(2.0f));

    static Vec3f const vertices[]={
        // Top and bottom vertices.
        Vec3f( 0,+a, 0),
        Vec3f( 0,-a, 0),

        // Square base vertices.
        Vec3f(+b, 0,+b),
        Vec3f(+b, 0,-b),
        Vec3f(-b, 0,-b),
        Vec3f(-b, 0,+b)
    };

    // Create a mesh from the static arrays.
    Mesh* m=new Mesh(vertices);

    // Make all vertices neighbors whose distance matches the required edge length.
    static float const e=::sqrt(a*a+2*b*b);
    populateNeighborhood(m,e,4);

    return m;
}

Mesh* Mesh::Factory::Hexahedron()
{
    // http://mathworld.wolfram.com/Hexahedron.html
    static float const a=0.5f;

    static Vec3f const vertices[]={
        // Front face vertices.
        Vec3f(+a,+a,+a),
        Vec3f(-a,+a,+a),
        Vec3f(-a,-a,+a),
        Vec3f(+a,-a,+a),

        // Back face vertices.
        Vec3f(-a,+a,-a),
        Vec3f(+a,+a,-a),
        Vec3f(+a,-a,-a),
        Vec3f(-a,-a,-a)
    };

    // Create a mesh from the static vertex array. Due to the different
    // valences, define the neighborhood dynamically.
    Mesh* m=new Mesh(vertices);

    // Make all vertices neighbors whose distance matches the required edge length.
    static float const e=2*a;
    populateNeighborhood(m,e,3);

    return m;
}

Mesh* Mesh::Factory::Icosahedron()
{
    // http://mathworld.wolfram.com/Icosahedron.html
    static float const a=0.5f;
    static float const b=1.0f/(2.0f*Constf::GOLDEN_RATIO());

    static Vec3f const vertices[]={
        Vec3f( 0,-b,+a),
        Vec3f(-b,-a, 0),
        Vec3f(+b,-a, 0),
        Vec3f( 0,-b,-a),
        Vec3f( 0,+b,-a),
        Vec3f(+a, 0,-b),
        Vec3f(-a, 0,-b),
        Vec3f( 0,+b,+a),
        Vec3f(-a, 0,+b),
        Vec3f(+a, 0,+b),
        Vec3f(-b,+a, 0),
        Vec3f(+b,+a, 0)
    };

    // Create a mesh from the static vertex array. Instead of hard-coding the
    // neighborhood, do a brute-force search for the correct edge length.
    Mesh* m=new Mesh(vertices);

    // Make all vertices neighbors whose distance matches the required edge length.
    static float const e=::sqrt(2*(a*a-a*b+b*b));
    populateNeighborhood(m,e,5);

    return m;
}

Mesh* Mesh::Factory::Dodecahedron()
{
    // http://mathworld.wolfram.com/Dodecahedron.html
    static float const a=0.5f;
    static float const b=1.0f/(2.0f*Constf::GOLDEN_RATIO());
    static float const c=(2.0f-Constf::GOLDEN_RATIO())*0.5f;

    static Vec3f const vertices[]={
        Vec3f(-c, 0,-a),
        Vec3f(+c, 0,-a),
        Vec3f(+b,-b,-b),
        Vec3f( 0,-a,-c),
        Vec3f(-b,-b,-b),
        Vec3f(-b,+b,-b),
        Vec3f( 0,+a,-c),
        Vec3f(+b,+b,-b),
        Vec3f(-c, 0,+a),
        Vec3f(+c, 0,+a),
        Vec3f(+b,+b,+b),
        Vec3f( 0,+a,+c),
        Vec3f(-b,+b,+b),
        Vec3f(-b,-b,+b),
        Vec3f( 0,-a,+c),
        Vec3f(+b,-b,+b),
        Vec3f(-a,-c, 0),
        Vec3f(+a,-c, 0),
        Vec3f(+a,+c, 0),
        Vec3f(-a,+c, 0)
    };

    // Create a mesh from the static vertex array. Instead of hard-coding the
    // neighborhood, do a brute-force search for the correct edge length.
    Mesh* m=new Mesh(vertices);

    // Make all vertices neighbors whose distance matches the required edge length.
    static float const e=2*c;
    populateNeighborhood(m,e,3);

    return m;
}

Mesh* Mesh::Factory::Torus(float r1,float r2,int r1_segs,int r2_segs)
{
    VectorArray path(r1_segs);
    VectorArray profile(r2_segs);

    float a,d;
    int i;

    // Calculate points on the outer path.
    a=0;
    d=2*Constf::PI()/r1_segs;
    for (i=0;i<r1_segs;++i) {
        path[i]=Vec3f(::cos(a)*r1,::sin(a)*r1,0);
        a+=d;
    }

    // Calculate points on the inner profile.
    a=0;
    d=2*Constf::PI()/r2_segs;
    for (i=0;i<r2_segs;++i) {
        profile[i]=Vec3f(::cos(a)*r2,::sin(a)*r2,0);
        a+=d;
    }

    return Extrude(path,profile);
}

Mesh* Mesh::Factory::TorusKnot(float r1,float r2,int r1_segs,int r2_segs,float w,float h,int p,int q)
{
    VectorArray path(r1_segs);
    VectorArray profile(r2_segs);

    int i;

    // Calculate points on the outer path.
    for (i=0;i<r1_segs;++i) {
        float angle=2*Constf::PI()/r1_segs*i;

        // Calculate the cylindrical coordinates.
        float theta=angle*p;
        float r=r1+w*::cos(theta);
        float z=h*::sin(theta);

        // Convert to cartesian coordinates.
        float phi=angle*q;
        float x=r*::cos(phi);
        float y=r*::sin(phi);

        path[i]=Vec3f(x,y,z);
    }

    // Calculate points on the inner profile.
    float a=0;
    float d=2*Constf::PI()/r2_segs;
    for (i=0;i<r2_segs;++i) {
        profile[i]=Vec3f(::cos(a)*r2,::sin(a)*r2,0);
        a+=d;
    }

    return Extrude(path,profile);
}

Mesh* Mesh::Factory::Extrude(VectorArray const& path,VectorArray const& profile,bool closed)
{
    if (path.getSize()<2 || profile.getSize()<3) {
        return NULL;
    }

    Mesh* m=new Mesh(static_cast<int>(!closed)*2+path.getSize()*profile.getSize());

    // Pointers to the first and last vectors (Alpha and Omega :-) in the path.
    Vec3f const* const pA=&path[0];
    Vec3f const* const pO=&path[path.getSize()-1];

    // Pointer to the currently considered center vector in the path.
    Vec3f const* p=pA;

    // Index of the vertex currently being calculated.
    int vi=0;

    if (!closed) {
        // Add the first path vertex as the center of the start cut face.
        m->vertices[vi]=*pA;

        // All vertices of the start cut face are its neighbors.
        IndexArray& nA=m->neighbors[vi];
        nA.setSize(profile.getSize());
        for (int i=0;i<nA.getSize();++i) {
            nA[i]=2+i;
        }
        ++vi;

        // Add the first path vertex as the center of the end cut face.
        m->vertices[vi]=*pO;

        // All vertices of the end cut face are its neighbors.
        IndexArray& nO=m->neighbors[vi];
        nO.setSize(profile.getSize());
        for (int i=0;i<nO.getSize();++i) {
            nO[i]=m->vertices.getSize()-1-i;
        }
        ++vi;
    }

    for (int pi=vi;vi<m->vertices.getSize();pi+=profile.getSize()) {
        // Pointer to p's predecessor along the path.
        Vec3f const* a=(p==pA)?(closed?pO:p):p-1;

        // Pointer to p's successor along the path.
        Vec3f const* b=(p==pO)?(closed?pA:p):p+1;

        // Calculate the Frenet frame in p.
        Vec3f tangent=~(*b-*a);
        Vec3f binormal=~(tangent^(*a+*b));
        Vec3f normal=binormal^tangent;

        HMat4f frenet(binormal,normal,tangent,*p);

        for (int k=0;k<profile.getSize();++k) {
            // Transform the profile along the path.
            m->vertices[vi]=frenet*profile[k];

            // Connect the neighbors.
            IndexArray& vn=m->neighbors[vi];
            vn.setSize(6);

#define WRAP_I(x) wrap(x,m->vertices.getSize())
#define WRAP_K(x) wrap(x,profile.getSize())

            int piwk,n=0;

            piwk=pi+WRAP_K(k-1);
            vn[n++]=piwk;

            if (!closed && p==pO) {
                // Connect the end cut face vertices to the last path vertex.
                vn[n++]=1;
                vn.setSize(5);
            }
            else {
                vn[n++]=WRAP_I(piwk+profile.getSize());
                vn[n++]=WRAP_I(vi+profile.getSize());
            }

            piwk=pi+WRAP_K(k+1);
            vn[n++]=piwk;

            if (!closed && p==pA) {
                // Connect the start cut face vertices to the first path vertex.
                vn[n++]=0;
                vn.setSize(5);
            }
            else {
                vn[n++]=WRAP_I(piwk-profile.getSize());
                vn[n++]=WRAP_I(vi-profile.getSize());
            }

#undef WRAP_K
#undef WRAP_I

            ++vi;
        }

        ++p;
    }

    return m;
}

Mesh* Mesh::Factory::Normals(Preparer const& geom,float scale)
{
    if (!geom.getCompiledMesh()) {
        return NULL;
    }

    int n=geom.getCompiledMesh()->vertices.getSize();

    // Double the vertices and neighbors for the lines' endpoints.
    Mesh* m=new Mesh(n*2);

    // Copy the vertices to the normal mesh.
    memcpy(m->vertices,geom.getCompiledMesh()->vertices,n*sizeof(VectorArray::Type));

    for (int i=0,k=n;i<n;++i,++k) {
        // Calculate the endpoints by pointing from the vertex into the normal direction.
        m->vertices[k]=m->vertices[i]+geom.normals[i]*scale;

        // Set the start- and endpoints to be their respective neighbors.
        m->neighbors[i].setSize(1);
        m->neighbors[i]=k;
        m->neighbors[k].setSize(1);
        m->neighbors[k]=i;
    }

    return m;
}

void Mesh::Factory::populateNeighborhood(Mesh* mesh,float distance,int valence)
{
    for (int i=0;i<mesh->vertices.getSize();++i) {
        // Get the reference vector, i.e. to one to determine the neighborhood of.
        Vec3f const& r=mesh->vertices[i];

        // Resize the neighbor array in advance.
        mesh->neighbors[i].setSize(valence);

        int c=0; // The current valence count.
        int n=0; // The current neighbor index.
        Vec3f u; // Copy of the first neighbor vertex.

        for (int k=0;k<mesh->vertices.getSize(),c<valence;++k) {
            // Get the vector from the reference to the (possible) neighbor.
            Vec3f v=mesh->vertices[k]-r;

            if (i==k || !meta::OpCmpEqual::evaluate(static_cast<float>(v.length()),distance)) {
                // Skip measuring the distance to itself or if it does not equal
                // the required edge length.
                continue;
            }

            if (n==0) {
                // Save the first neighbor found.
                u=v;
            }
            else {
                // Project the neighbors onto the plane "r" is the normal for.
                Vec3f up=u-u.orthoProjection(r);
                Vec3f vp=v-v.orthoProjection(r);

                // Get the oriented angle between the neighbors in the plane.
                double oa=up.orientedAngle(vp,r)*Constd::RAD_TO_DEG();
                n=static_cast<int>(roundToEven(oa/(360.0/valence)));
            }

            // Store the neighbor's vertex index at the calculated position.
            mesh->neighbors[i][n++]=k;
            ++c;
        }
    }
}

} // namespace model

} // namespace gale
