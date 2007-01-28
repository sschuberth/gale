#include <stdlib.h>

#include <ctime>
#include <iostream>

#include <gale/math/hmatrix4.h>

using namespace std;

using namespace gale::math;
//using namespace gale::system;

using gale::meta::OpCmpEqualEps;

int main()
{
    srand(static_cast<unsigned int>(time(NULL)));

    HMat4f m,n;

    m(0,0)=n(0,0)=static_cast<float>(rand())/RAND_MAX;
    m(1,0)=n(0,1)=static_cast<float>(rand())/RAND_MAX;
    m(2,0)=n(0,2)=static_cast<float>(rand())/RAND_MAX;

    m(0,1)=n(1,0)=static_cast<float>(rand())/RAND_MAX;
    m(1,1)=n(1,1)=static_cast<float>(rand())/RAND_MAX;
    m(2,1)=n(1,2)=static_cast<float>(rand())/RAND_MAX;

    m(0,2)=n(2,0)=static_cast<float>(rand())/RAND_MAX;
    m(1,2)=n(2,1)=static_cast<float>(rand())/RAND_MAX;
    m(2,2)=n(2,2)=static_cast<float>(rand())/RAND_MAX;

    m(0,3)=n(0,3)=0;
    m(1,3)=n(1,3)=0;
    m(2,3)=n(2,3)=0;

    n.invert();

    G_ASSERT(m==n)

    m(0,3)=static_cast<float>(rand())/RAND_MAX;
    m(1,3)=static_cast<float>(rand())/RAND_MAX;
    m(2,3)=static_cast<float>(rand())/RAND_MAX;

    m.orthonormalize();
    n=!m;

    HMat4f a(m.normal,m.up,m.look,m.position);
    m*=n;

    cout << m << endl;

    G_ASSERT(m==HMat4f::IDENTITY())

    G_ASSERT(n*HMat4f::IDENTITY()==n)

    HMat4f b=a+a;
    G_ASSERT(a==b/2)
    G_ASSERT(b-a==a)

#ifdef _WIN32
    system("pause");
#endif
    return 0;
}
