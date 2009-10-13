#include <gale/math/random.h>
#include <gale/math/hmatrix4.h>

#ifndef GALE_TINY_CODE
    #include <iostream>
#endif

using namespace gale::math;

int main()
{
    RandomEcuyerf r;

    // Homogeneous matrix tests.

    HMat4f m,n;

    m(0,0)=n(0,0)=r.random01();
    m(1,0)=n(0,1)=r.random01();
    m(2,0)=n(0,2)=r.random01();

    m(0,1)=n(1,0)=r.random01();
    m(1,1)=n(1,1)=r.random01();
    m(2,1)=n(1,2)=r.random01();

    m(0,2)=n(2,0)=r.random01();
    m(1,2)=n(2,1)=r.random01();
    m(2,2)=n(2,2)=r.random01();

    m(0,3)=n(0,3)=0;
    m(1,3)=n(1,3)=0;
    m(2,3)=n(2,3)=0;

    n.invert();

    assert(m==n);

    m(0,3)=r.random01();
    m(1,3)=r.random01();
    m(2,3)=r.random01();

    m.orthonormalize();
    n=!m;

    HMat4f a(m.c0,m.c1,m.c2,m.c3);
    m*=n;

#ifndef GALE_TINY_CODE
    std::cout << m << std::endl;
#endif

    assert(m==HMat4f::IDENTITY());
    assert(n*HMat4f::IDENTITY()==n);

    HMat4f b=a+a;
    assert(a==b/2);
    assert(b-a==a);

    HMat4f projection=HMat4f::Factory::Projection(Vec3f::X());
    assert(projection*Vec3f::X()==Vec3f::ZERO());
    assert(projection*Vec3f::Y()==Vec3f::Y());
    assert(projection*Vec3f::Z()==Vec3f::Z());
    assert(projection*Vec3f(1,1,1)==Vec3f(0,1,1));

    Vec3f axis=r.randomVec3();
    double angle=r.random0ExclN(Constf::PI()*2);
    Vec3f v=r.randomVec3();

    m=HMat4f::Factory::Rotation(axis,angle);
    Vec3f vrm=m*v;

    Quatf q(axis,angle);
    Vec3f vrq=q*v;

    assert(vrm.equals(vrq));

    // Matrix tests.

    Mat4f A=r.randomMat4(),B=A,C;

    bool result;
    B.invert(&result);
    assert(result);

    C=A*B;
    assert(C==Mat4f::IDENTITY());

    return 0;
}
