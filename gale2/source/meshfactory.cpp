#include "gale/model/mesh.h"

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

    // Define neighbors in counter-clockwise ordering on the surface, i.e. in
    // mathematically positive direction of rotation for the implied surface.
    static unsigned int const neighbors[][3]={
        {1,2,3},
        {0,3,2},
        {0,1,3},
        {0,2,1}
    };

    // Create a mesh from the static arrays.
    Mesh* m=new Mesh(vertices);

    for (int i=0;i<G_ARRAY_LENGTH(vertices);++i) {
        m->neighbors[i].insert(neighbors[i]);
    }

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

    // Define neighbors in counter-clockwise ordering on the surface, i.e. in
    // mathematically positive direction of rotation for the implied surface.
    static unsigned int const neighbors[][4]={
        {2,3,4,5},
        {2,5,4,3},
        {3,0,5,1},
        {2,1,4,0},
        {3,1,5,0},
        {2,0,4,1}
    };

    // Create a mesh from the static arrays.
    Mesh* m=new Mesh(vertices);

    for (int i=0;i<G_ARRAY_LENGTH(vertices);++i) {
        m->neighbors[i].insert(neighbors[i]);
    }

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

    // Define neighbors in counter-clockwise ordering on the surface, i.e. in
    // mathematically positive direction of rotation for the implied surface.
    m->neighbors[0].setSize(5);
    m->neighbors[0]=1,2,3,5,4;
    m->neighbors[1].setSize(4);
    m->neighbors[1]=0,4,7,2;
    m->neighbors[2].setSize(5);
    m->neighbors[2]=0,1,7,6,3;
    m->neighbors[3].setSize(4);
    m->neighbors[3]=0,2,6,5;

    m->neighbors[4].setSize(5);
    m->neighbors[4]=0,5,6,7,1;
    m->neighbors[5].setSize(4);
    m->neighbors[5]=0,3,6,4;
    m->neighbors[6].setSize(5);
    m->neighbors[6]=2,7,4,5,3;
    m->neighbors[7].setSize(4);
    m->neighbors[7]=1,4,6,2;

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

    static float const e=::sqrt(2*(a*a-a*b+b*b));

    for (int i=0;i<G_ARRAY_LENGTH(vertices);++i) {
        // Get the reference vector, i.e. to one to determine the neighborhood of.
        Vec3f const& r=m->vertices[i];

        // Each vertex has 5 neighbors.
        m->neighbors[i].setSize(5);

        // "n" is the current neighbor index, "u" points to the first neighbor.
        int n=0;
        Vec3f u;

        for (int k=0;k<G_ARRAY_LENGTH(vertices);++k) {
            // Get the vector from the reference to the (possible) neighbor.
            Vec3f v=m->vertices[k]-r;

            if (i==k || !meta::OpCmpEqual::evaluate(static_cast<float>(v.length()),e)) {
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
                n=roundToEven(oa/72.0);
            }

            // Store the neighbor's vertex index at the calculated position.
            m->neighbors[i][n++]=k;
        }
    }

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

    Mesh* m=new Mesh(vertices);
    return m;
}

} // namespace model

} // namespace gale
