// Enable memory leak detection, see:
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vsdebug/html/vxcondetectingisolatingmemoryleaks.asp
#if !defined NDEBUG && !defined GALE_TINY_CODE
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
#endif

#include <gale/math/hmatrix4.h>
#include <gale/math/matrix4.h>
#include <gale/math/quaternion.h>

#ifndef GALE_TINY_CODE
    #include <iostream>
#endif

using namespace gale::math;

int __cdecl main()
{
    RandomEcuyerd r;

    // Homogeneous matrix tests.

    HMat4d m=HMat4d::IDENTITY(),n=HMat4d::IDENTITY();

    Vec3d axis=Vec3d::random(r);
    double angle=r.random0ExclN(Constd::PI()*2);

    Vec3d dir=Vec3d::random(r);
    double factor=r.randomExcl0N(10.0);

    double sx=r.randomExcl0N(10.0);
    double sy=r.randomExcl0N(10.0);
    double sz=r.randomExcl0N(10.0);

    m*=HMat4d::Factory::Rotation(axis,angle);
    m*=HMat4d::Factory::Translation(dir,factor);
    m*=HMat4d::Factory::Scaling(sx,sy,sz);

    n=m;
    n.invert();
    assert(m*n==HMat4d::IDENTITY());

    m(0,3)=r.random01();
    m(1,3)=r.random01();
    m(2,3)=r.random01();

    m.orthonormalize();
    n=!m;

    HMat4d a(m.c0,m.c1,m.c2,m.c3);
    m*=n;

#ifndef GALE_TINY_CODE
    std::cout << m << std::endl;
#endif

    assert(m==HMat4d::IDENTITY());
    assert(n*HMat4d::IDENTITY()==n);

    HMat4d b=a+a;
    assert(a==b/2);
    assert(b-a==a);

    Mat4f projection=Mat4f::Factory::Projection(Vec3f::Y(),Vec3f(-2,2,0));
    assert(projection*Vec3f(-1,1,0)==Vec3f::ZERO());
    assert(projection*Vec3f(0,1,0)==Vec3f(2,0,0));
    assert(projection*Vec3f(-2,1,0)==Vec3f(-2,0,0));

    axis=Vec3d::random(r);
    angle=r.random0ExclN(Constd::PI()*2);
    Vec3d v=Vec3d::random(r);

    m=HMat4d::Factory::Rotation(axis,angle);
    Vec3d vrm=m*v;

    Quatd q(axis,angle);
    Vec3d vrq=q*v;

    assert(vrm.equals(vrq));

    // Matrix tests.

    Mat4d A=Mat4d::random(r),B=A,C;

    bool result;
    B.invert(&result);
    assert(result);

    C=A*B;
    assert(C==Mat4d::IDENTITY());

#if !defined NDEBUG && !defined GALE_TINY_CODE
    _CrtDumpMemoryLeaks();
#endif

    return 0;
}
