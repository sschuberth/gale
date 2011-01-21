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

void Mesh::Factory::Shape::Ellipse(VectorArray& shape,int const segs,float const w,float const h)
{
    shape.setSize(segs);

    float delta=2*Constf::PI()/segs;
    for (int i=0;i<segs;++i) {
        // Because of precision issues, multiply here in each iteration instead
        // of accumulating the delta angles.
        float theta=i*delta;

        shape[i]=Vec3f(cos(theta)*w,sin(theta)*h,0);
    }
}

void Mesh::Factory::Shape::Heart(VectorArray& shape,int const segs)
{
    shape.setSize(segs);

    float delta=2*Constf::PI()/segs;
    for (int i=0;i<segs;++i) {
        // Because of precision issues, multiply here in each iteration instead
        // of accumulating the delta angles.
        float omega=-Constf::PI()+i*delta;

        float lambda=abs(omega)/Constf::PI();
        float lambda2=lambda*lambda,lambda3=lambda2*lambda;

        float r=(13*lambda-22*lambda2+10*lambda3)/(6-5*lambda);

        // Rotated by 90° CCW from the original formula.
        shape[i]=Vec3f(sin(omega)*r,-cos(omega)*r,0);
    }
}

void Mesh::Factory::Shape::Supershape(VectorArray& shape,int const segs,float const m,float const n1,float const n2,float const n3,float const a,float const b)
{
    math::SuperFormula f(m,n1,n2,n3,a,b);

    shape.setSize(segs);

    float delta=2*Constf::PI()/segs;
    for (int i=0;i<segs;++i) {
        // Because of precision issues, multiply here in each iteration instead
        // of accumulating the delta angles.
        float theta=i*delta;

        float r=f(theta);
        shape[i]=Vec3f(cos(theta)*r,sin(theta)*r,0);
    }
}

Mesh* Mesh::Factory::Tetrahedron()
{
    // http://mathworld.wolfram.com/Tetrahedron.html
    static float const a=0.5f;

    static Vec3f const vertices[]={
        // Vertices on the lower edge, which is orthogonal to the upper edge.
        Vec3f(-a,-a,+a)
    ,   Vec3f(+a,-a,-a)

        // Vertices on the upper edge, which is orthogonal to the lower edge.
    ,   Vec3f(+a,+a,+a)
    ,   Vec3f(-a,+a,-a)
    };

    // Create a mesh from the static arrays.
    Mesh* m=new Mesh(vertices);

    // Make all vertices neighbors whose distance matches the required edge length.
    static float const e=sqrt(8*a*a);
    populateNeighborhood(m,e,3);

    return m;
}

Mesh* Mesh::Factory::Octahedron()
{
    // http://mathworld.wolfram.com/Octahedron.html
    static float const a=0.5f;
    static float const b=1.0f/(2.0f*sqrt(2.0f));

    static Vec3f const vertices[]={
        // Top and bottom vertices.
        Vec3f( 0,+a, 0)
    ,   Vec3f( 0,-a, 0)

        // Square base vertices.
    ,   Vec3f(+b, 0,+b)
    ,   Vec3f(+b, 0,-b)
    ,   Vec3f(-b, 0,-b)
    ,   Vec3f(-b, 0,+b)
    };

    // Create a mesh from the static arrays.
    Mesh* m=new Mesh(vertices);

    // Make all vertices neighbors whose distance matches the required edge length.
    static float const e=sqrt(a*a+2*b*b);
    populateNeighborhood(m,e,4);

    return m;
}

Mesh* Mesh::Factory::Hexahedron()
{
    // http://mathworld.wolfram.com/Hexahedron.html
    static float const a=0.5f;

    static Vec3f const vertices[]={
        // Front face vertices.
        Vec3f(+a,+a,+a)
    ,   Vec3f(-a,+a,+a)
    ,   Vec3f(-a,-a,+a)
    ,   Vec3f(+a,-a,+a)

        // Back face vertices.
    ,   Vec3f(-a,+a,-a)
    ,   Vec3f(+a,+a,-a)
    ,   Vec3f(+a,-a,-a)
    ,   Vec3f(-a,-a,-a)
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
        Vec3f( 0,-b,+a)
    ,   Vec3f(-b,-a, 0)
    ,   Vec3f(+b,-a, 0)
    ,   Vec3f( 0,-b,-a)
    ,   Vec3f( 0,+b,-a)
    ,   Vec3f(+a, 0,-b)
    ,   Vec3f(-a, 0,-b)
    ,   Vec3f( 0,+b,+a)
    ,   Vec3f(-a, 0,+b)
    ,   Vec3f(+a, 0,+b)
    ,   Vec3f(-b,+a, 0)
    ,   Vec3f(+b,+a, 0)
    };

    // Create a mesh from the static vertex array. Instead of hard-coding the
    // neighborhood, do a brute-force search for the correct edge length.
    Mesh* m=new Mesh(vertices);

    // Make all vertices neighbors whose distance matches the required edge length.
    static float const e=sqrt(2*(a*a-a*b+b*b));
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
        Vec3f(-c, 0,-a)
    ,   Vec3f(+c, 0,-a)
    ,   Vec3f(+b,-b,-b)
    ,   Vec3f( 0,-a,-c)
    ,   Vec3f(-b,-b,-b)
    ,   Vec3f(-b,+b,-b)
    ,   Vec3f( 0,+a,-c)
    ,   Vec3f(+b,+b,-b)
    ,   Vec3f(-c, 0,+a)
    ,   Vec3f(+c, 0,+a)
    ,   Vec3f(+b,+b,+b)
    ,   Vec3f( 0,+a,+c)
    ,   Vec3f(-b,+b,+b)
    ,   Vec3f(-b,-b,+b)
    ,   Vec3f( 0,-a,+c)
    ,   Vec3f(+b,-b,+b)
    ,   Vec3f(-a,-c, 0)
    ,   Vec3f(+a,-c, 0)
    ,   Vec3f(+a,+c, 0)
    ,   Vec3f(-a,+c, 0)
    };

    // Create a mesh from the static vertex array. Instead of hard-coding the
    // neighborhood, do a brute-force search for the correct edge length.
    Mesh* m=new Mesh(vertices);

    // Make all vertices neighbors whose distance matches the required edge length.
    static float const e=2*c;
    populateNeighborhood(m,e,3);

    return m;
}

Mesh* Mesh::Factory::Sphere(float const r,int const steps)
{
    Mesh* m=Icosahedron();

    // Scale the base mesh's vertices to have the radius' length.
    for (int vi=0;vi<m->vertices.getSize();++vi) {
        Vec3f& v=m->vertices[vi];
        v=(~v)*r;
    }

    // Create new vertices in the middle of each edge and push them out to the
    // surface of the sphere.
    Subdivider::Polyhedral(*m,steps,r);

    return m;
}

Mesh* Mesh::Factory::Torus(float const r1,float const r2,int const r1_segs,int const r2_segs)
{
    // Calculate points on the path.
    VectorArray path;
    Shape::Ellipse(path,r1_segs,r1,r1);

    // Calculate points on the contour.
    VectorArray contour;
    Shape::Ellipse(contour,r2_segs,r2,r2);

    // Extrude the contour along the path.
    return Extruder(path,contour);
}

Mesh* Mesh::Factory::TorusKnot(float const rk,float const rt,int const sp,int const sc,int const p,int const q,float const w,float const h)
{
    // Calculate points on the path.
    VectorArray path(sp);
    for (int i=0;i<sp;++i) {
        float angle=2*Constf::PI()/sp*i;

        // Calculate the cylindrical coordinates.
        float theta=angle*q;
        float r=rk+w*cos(theta);
        float z=h*sin(theta);

        // Convert to cartesian coordinates.
        float phi=angle*p;
        float x=r*cos(phi);
        float y=r*sin(phi);

        path[i]=Vec3f(x,y,z);
    }

    // Calculate points on the contour.
    VectorArray contour;
    Shape::Ellipse(contour,sc,rt,rt);

    // Extrude the contour along the path.
    return Extruder(path,contour);
}

Mesh* Mesh::Factory::MoebiusStrip(float const r1w,float const r1h,float const r2w,float const r2h,int const r1_segs,int const r2_segs)
{
    // Calculate points on the path.
    VectorArray path;
    Shape::Ellipse(path,r1_segs,r1w,r1h);

    // Calculate points on the contour.
    VectorArray contour;
    Shape::Ellipse(contour,r2_segs,r2w,r2h);

    MatrixArray rotation(r1_segs);
    for (int i=0;i<r1_segs;++i) {
        rotation[i]=HMat4f::Factory::RotationZ(Constf::PI()/r1_segs*i);
    }

    // Extrude the contour along the path.
    return Extruder(path,contour,true,&rotation);
}

Mesh* Mesh::Factory::Apple(int const s_sections,int const t_sections)
{
    struct Formula:FormulaR2R3
    {
        Vec3f operator()(Vec2f const& w) const {
            float const& u=w.getX();
            float const& v=w.getY();

            float const cv=cos(v);
            float const sv=sin(v);

            float const f=(4.0f+3.8f*cv);

            return Vec3f(
                cos(u)*f
            ,   sin(u)*f
            ,   (cv+sv-1.0f)*(1.0f+sv)*log(1.0f-Constf::PI()*v/10.0f)+7.5f*sv
            );
        }
    };

    Formula f;

    return GridMapper(f,0,2*Constf::PI(),s_sections,true,-Constf::PI(),Constf::PI(),t_sections,true);
}

Mesh* Mesh::Factory::Shell(int const s_sections,int const t_sections,float const r1,float const r2,float const h,int const n)
{
// Warning C4512: Assignment operator could not be generated.
#pragma warning(disable:4512)

    struct Formula:public FormulaR2R3
    {
        Formula(float const r1,float const r2,float const h,int const n)
        :   a(r1),b(h),c(r2)
        ,   n(n)
        {}

        Vec3f operator()(Vec2f const& v) const {
            float const& s=v.getX();
            float const& t=v.getY();

            float const top=t/(2*Constf::PI());
            float const at=a*(1-top);

            float const cnt=cos(n*t);
            float const snt=sin(n*t);

            float const cs=cos(s);
            float const ss=sin(s);

            return Vec3f(
                at*cnt*(1+ss)+c*cnt
            ,   at*snt*(1+ss)+c*snt
            ,   b*top+at*cs
            );
        }

        float const a,b,c;
        int const n;
    };

#pragma warning(default:4512)

    Formula f(r1,r2,h,n);

    return GridMapper(f,0,2*Constf::PI(),s_sections,true,0,2*Constf::PI(),t_sections,false);
}

// Warning C4512: Assignment operator could not be generated.
#pragma warning(disable:4512)

/**
 * Helper formula to combine the spherical and toroidal product into a single formula.
 */
struct ProductFormula:public FormulaR2R3
{
    /// Initializes with the product formulas \a r1 and \a r2. \a fm and \a fa
    /// define the multiplicative and additive terms for the product calculation.
    ProductFormula(Formula const& r1,Formula const& r2,Formula const& fm,Formula const& fa)
    :   r1(r1),r2(r2)
    ,   fm(fm),fa(fa)
    {}

    /// Evaluates the functional product for the given angles theta and phi
    /// encoded as the first and second components of \a v.
    Vec3f operator()(Vec2f const& v) const {
        float r1t=r1(v.getX()),r2p=r2(v.getY());
        float ct=cos(v.getX()),cp=cos(v.getY());
        float st=sin(v.getX()),sp=sin(v.getY());
        float r2pcp=r2p*cp;

        float x=r1t*ct * fm(r2pcp) + fa(r2pcp*ct);
        float y=r1t*st * fm(r2pcp) + fa(r2pcp*st);

        float z=r2p*sp;

        return Vec3f(x,y,z);
    }

    Formula const& r1; ///< First product formula.
    Formula const& r2; ///< Second product formula.

    Formula const& fm; ///< Multiplicative term formula.
    Formula const& fa; ///< Additive term formula.
};

#pragma warning(default:4512)

Mesh* Mesh::Factory::SphericalProduct(Formula const& long_form,int const long_segs,Formula const& lat_form,int const lat_segs)
{
    Formula fm;
    ConstantFormula fa(0);

    ProductFormula eval(long_form,lat_form,fm,fa);

    // Longitude: -PI <= theta <= PI, Latitude: -PI/2 <= phi <= PI/2.
    return GridMapper(eval,-Constf::PI(),Constf::PI(),long_segs,true,-Constf::PI()*0.5f,Constf::PI()*0.5f,lat_segs,true);
}

Mesh* Mesh::Factory::ToroidalProduct(Formula const& long_form,int const long_segs,Formula const& lat_form,int const lat_segs)
{
    ConstantFormula fm(1);
    Formula fa;

    ProductFormula eval(long_form,lat_form,fm,fa);

    // Longitude: -PI <= theta <= PI, Latitude: -PI/2 <= phi <= PI/2.
    return GridMapper(eval,-Constf::PI(),Constf::PI(),long_segs,true,-Constf::PI()*0.5f,Constf::PI()*0.5f,lat_segs,true);
}

// Warning C4701: Potentially uninitialized local variable 'mn' used.
#pragma warning(disable:4701)

Mesh* Mesh::Factory::Extruder(VectorArray const& path,VectorArray const& contour,bool const closed,MatrixArray const* const trans)
{
    if (path.getSize()<2 || contour.getSize()<3) {
        return NULL;
    }

    Mesh* m=new Mesh(path.getSize()*contour.getSize()+static_cast<int>(!closed)*2);
    VectorArray& mv=m->vertices;

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
        Vec3f const* const a=(p==pA)?(closed?pO:p):p-1;

        // Pointer to p's successor along the path.
        Vec3f const* const b=(p==pO)?(closed?pA:p):p+1;

        // Calculate the Frenet frame in p.
        Vec3f tangent=~(*b-*a);
        Vec3f binormal=*a+*b;
        if (binormal.isCollinear(tangent)) {
            binormal=Vec3f::X();
        }
        else {
            binormal=~(tangent^binormal);
        }
        Vec3f normal=binormal^tangent;

        HMat4f frenet(binormal,normal,tangent,*p++);

        if (trans) {
            frenet*=(*trans)[pi%trans->getSize()];
        }

        for (int ci=0;ci<contour.getSize();++ci) {
            // Transform the contour along the path.
            mv[vi++]=frenet*contour[ci];
        }
    }

    if (!closed) {
        // Add the first path vertex as the center of the start cut face.
        mv[vi]=*pA;

        // Make all vertices of the start cut face its neighbors.
        IndexArray& nA=m->neighbors[vi];
        nA.setSize(contour.getSize());
        for (int i=0;i<nA.getSize();++i) {
            nA[i]=i;
        }
        ++vi;

        // Add the last path vertex as the center of the end cut face.
        mv[vi]=*pO;

        // Make all vertices of the end cut face its neighbors.
        IndexArray& nO=m->neighbors[vi];
        nO.setSize(contour.getSize());
        for (int i=0;i<nO.getSize();++i) {
            nO[i]=mv.getSize()-3-i;
        }
        ++vi;
    }

    // Reset the index of the vertex currently being dealt with.
    vi=0;

    // Calculate the vertex neighbors.
    for (int pi=0;pi<path.getSize();++pi) {
        // Current "base" vertex on the contour.
        int bi=vi;

        // Index of an end cut face vertex with minimal distance to the first
        // start cut face vertex.
        int mn;

        for (int ci=0;ci<contour.getSize();++ci) {
            // Connect the 6 neighbors (5 for the endpoints of an open path).
            IndexArray& vn=m->neighbors[vi];
            vn.setSize(6);

#define WRAP_C(x) wrap(x,contour.getSize())

            int n=0,biwc;

            // Add the predecessor on the contour as a neighbor.
            biwc=bi+WRAP_C(ci-1);
            vn[n++]=biwc;

            // Is this the last vertex on the path?
            if (pi==path.getSize()-1) {
                if (closed) {
                    // As the start cut face's neighbors were already determined,
                    // just turn partial neighbors into mutual neighbors here.
                    for (int cA=0;cA<contour.getSize();++cA) {
                        if (m->neighbors[cA].find(vi)>=0) {
                            // If there is a wrap-around in the neighbor indices,
                            // swap them.
                            unsigned int& pn=vn[n-1];
                            if (pn==0 && cA==contour.getSize()-1) {
                                vn[n]=pn;
                                pn=cA;
                            }
                            else {
                                vn[n]=cA;
                            }
                            ++n;
                        }
                    }
                }
                else {
                    // Connect the end cut face vertices to the last path vertex.
                    vn[n++]=mv.getSize()-1;
                    vn.setSize(5);
                }
            }
            else {
                vn[n++]=biwc+contour.getSize();
                vn[n++]=vi+contour.getSize();
            }

            // Add the successor on the contour as a neighbor.
            biwc=bi+WRAP_C(ci+1);
            vn[n++]=biwc;

            // Is this the first vertex on the path?
            if (pi==0) {
                if (closed) {
                    // It is unclear how to determine the transformed neighbors
                    // of a closed path, so just connect to the closest ones.
                    Vec3f const& v=mv[vi];

                    // Determine the index of the closest end cut face vertex
                    // only for the first start cut face vertex, and simply
                    // increment the index from then on.
                    if (vi==0) {
                        int start=mv.getSize()-1;

                        mn=start;
                        float md=(mv[mn]-v).length2();

                        // Calculate the distances to the opposite cut face's vertices.
                        for (int cA=1;cA<contour.getSize();++cA) {
                            int cO=start-cA;
                            float dA=(mv[cO]-v).length2();
                            if (dA<md) {
                                md=dA;
                                mn=cO;
                            }
                        }
                    }
                    else {
                        ++mn;
                        if (mn==mv.getSize()) {
                            mn-=contour.getSize();
                        }
                    }

                    biwc=mn%contour.getSize();
                    vn[n++]=mn-biwc+WRAP_C(biwc+1);
                    vn[n++]=mn;
                }
                else {
                    // Connect the start cut face vertices to the first path vertex.
                    vn[n++]=mv.getSize()-2;
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

#pragma warning(default:4701)

Mesh* Mesh::Factory::GridMapper(
    FormulaR2R3 const& eval
,   float const s_min
,   float const s_max
,   int const s_sections
,   bool const s_closed
,   float const t_min
,   float const t_max
,   int const t_sections
,   bool const t_closed
)
{
    // Perform some sanity checks.
    if (s_sections<3 || t_sections<3) {
        return NULL;
    }

    // If the grid is not closed, start and end vertices cannot be shared, and
    // we need one more set of coordinates.
    int s_coords=s_sections+static_cast<int>(!s_closed);
    int t_coords=t_sections+static_cast<int>(!t_closed);

    // Create an empty mesh with the required number of vertices.
    Mesh* m=new Mesh(s_coords*t_coords);

    // Current grid coordinate.
    Vec2f st;

    // Index of the vertex currently being calculated.
    int vi=0;

    float s_delta=(s_max-s_min)/s_sections;
    float t_delta=(t_max-t_min)/t_sections;

    for (int s=0;s<s_coords;++s) {
        st.setX(s_min+s*s_delta);

        // Current "base" vertex on the grid.
        int bi=vi;

        for (int t=0;t<t_coords;++t) {
            st.setY(t_min+t*t_delta);

            // Calculate the vertex position.
            m->vertices[vi]=eval(st);

            // Calculate the vertex neighborhood.
            IndexArray& vn=m->neighbors[vi];
            vn.setSize(6);

// Wrap around in y-direction.
#define WRAP_T(t) wrap(t,t_coords)

// Wrap around in x-direction.
#define WRAP_V(s) wrap(s,m->vertices.getSize())

            int n=0;

            bool t_border=(t==0);

            if (!t_border || t_closed) {
                vn[n++]=bi+WRAP_T(t-1);
            }

            if (s!=s_coords-1 || s_closed) {
                if (!t_border || t_closed) {
                    vn[n++]=WRAP_V(static_cast<int>(vn[n-1])+t_coords);
                }
                vn[n++]=WRAP_V(vi+t_coords);
            }

            t_border=(t==t_coords-1);

            if (!t_border || t_closed) {
                vn[n++]=bi+WRAP_T(t+1);
            }

            if (s!=0 || s_closed) {
                if (!t_border || t_closed) {
                    vn[n++]=WRAP_V(static_cast<int>(vn[n-1])-t_coords);
                }
                vn[n++]=WRAP_V(vi-t_coords);
            }

            // Adjust to the real number of neighbors.
            vn.setSize(n);

#undef WRAP_V

#undef WRAP_T

            ++vi;
        }
    }

    return m;
}

Mesh* Mesh::Factory::Normals(int n,Vec3f const* vertices,Vec3f const* normals,float const scale)
{
    if (n<=0 || !vertices || !normals) {
        return NULL;
    }

    // Double the vertices and neighbors for the lines' end-points.
    Mesh* m=new Mesh(n*2);

    // Copy the start-points to the normal mesh.
    memcpy(m->vertices,vertices,n*sizeof(VectorArray::Type));

    // Calculate the end-points for the normal mesh.
    for (int i=0,k=n;i<n;++i,++k) {
        // Calculate the end-point by pointing from the start-point into the normal direction.
        m->vertices[k]=(*vertices++)+(*normals++)*scale;

        // Set the start- and end-points to be their respective neighbors.
        m->neighbors[i].setSize(1);
        m->neighbors[i]=k;
        m->neighbors[k].setSize(1);
        m->neighbors[k]=i;
    }

    return m;
}

void Mesh::Factory::populateNeighborhood(Mesh* const mesh,float distance,int const valence)
{
    // Square the distance so we can compare it to the (cheaper) squared length.
    distance*=distance;

    for (int i=0;i<mesh->vertices.getSize();++i) {
        // Get the reference vector, i.e. to one to determine the neighborhood of.
        Vec3f const& r=mesh->vertices[i];

        // Resize the neighbor array in advance.
        mesh->neighbors[i].setSize(valence);

        int c=0; // The current valence count.
        int n=0; // The current neighbor index.
        Vec3f u; // Copy of the first neighbor vertex.

        for (int k=0;k<mesh->vertices.getSize() && c<valence;++k) {
            // Get the vector from the reference to the (possible) neighbor.
            Vec3f v=mesh->vertices[k]-r;

            if (i==k || !meta::OpCmpEqual::evaluate(static_cast<float>(v.length2()),distance)) {
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
                n=roundToEven(oa/(360.0/valence));
            }

            // Store the neighbor's vertex index at the calculated position.
            mesh->neighbors[i][n++]=k;
            ++c;
        }
    }
}

} // namespace model

} // namespace gale
