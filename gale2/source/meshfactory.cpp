#include "gale/model/mesh.h"

using namespace gale::math;

namespace gale {

namespace model {

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

    Mesh* m=new Mesh(vertices);

    for (int i=0;i<G_ARRAY_LENGTH(vertices);++i) {
        m->neighbors[i].insert(neighbors[i]);
    }

    return m;
}

} // namespace model

} // namespace gale
