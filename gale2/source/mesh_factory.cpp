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

using namespace gale::math;

namespace gale {

namespace model {

void Mesh::Factory::Shape::Ellipse(VectorArray& shape,int segs,float w,float h)
{
    shape.setSize(segs);

    float delta=2*Constf::PI()/segs;
    for (int i=0;i<segs;++i) {
        // Because of precision issues, multiply here in each iteration instead
        // of accumulating the delta angles.
        float theta=i*delta;

        shape[i]=Vec3f(::cos(theta)*w,::sin(theta)*h,0);
    }
}

void Mesh::Factory::Shape::Supershape(VectorArray& shape,int segs,float m,float n1,float n2,float n3,float a,float b)
{
    math::Superformula f(m,n1,n2,n3,a,b);

    shape.setSize(segs);

    float delta=2*Constf::PI()/segs;
    for (int i=0;i<segs;++i) {
        // Because of precision issues, multiply here in each iteration instead
        // of accumulating the delta angles.
        float theta=i*delta;

        float r=f(theta);
        shape[i]=Vec3f(::cos(theta)*r,::sin(theta)*r,0);
    }
}

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

Mesh* Mesh::Factory::Sphere(float r,int steps)
{
    Mesh* m=Icosahedron();

    // Scale the base mesh's vertices to have the radius' length.
    for (int vi=0;vi<m->vertices.getSize();++vi) {
        m->vertices[vi]=(~m->vertices[vi])*r;
    }

    // Create new vertices in the middle of each edge and push them out to the
    // surface of the sphere.
    Subdivider::Polyhedral(*m,steps,r);

    return m;
}

Mesh* Mesh::Factory::Torus(float r1,float r2,int r1_segs,int r2_segs)
{
    // Calculate points on the path.
    VectorArray path;
    Shape::Ellipse(path,r1_segs,r1,r1);

    // Calculate points on the contour.
    VectorArray contour;
    Shape::Ellipse(contour,r2_segs,r2,r2);

    // Extrude the contour along the path.
    return Extrude(path,contour);
}

Mesh* Mesh::Factory::TorusKnot(float r1,float r2,int r1_segs,int r2_segs,float w,float h,int p,int q)
{
    // Calculate points on the path.
    VectorArray path(r1_segs);
    for (int i=0;i<r1_segs;++i) {
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

    // Calculate points on the contour.
    VectorArray contour;
    Shape::Ellipse(contour,r2_segs,r2,r2);

    // Extrude the contour along the path.
    return Extrude(path,contour);
}

Mesh* Mesh::Factory::MoebiusStrip(float r1w,float r1h,float r2w,float r2h,int r1_segs,int r2_segs)
{
    // Calculate points on the path.
    VectorArray path;
    Shape::Ellipse(path,r1_segs,r1w,r1h);

    // Calculate points on the contour.
    VectorArray contour;
    Shape::Ellipse(contour,r2_segs,r2w,r2h);

    MatrixArray rotation(r1_segs);
    for (int i=0;i<r1_segs;++i) {
        rotation[i]=HMat4f::Factory::RotationZ(Constd::PI()/r1_segs*i);
    }

    // Extrude the contour along the path.
    return Extrude(path,contour,true,&rotation);
}

Mesh* Mesh::Factory::SphericalProduct(Formula& r1,int r1_segs,Formula& r2,int r2_segs)
{
    return FunctionalProduct(r1,r1_segs,r2,r2_segs,Formula(),Constantformula(0));
}

Mesh* Mesh::Factory::ToroidalProduct(Formula& r1,int r1_segs,Formula& r2,int r2_segs)
{
    return FunctionalProduct(r1,r1_segs,r2,r2_segs,Constantformula(1),Formula());
}

Mesh* Mesh::Factory::Extrude(VectorArray const& path,VectorArray const& contour,bool closed,MatrixArray const* trans)
{
    if (path.getSize()<2 || contour.getSize()<3) {
        return NULL;
    }

    Mesh* m=new Mesh(path.getSize()*contour.getSize()+static_cast<int>(!closed)*2);

    // Pointers to the first and last vectors (Alpha and Omega :-) in the path.
    Vec3f const* const pA=&path[0];
    Vec3f const* const pO=&path[path.getSize()-1];

    // Pointer to the currently considered center vector in the path.
    Vec3f const* p=pA;

    // Index of the vertex currently being calculated.
    int vi=0;

    // Calculate the vertex positions.
    for (int pi=0;pi<path.getSize();++pi) {
        // Pointer to p's predecessor along the path.
        Vec3f const* a=(p==pA)?(closed?pO:p):p-1;

        // Pointer to p's successor along the path.
        Vec3f const* b=(p==pO)?(closed?pA:p):p+1;

        // Calculate the Frenet frame in p.
        Vec3f tangent=~(*b-*a);
        Vec3f binormal=~(tangent^(*a+*b));
        Vec3f normal=binormal^tangent;

        HMat4f frenet(binormal,normal,tangent,*p++);

        if (trans) {
            frenet*=(*trans)[pi%trans->getSize()];
        }

        for (int ci=0;ci<contour.getSize();++ci) {
            // Transform the contour along the path.
            m->vertices[vi++]=frenet*contour[ci];
        }
    }

    if (!closed) {
        // Add the first path vertex as the center of the start cut face.
        m->vertices[vi]=*pA;

        // Make all vertices of the start cut face its neighbors.
        IndexArray& nA=m->neighbors[vi];
        nA.setSize(contour.getSize());
        for (int i=0;i<nA.getSize();++i) {
            nA[i]=i;
        }
        ++vi;

        // Add the last path vertex as the center of the end cut face.
        m->vertices[vi]=*pO;

        // Make all vertices of the end cut face its neighbors.
        IndexArray& nO=m->neighbors[vi];
        nO.setSize(contour.getSize());
        for (int i=0;i<nO.getSize();++i) {
            nO[i]=m->vertices.getSize()-3-i;
        }
        ++vi;
    }

    vi=0;

    // Calculate the vertex neighbors.
    for (int pi=0;pi<path.getSize();++pi) {
        // Current "base" vertex on the contour.
        int bi=vi;

        for (int ci=0;ci<contour.getSize();++ci) {
            // Connect the 6 neighbors (5 for the endpoints of an open path).
            IndexArray& vn=m->neighbors[vi];
            vn.setSize(6);

#define WRAP_C(x) wrap(x,contour.getSize())

            int n=0,biwc;

            biwc=bi+WRAP_C(ci-1);
            vn[n++]=biwc;

            // Is this the last vertex on the path?
            if (pi==path.getSize()-1) {
                if (closed) {
                    // It is not easy to find the transformed neighbors of a
                    // closed path, so just connect to the closest ones.
                    Vec3f const& v=m->vertices[vi];

                    int mn=0;
                    float md=(m->vertices[mn]-v).length2();

                    // Calculate the distances to the opposite cut face's vertices.
                    for (int cA=1;cA<contour.getSize();++cA) {
                        float dA=(m->vertices[cA]-v).length2();
                        if (dA<md) {
                            md=dA;
                            mn=cA;
                        }
                    }

                    biwc=mn%contour.getSize();
                    vn[n++]=mn-biwc+WRAP_C(biwc-1);
                    vn[n++]=mn;
                }
                else {
                    // Connect the end cut face vertices to the last path vertex.
                    vn[n++]=m->vertices.getSize()-1;
                    vn.setSize(5);
                }
            }
            else {
                vn[n++]=biwc+contour.getSize();
                vn[n++]=vi+contour.getSize();
            }

            biwc=bi+WRAP_C(ci+1);
            vn[n++]=biwc;

            // Is this the first vertex on the path?
            if (pi==0) {
                if (closed) {
                    // It is not easy to find the transformed neighbors of a
                    // closed path, so just connect to the closest ones.
                    Vec3f const& v=m->vertices[vi];

                    int start=m->vertices.getSize()-1;

                    int mn=start;
                    float md=(m->vertices[mn]-v).length2();

                    // Calculate the distances to the opposite cut face's vertices.
                    for (int cA=1;cA<contour.getSize();++cA) {
                        int cO=start-cA;
                        float dA=(m->vertices[cO]-v).length2();
                        if (dA<md) {
                            md=dA;
                            mn=cO;
                        }
                    }

                    biwc=mn%contour.getSize();
                    vn[n++]=mn-biwc+WRAP_C(biwc+1);
                    vn[n++]=mn;
                }
                else {
                    // Connect the start cut face vertices to the first path vertex.
                    vn[n++]=m->vertices.getSize()-2;
                    vn.setSize(5);
                }
            }
            else {
                vn[n++]=biwc-contour.getSize();
                vn[n++]=vi-contour.getSize();
            }

#undef WRAP_C

            ++vi;
        }
    }

    return m;
}

Mesh* Mesh::Factory::FunctionalProduct(Formula& r1,int r1_segs,Formula& r2,int r2_segs,Formula& fm,Formula& fa)
{
    // Perform some sanity checks.
    if (r1_segs<4 || r2_segs<2) {
        return NULL;
    }

    // Create an empty mesh with the required number of vertices.
    Mesh* m=new Mesh(r1_segs*r2_segs);

    // Index of the vertex currently being calculated.
    int vi=0;

    for (int longitude=0;longitude<r1_segs;++longitude) {
        // -PI <= theta <= PI
        float theta=2*Constf::PI()/r1_segs*longitude-Constf::PI();

        // Current "base" vertex on the contour.
        int bi=vi;

        for (int latitude=0;latitude<r2_segs;++latitude) {
            // -PI/2 <= phi <= PI/2
            float phi=Constf::PI()/r2_segs*latitude-Constf::PI()*0.5f;

            // Calculate the vertex position.
            float r1t=r1(theta),r2p=r2(phi);
            float ct=::cos(theta),cp=::cos(phi);
            float st=::sin(theta),sp=::sin(phi);
            float r2pcp=r2p*cp;

            float x=r1t*ct * fm(r2pcp) + fa(r2pcp*ct);
            float y=r1t*st * fm(r2pcp) + fa(r2pcp*st);

            float z=r2p*sp;

            m->vertices[vi]=Vec3f(x,y,z);

            // Calculate the vertex neighborhood.
            IndexArray& vn=m->neighbors[vi];
            vn.setSize(6);

#define WRAP_LAT(x) wrap(x,r2_segs)

            vn[0]=bi+WRAP_LAT(latitude-1);
            vn[1]=vn[0]+r2_segs;
            vn[2]=vi+r2_segs;
            if (longitude==r1_segs-1) {
                // Wrap around in longitude direction.
                vn[1]-=m->vertices.getSize();
                vn[2]-=m->vertices.getSize();
            }

            vn[3]=bi+WRAP_LAT(latitude+1);
            vn[4]=vn[3]-r2_segs;
            vn[5]=vi-r2_segs;
            if (longitude==0) {
                // Wrap around in longitude direction.
                vn[4]+=m->vertices.getSize();
                vn[5]+=m->vertices.getSize();
            }

#undef WRAP_LAT

            ++vi;
        }
    }

    return m;
}

Mesh* Mesh::Factory::Normals(Preparer const& geom,float scale)
{
    if (!geom.getMesh()) {
        return NULL;
    }

    int n=geom.getMesh()->vertices.getSize();

    // Double the vertices and neighbors for the lines' endpoints.
    Mesh* m=new Mesh(n*2);

    // Copy the vertices to the normal mesh.
    memcpy(m->vertices,geom.getMesh()->vertices,n*sizeof(VectorArray::Type));

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
