#include <stdlib.h>

#include <ctime>
#include <iostream>

#include <gale/math/random.h>
#include <gale/math/hmatrix4.h>

using namespace std;

using namespace gale::math;

int main()
{
    RandomEcuyerf r;

    HMat4f m,n;

    m(0,0)=n(0,0)=r.getRandom01();
    m(1,0)=n(0,1)=r.getRandom01();
    m(2,0)=n(0,2)=r.getRandom01();

    m(0,1)=n(1,0)=r.getRandom01();
    m(1,1)=n(1,1)=r.getRandom01();
    m(2,1)=n(1,2)=r.getRandom01();

    m(0,2)=n(2,0)=r.getRandom01();
    m(1,2)=n(2,1)=r.getRandom01();
    m(2,2)=n(2,2)=r.getRandom01();

    m(0,3)=n(0,3)=0;
    m(1,3)=n(1,3)=0;
    m(2,3)=n(2,3)=0;

    n.invert();

    assert(m==n);

    m(0,3)=r.getRandom01();
    m(1,3)=r.getRandom01();
    m(2,3)=r.getRandom01();

    m.orthonormalize();
    n=!m;

    HMat4f a(m.c0,m.c1,m.c2,m.c3);
    m*=n;

    cout << m << endl;

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

#ifdef _WIN32
    system("pause");
#endif
    return 0;
}
