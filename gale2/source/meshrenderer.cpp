#include "gale/model/mesh.h"

#include "gale/global/platform.h"

using namespace gale::math;

namespace gale {

namespace model {

void Mesh::Renderer::compile()
{
    // Clear all indices as they will be rebuilt now.
    triangles.clear();
    quads.clear();
    polygons.clear();

    for (int vi=0;vi<mesh.vertices.getSize();++vi) {
        IndexArray const& vn=mesh.neighbors[vi];
        Vec3f const& v=mesh.vertices[vi];

        for (int n=0;n<vn.getSize();++n) {
            // Get the orbit vertices starting with the edge from vi to its
            // currently selected neighbor.
            IndexArray polygon;
            int o=mesh.orbit(vi,vn[n],polygon);

            // Make sure to walk each face only once, i.e. rule out permutations
            // of face indices. Use the address in memory to define a relation
            // on the universe of vertices.
            Vec3f const& a=mesh.vertices[polygon[1]];
            Vec3f const& b=mesh.vertices[polygon[2]];
            if (o<3 || &v<&a || &v<&b) {
                continue;
            }

            if (o==3) {
                triangles.insert(polygon);
            }
            else {
                // More than 3 vertices require another check.
                Vec3f const& c=mesh.vertices[polygon[3]];
                if (&v<&c) {
                    continue;
                }

                if (o==4) {
                    quads.insert(polygon);
                }
                else {
                    // More than 4 vertices require another check.
                    Vec3f const& d=mesh.vertices[polygon[4]];
                    if (o>5 || &v<&d) {
                        continue;
                    }

                    // Note: For more than 5 vertices more checks are needed.
                    polygons.insert(polygon);
                }
            }
        }
    }
}

void Mesh::Renderer::render()
{
    glEnableClientState(GL_VERTEX_ARRAY);

    glVertexPointer(3,GL_FLOAT,0,mesh.vertices);

    glDrawElements(GL_TRIANGLES,triangles.getSize(),GL_UNSIGNED_INT,triangles);
    glDrawElements(GL_QUADS,quads.getSize(),GL_UNSIGNED_INT,quads);

    for (int i=0;i<polygons.getSize();++i) {
        glDrawElements(GL_POLYGON,polygons[i].getSize(),GL_UNSIGNED_INT,polygons[i]);
    }
}

} // namespace model

} // namespace gale
