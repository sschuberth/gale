#include "gale/model/mesh.h"

using namespace gale::math;

namespace gale {

namespace model {

Mesh* Mesh::Factory::Tetrahedron()
{
    // http://mathworld.wolfram.com/Tetrahedron.html
    static float const a=0.5f;

    static Vec3f const vertices[]={
        Vec3f(-a,-a,+a),
        Vec3f(+a,-a,-a),
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
    static float const a=1.0f/(2.0f*::sqrt(2.0f));
    static float const b=0.5f;

    static Vec3f const vertices[]={
        // Square base vertices.
        Vec3f(+a, 0,+a),
        Vec3f(+a, 0,-a),
        Vec3f(-a, 0,-a),
        Vec3f(-a, 0,+a),

        // Top and bottom vertices.
        Vec3f( 0,+b, 0),
        Vec3f( 0,-b, 0)
    };

    // Define neighbors in counter-clockwise ordering on the surface, i.e. in
    // mathematically positive direction of rotation for the implied surface.
    static unsigned int const neighbors[][4]={
        {1,4,3,5},
        {0,5,2,4},
        {1,5,3,4},
        {0,4,2,5},
        {0,1,2,3},
        {0,3,2,1}
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

} // namespace model

} // namespace gale
