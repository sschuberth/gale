#include <stdlib.h>

#include <iostream>

#include <gale/math/vector.h>

using namespace std;
using namespace gale::math;

using gale::meta::OpCmpEqualEps;

int main() {
    Vec4d n(Vec4d::ZERO());
    Vec4d x(Vec4d::X()),y(Vec4d::Y()),z(Vec4d::Z()),w(Vec4d::W());

    cout << "Check predefined constants ..."
         << endl;
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

    cout << "Check conversion constructor ..."
         << endl;
    Vec4i a(12,34,56,78);
    Vec4d b(a);

    cout << "Check access methods ..."
         << endl;
    G_ASSERT(double(a.getX())==b[0])
    G_ASSERT(double(a.getY())==b[1])
    G_ASSERT(double(a.getZ())==b[2])
    G_ASSERT(double(a.getW())==b[3])

    cout << "Check magnitude related methods ..."
         << endl;
    b.normalize();
    G_ASSERT(OpCmpEqualEps::evaluate(b.getLengthSquared(),Vec4d::X()[0]))

    cout << "Check the cross product operator ..."
         << endl;
    G_ASSERT((Vec3i::X()^Vec3i::Y())==Vec3i::Z())

    cout << "Check angle related methods ..."
         << endl;
    double axy1=Vec3i::X().getAngle(Vec3i::Y());
    double axy2=Vec3i::X().getAccurateAngle(Vec3i::Y());
    G_ASSERT(OpCmpEqualEps::evaluate(axy1,axy2))

    cout << "Check getting an orthogonal vector ..."
         << endl;
    G_ASSERT(Vec3f::X().getOrthoVector()==Vec3f::Z())
    G_ASSERT(Vec3f::Y().getOrthoVector()==-Vec3f::Z())
    G_ASSERT(Vec3f::Z().getOrthoVector()==Vec3f::Y())

    cout << "Check unary sign, conversion constructor and collinear methods ..."
         << endl;
    G_ASSERT(Vec3f::X().isCollinear(-Vec3i::X()))

    cout << "Check the dot product operator ..."
         << endl;
    G_ASSERT((Vec3i::X()%Vec3i::Y())==0)
    G_ASSERT((Vec3i::X()%Vec3i::Z())==0)
    G_ASSERT((Vec3i::Y()%Vec3i::Z())==0)

#ifdef _WIN32
    system("pause");
#endif
    return 0;
}
