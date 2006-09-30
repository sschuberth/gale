#include <stdlib.h>

#include <iostream>

#include <gale/math/vector.h>

using namespace std;
using namespace gale::math;

using gale::meta::OpCmpEqualEps;

int main() {
    Vec4d n(Vec4d::ZERO()),x(Vec4d::X()),y(Vec4d::Y()),z(Vec4d::Z()),w(Vec4d::W());

    // Check predefined constants.
    G_ASSERT(n[0]==0)
    G_ASSERT(n[1]==0)
    G_ASSERT(n[2]==0)
    G_ASSERT(n[3]==0)

    G_ASSERT(x[0]==1)
    G_ASSERT(x[1]==0)
    G_ASSERT(x[2]==0)
    G_ASSERT(x[3]==0)

    G_ASSERT(y[0]==0)
    G_ASSERT(y[1]==1)
    G_ASSERT(y[2]==0)
    G_ASSERT(y[3]==0)

    G_ASSERT(z[0]==0)
    G_ASSERT(z[1]==0)
    G_ASSERT(z[2]==1)
    G_ASSERT(z[3]==0)

    G_ASSERT(w[0]==0)
    G_ASSERT(w[1]==0)
    G_ASSERT(w[2]==0)
    G_ASSERT(w[3]==1)

    // Check conversion constructor.
    Vec4i a(12,34,56,78);
    Vec4d b(a);

    // Check access methods.
    G_ASSERT(double(a.getX())==b[0])
    G_ASSERT(double(a.getY())==b[1])
    G_ASSERT(double(a.getZ())==b[2])
    G_ASSERT(double(a.getW())==b[3])

    // Check magnitude related methods.
    b.normalize();
    G_ASSERT(OpCmpEqualEps::evaluate(b.getLengthSquared(),1))

    // Check the cross product operator.
    G_ASSERT((Vec3i::X()^Vec3i::Y())==Vec3i::Z())

    // Check angle related methods.
    double axy1=Vec3i::X().getAngle(Vec3i::Y());
    double axy2=Vec3i::X().getAccurateAngle(Vec3i::Y());
    G_ASSERT(OpCmpEqualEps::evaluate(axy1,axy2))

    // Check getting an orthogonal vector.
    G_ASSERT(Vec3f::X().getOrthoVector()==Vec3f::Z())
    G_ASSERT(Vec3f::Y().getOrthoVector()==-Vec3f::Z())
    G_ASSERT(Vec3f::Z().getOrthoVector()==Vec3f::Y())

    // Check unary sign, conversion constructor and collinear methods.
    G_ASSERT(Vec3f::X().isCollinear(-Vec3i::X()))

    // Check the dot product operator.
    G_ASSERT((Vec3i::X()%Vec3i::Y())==0)
    G_ASSERT((Vec3i::X()%Vec3i::Z())==0)
    G_ASSERT((Vec3i::Y()%Vec3i::Z())==0)

    system("pause");
    return 0;
}
