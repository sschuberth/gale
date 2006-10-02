#include <stdlib.h>

#include <cmath>
#include <ctime>
#include <iostream>

#include <gale/math/tuple.h>
#include <gale/system/cpuinfo.h>

using namespace std;

using gale::math::Tuple;
using gale::meta::OpCmpEqualEps;
using gale::system::CPU;

int main() {
    cout << "CPU vendor: "
         << CPU.getVendorString()
         << endl;

    cout << "Instruction sets:"
         << (CPU.hasMMX()?" MMX":"")
         << (CPU.hasMMXExt()?" MMXExt":"")
         << (CPU.has3DNow()?" 3DNow!":"")
         << (CPU.has3DNowExt()?" 3DNow!Ext":"")
         << (CPU.hasSSE()?" SSE":"")
         << (CPU.hasSSE2()?" SSE2":"")
         << (CPU.hasSSE3()?" SSE3":"")
         << (CPU.hasSSSE3()?" SSSE3":"")
         << endl;

    srand(static_cast<unsigned int>(time(NULL)));

    cout << "Check construction of objects ..."
         << endl;
    Tuple<2,int> t2i_a(1,2),t2i_b(rand(),rand());

    Tuple<3,float> t3f_a(3.0f,4.0f,5.0f);
    Tuple<3,float> t3f_b(
        static_cast<float>(rand()),
        static_cast<float>(rand()),
        static_cast<float>(rand())
    );

    Tuple<4,double> t4d_a(5.0,6.0,7.0,8.0);
    Tuple<4,double> t4d_b(
        static_cast<double>(rand()),
        static_cast<double>(rand()),
        static_cast<double>(rand()),
        static_cast<double>(rand())
    );

    cout << t2i_a << ", " << t2i_b << endl;
    cout << t3f_a << ", " << t3f_b << endl;
    cout << t4d_a << ", " << t4d_b << endl;

    cout << "Check the sizes in memory ..."
         << endl;
    G_ASSERT(sizeof(t2i_a)==2*sizeof(int))
    G_ASSERT(sizeof(t3f_a)==3*sizeof(float))
    G_ASSERT(sizeof(t4d_a)==4*sizeof(double))

    cout << "Check implicit pointer conversion ..."
         << endl;
    cout << t2i_a[0] << endl;
    cout << t2i_a[1] << endl;
    cout << t2i_b[0] << endl;
    cout << t2i_b[1] << endl;

    cout << "-- " << endl;

    cout << "Check addition ..."
         << endl;
    {
        Tuple<2,int> tmp=t2i_a+t2i_b;
        G_ASSERT(tmp[0]==t2i_a[0]+t2i_b[0])
        G_ASSERT(tmp[1]==t2i_a[1]+t2i_b[1])
    }

    cout << "Check subtraction ..."
         << endl;
    {
        Tuple<2,int> tmp=t2i_a-t2i_b;
        G_ASSERT(tmp[0]==t2i_a[0]-t2i_b[0])
        G_ASSERT(tmp[1]==t2i_a[1]-t2i_b[1])
    }

    cout << "Check division (and implicitly multiplication) ..."
         << endl;
    {
        Tuple<2,int> tmp=t2i_a/t2i_b;
        G_ASSERT(tmp[0]==t2i_a[0]/t2i_b[0])
        G_ASSERT(tmp[1]==t2i_a[1]/t2i_b[1])
    }

    cout << "Check division (and implicitly multiplication) by a scalar ..."
         << endl;
    {
        float s=12.3f;
        Tuple<3,float> tmp=t3f_a/s;
        G_ASSERT(OpCmpEqualEps::evaluate(tmp[0],t3f_a[0]/s))
        G_ASSERT(OpCmpEqualEps::evaluate(tmp[1],t3f_a[1]/s))
        G_ASSERT(OpCmpEqualEps::evaluate(tmp[2],t3f_a[2]/s))
    }

    cout << "Check minimum element determination ..."
         << endl;
    {
        float tmp=t3f_b.getMinElement();
        G_ASSERT(tmp<=t3f_b[0])
        G_ASSERT(tmp<=t3f_b[1])
        G_ASSERT(tmp<=t3f_b[2])
    }

    cout << "Check maximum element determination ..."
         << endl;
    {
        double tmp=t4d_b.getMaxElement();
        G_ASSERT(tmp>=t4d_b[0])
        G_ASSERT(tmp>=t4d_b[1])
        G_ASSERT(tmp>=t4d_b[2])
        G_ASSERT(tmp>=t4d_b[3])
    }

    cout << "Check unary sign operators and to get absolute min / max elements ..."
         << endl;
    {
        Tuple<3,float> tmp1=+t3f_b;
        Tuple<3,float> tmp2=-t3f_a;

        float tmp_min=tmp2.getAbsMinElement();
        G_ASSERT(tmp_min<=fabs(tmp2[0]))
        G_ASSERT(tmp_min<=fabs(tmp2[1]))
        G_ASSERT(tmp_min<=fabs(tmp2[2]))

        float tmp_max=tmp2.getAbsMaxElement();
        G_ASSERT(tmp_max>=fabs(tmp2[0]))
        G_ASSERT(tmp_max>=fabs(tmp2[1]))
        G_ASSERT(tmp_max>=fabs(tmp2[2]))
    }

    cout << "Check element-wise minimum determination ..."
         << endl;
    {
        Tuple<3,float> tmp=t3f_a.getMinElements(t3f_b);
        G_ASSERT(tmp[0]<=t3f_a[0] && tmp[0]<=t3f_b[0])
        G_ASSERT(tmp[1]<=t3f_a[1] && tmp[1]<=t3f_b[1])
        G_ASSERT(tmp[2]<=t3f_a[2] && tmp[2]<=t3f_b[2])
    }

    cout << "Check element-wise maximum determination ..."
         << endl;
    {
        Tuple<3,float> tmp=t3f_a.getMaxElements(t3f_b);
        G_ASSERT(tmp[0]>=t3f_a[0] && tmp[0]>=t3f_b[0])
        G_ASSERT(tmp[1]>=t3f_a[1] && tmp[1]>=t3f_b[1])
        G_ASSERT(tmp[2]>=t3f_a[2] && tmp[2]>=t3f_b[2])
    }

    cout << "Check linear interpolation ..."
         << endl;
    {
        const double s=0.63;
        Tuple<4,double> tmp=t4d_a.lerp(t4d_b,s);
        G_ASSERT(OpCmpEqualEps::evaluate(tmp[0],t4d_a[0]+(t4d_b[0]-t4d_a[0])*s))
        G_ASSERT(OpCmpEqualEps::evaluate(tmp[1],t4d_a[1]+(t4d_b[1]-t4d_a[1])*s))
        G_ASSERT(OpCmpEqualEps::evaluate(tmp[2],t4d_a[2]+(t4d_b[2]-t4d_a[2])*s))
        G_ASSERT(OpCmpEqualEps::evaluate(tmp[3],t4d_a[3]+(t4d_b[3]-t4d_a[3])*s))
    }

    cout << "Check remaining vector vs. vector operators ..."
         << endl;
    {
        Tuple<3,float> tmp(
            static_cast<float>(rand()),
            static_cast<float>(rand()),
            static_cast<float>(rand())
        );

        Tuple<3,float> res=((t3f_a+t3f_b)-t3f_b)*tmp;
        G_ASSERT(OpCmpEqualEps::evaluate(res[0],(t3f_a[0]+t3f_b[0]-t3f_b[0])*tmp[0]))
        G_ASSERT(OpCmpEqualEps::evaluate(res[1],(t3f_a[1]+t3f_b[1]-t3f_b[1])*tmp[1]))
        G_ASSERT(OpCmpEqualEps::evaluate(res[2],(t3f_a[2]+t3f_b[2]-t3f_b[2])*tmp[2]))

        Tuple<3,float> res2=tmp/res;
        G_ASSERT(OpCmpEqualEps::evaluate(res2[0],tmp[0]/res[0]))
        G_ASSERT(OpCmpEqualEps::evaluate(res2[1],tmp[1]/res[1]))
        G_ASSERT(OpCmpEqualEps::evaluate(res2[2],tmp[2]/res[2]))
    }

    cout << "Check remaining vector vs. scalar operators ..."
         << endl;
    {
        float s=344.5f,t=24.6f;

        Tuple<3,float> res=t3f_a*s;
        G_ASSERT(OpCmpEqualEps::evaluate(res[0],t3f_a[0]*s))
        G_ASSERT(OpCmpEqualEps::evaluate(res[1],t3f_a[1]*s))
        G_ASSERT(OpCmpEqualEps::evaluate(res[2],t3f_a[2]*s))

        Tuple<3,float> res2=res/t;
        G_ASSERT(OpCmpEqualEps::evaluate(res2[0],res[0]/t,numeric_limits<float>::epsilon()*40))
        G_ASSERT(OpCmpEqualEps::evaluate(res2[1],res[1]/t,numeric_limits<float>::epsilon()*40))
        G_ASSERT(OpCmpEqualEps::evaluate(res2[2],res[2]/t,numeric_limits<float>::epsilon()*40))

    }

    cout << "Check comparison operators ..."
         << endl;
    {
        bool res_l=(t3f_a<t3f_b);
        G_ASSERT(res_l==(t3f_a[0]<t3f_b[0] && t3f_a[1]<t3f_b[1] && t3f_a[2]<t3f_b[2]))

        bool res_le=(t3f_a<=t3f_b);
        G_ASSERT(res_le==(t3f_a[0]<=t3f_b[0] && t3f_a[1]<=t3f_b[1] && t3f_a[2]<=t3f_b[2]))

        bool res_g=(t3f_a>t3f_b);
        G_ASSERT(res_g==(t3f_a[0]>t3f_b[0] && t3f_a[1]>t3f_b[1] && t3f_a[2]>t3f_b[2]))

        bool res_ge=(t3f_a>=t3f_b);
        G_ASSERT(res_ge==(t3f_a[0]>=t3f_b[0] && t3f_a[1]>=t3f_b[1] && t3f_a[2]>=t3f_b[2]))

        bool res_eq=(t2i_a==t2i_b);
        G_ASSERT(res_eq==(t2i_a[0]==t2i_b[0] && t2i_a[1]==t2i_b[1]))

        bool res_ne=(t2i_a!=t2i_b);
        G_ASSERT(res_ne==(t2i_a[0]!=t2i_b[0] || t2i_a[1]!=t2i_b[1]))
    }

#ifdef _WIN32
    system("pause");
#endif
    return 0;
}
