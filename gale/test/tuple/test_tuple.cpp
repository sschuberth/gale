#include <stdlib.h>

#include <ctime>
#include <iostream>

#include <gale/math/tuple.h>

using namespace std;

using gale::math::Tuple;
using gale::meta::OpCmpEqualEps;

int main() {
    srand((unsigned)time(NULL));

    // Check construction of objects.
    Tuple<2,int> t2i_a(1,2),t2i_b(rand(),rand());
    Tuple<3,float> t3f_a(3.0f,4.0f,5.0f),t3f_b((float)rand(),(float)rand(),(float)rand());
    Tuple<4,double> t4d_a(5.0,6.0,7.0,8.0),t4d_b((double)rand(),(double)rand(),(double)rand(),(double)rand());

    cout << t2i_a << ", " << t2i_b << endl;
    cout << t3f_a << ", " << t3f_b << endl;
    cout << t4d_a << ", " << t4d_b << endl;

    // Check the sizes in memory.
    G_ASSERT(sizeof(t2i_a)==2*sizeof(int))
    G_ASSERT(sizeof(t3f_a)==3*sizeof(float))
    G_ASSERT(sizeof(t4d_a)==4*sizeof(double))

    // Check implicit pointer conversion.
    cout << t2i_a[0] << endl;
    cout << t2i_a[1] << endl;

    cout << t2i_b[0] << endl;
    cout << t2i_b[1] << endl;

    cout << "-- " << endl;

    // Check addition.
    {
        Tuple<2,int> tmp=t2i_a+t2i_b;
        G_ASSERT(tmp[0]==t2i_a[0]+t2i_b[0])
        G_ASSERT(tmp[1]==t2i_a[1]+t2i_b[1])
    }

    // Check subtraction.
    {
        Tuple<2,int> tmp=t2i_a-t2i_b;
        G_ASSERT(tmp[0]==t2i_a[0]-t2i_b[0])
        G_ASSERT(tmp[1]==t2i_a[1]-t2i_b[1])
    }

    // Check division (and implicitly multiplication).
    {
        Tuple<2,int> tmp=t2i_a/t2i_b;
        G_ASSERT(tmp[0]==t2i_a[0]/t2i_b[0])
        G_ASSERT(tmp[1]==t2i_a[1]/t2i_b[1])
    }

    // Check division (and implicitly multiplication) by a scalar.
    {
        float s=12.3f;
        Tuple<3,float> tmp=t3f_a/s;
        G_ASSERT(OpCmpEqualEps::evaluate(tmp[0],t3f_a[0]/s))
        G_ASSERT(OpCmpEqualEps::evaluate(tmp[1],t3f_a[1]/s))
        G_ASSERT(OpCmpEqualEps::evaluate(tmp[2],t3f_a[2]/s))
    }

    // Check minimum element determination.
    {
        float tmp=t3f_b.getMinElement();
        G_ASSERT(tmp<=t3f_b[0])
        G_ASSERT(tmp<=t3f_b[1])
        G_ASSERT(tmp<=t3f_b[2])
    }

    // Check maximum element determination.
    {
        double tmp=t4d_b.getMaxElement();
        G_ASSERT(tmp>=t4d_b[0])
        G_ASSERT(tmp>=t4d_b[1])
        G_ASSERT(tmp>=t4d_b[2])
        G_ASSERT(tmp>=t4d_b[3])
    }

    // Check unary sign operators and absolute extremes element determination.
    {
        Tuple<3,float> tmp1=+t3f_b;
        Tuple<3,float> tmp2=-t3f_a;

        float tmp_min=tmp2.getAbsMinElement();
        G_ASSERT(tmp_min<=std::abs(tmp2[0]))
        G_ASSERT(tmp_min<=std::abs(tmp2[1]))
        G_ASSERT(tmp_min<=std::abs(tmp2[2]))

        float tmp_max=tmp2.getAbsMaxElement();
        G_ASSERT(tmp_max>=std::abs(tmp2[0]))
        G_ASSERT(tmp_max>=std::abs(tmp2[1]))
        G_ASSERT(tmp_max>=std::abs(tmp2[2]))
    }

    // Check element-wise minimum determination.
    {
        Tuple<3,float> tmp=t3f_a.getMinElements(t3f_b);
        G_ASSERT(tmp[0]<=t3f_a[0] && tmp[0]<=t3f_b[0])
        G_ASSERT(tmp[1]<=t3f_a[1] && tmp[1]<=t3f_b[1])
        G_ASSERT(tmp[2]<=t3f_a[2] && tmp[2]<=t3f_b[2])
    }

    // Check element-wise maximum determination.
    {
        Tuple<3,float> tmp=t3f_a.getMaxElements(t3f_b);
        G_ASSERT(tmp[0]>=t3f_a[0] && tmp[0]>=t3f_b[0])
        G_ASSERT(tmp[1]>=t3f_a[1] && tmp[1]>=t3f_b[1])
        G_ASSERT(tmp[2]>=t3f_a[2] && tmp[2]>=t3f_b[2])
    }

    // Check linear interpolation.
    {
        const double s=0.63;
        Tuple<4,double> tmp=t4d_a.lerp(t4d_b,s);
        G_ASSERT(tmp[0]==t4d_a[0]+(t4d_b[0]-t4d_a[0])*s)
        G_ASSERT(tmp[1]==t4d_a[1]+(t4d_b[1]-t4d_a[1])*s)
        G_ASSERT(tmp[2]==t4d_a[2]+(t4d_b[2]-t4d_a[2])*s)
        G_ASSERT(tmp[3]==t4d_a[3]+(t4d_b[3]-t4d_a[3])*s)
    }

    // Check remaining vector / vector operators.
    {
        Tuple<3,float> tmp((float)rand(),(float)rand(),(float)rand());
        Tuple<3,float> res=((t3f_a+t3f_b)-t3f_b)*tmp;
        G_ASSERT(OpCmpEqualEps::evaluate(res[0],(t3f_a[0]+t3f_b[0]-t3f_b[0])*tmp[0]))
        G_ASSERT(OpCmpEqualEps::evaluate(res[1],(t3f_a[1]+t3f_b[1]-t3f_b[1])*tmp[1]))
        G_ASSERT(OpCmpEqualEps::evaluate(res[2],(t3f_a[2]+t3f_b[2]-t3f_b[2])*tmp[2]))

        Tuple<3,float> res2=tmp/res;
        G_ASSERT(OpCmpEqualEps::evaluate(res2[0],tmp[0]/res[0]))
        G_ASSERT(OpCmpEqualEps::evaluate(res2[1],tmp[1]/res[1]))
        G_ASSERT(OpCmpEqualEps::evaluate(res2[2],tmp[2]/res[2]))
    }

    // Check remaining vector / scalar operators.
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

    // Check comparison operators.
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

    system("pause");
    return 0;
}