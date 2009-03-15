#include <gale/math/color.h>
#include <gale/math/random.h>
#include <gale/math/tuple.h>
#include <gale/math/vector.h>
#include <gale/system/cpuinfo.h>
#include <gale/system/timer.h>

#include <stdlib.h>

#include <ctime>
#include <iostream>

using namespace gale::math;
using namespace gale::system;

using gale::meta::OpCmpEqual;

using namespace std;

RandomEcuyerf r;

void test_tuple();
void test_vector();
void test_color();

int main()
{
    cout << "Found "
         << CPU.coresPerProcessor()
         << " core(s) per processor, each supporting "
         << CPU.threadsPerCore()
         << " hardware thread(s)."
         << endl;

    cout << "CPU vendor: "
         << CPU.vendorString()
         << endl;

    cout << "Instruction sets:"
         << (CPU.hasHTT()?" HTT":"")
         << (CPU.hasMMX()?" MMX":"")
         << (CPU.hasMMXExt()?" MMXExt":"")
         << (CPU.has3DNow()?" 3DNow!":"")
         << (CPU.has3DNowExt()?" 3DNow!Ext":"")
         << (CPU.hasSSE()?" SSE":"")
         << (CPU.hasSSE2()?" SSE2":"")
         << (CPU.hasSSE3()?" SSE3":"")
         << (CPU.hasSSSE3()?" SSSE3":"")
         << ((CPU.isIntel() && CPU.hasEM64T())?" EM64T":"")
         << ((CPU.isAMD() && CPU.hasAMD64())?" AMD64":"")
         << endl;

    test_tuple();
    test_vector();
    test_color();

#ifdef _WIN32
    system("pause");
#endif
    return 0;
}

void test_tuple()
{
#ifdef GALE_USE_SSE
    cout << "Check SIMD implementation ..."
         << endl;
    float t4f_a0=2.0f,t4f_a1=3.0f,t4f_a2=5.0f,t4f_a3=7.0f;
    float t4f_b0=9.0f,t4f_b1=8.0f,t4f_b2=7.0f,t4f_b3=6.0f;

    Tuple<4,float> t4f_a(t4f_a0,t4f_a1,t4f_a2,t4f_a3);
    Tuple<4,float> t4f_b(t4f_b0,t4f_b1,t4f_b2,t4f_b3);

    Tuple<4,float> t4f_c=t4f_a.minElements(t4f_b);
    assert(t4f_c[0]==gale::math::min(t4f_a0,t4f_b0));
    assert(t4f_c[1]==gale::math::min(t4f_a1,t4f_b1));
    assert(t4f_c[2]==gale::math::min(t4f_a2,t4f_b2));
    assert(t4f_c[3]==gale::math::min(t4f_a3,t4f_b3));

    assert(t4f_c<=t4f_a && t4f_c<=t4f_b);

    t4f_c=t4f_a.getLerp(t4f_b,0.5);
    assert(t4f_c[0]==(t4f_a0+t4f_b0)/2);
    assert(t4f_c[1]==(t4f_a1+t4f_b1)/2);
    assert(t4f_c[2]==(t4f_a2+t4f_b2)/2);
    assert(t4f_c[3]==(t4f_a3+t4f_b3)/2);

    t4f_c=-t4f_a+t4f_b;
    assert(t4f_c[0]==-t4f_a0+t4f_b0);
    assert(t4f_c[1]==-t4f_a1+t4f_b1);
    assert(t4f_c[2]==-t4f_a2+t4f_b2);
    assert(t4f_c[3]==-t4f_a3+t4f_b3);

    t4f_a=t4f_b*t4f_c;
    assert(t4f_a[0]==t4f_b[0]*t4f_c[0]);
    assert(t4f_a[1]==t4f_b[1]*t4f_c[1]);
    assert(t4f_a[2]==t4f_b[2]*t4f_c[2]);
    assert(t4f_a[3]==t4f_b[3]*t4f_c[3]);

    t4f_c=t4f_a*t4f_b[3];
    assert(t4f_c[0]==t4f_a[0]*t4f_b[3]);
    assert(t4f_c[1]==t4f_a[1]*t4f_b[3]);
    assert(t4f_c[2]==t4f_a[2]*t4f_b[3]);
    assert(t4f_c[3]==t4f_a[3]*t4f_b[3]);

    t4f_c=t4f_a/t4f_b[3];
    assert(OpCmpEqual::evaluate(t4f_c[0],t4f_a[0]/t4f_b[3]));
    assert(OpCmpEqual::evaluate(t4f_c[1],t4f_a[1]/t4f_b[3]));
    assert(OpCmpEqual::evaluate(t4f_c[2],t4f_a[2]/t4f_b[3]));
    assert(OpCmpEqual::evaluate(t4f_c[3],t4f_a[3]/t4f_b[3]));

    t4f_a=2.0f/t4f_c;
    assert(OpCmpEqual::evaluate(t4f_a[0],2.0f/t4f_c[0]));
    assert(OpCmpEqual::evaluate(t4f_a[1],2.0f/t4f_c[1]));
    assert(OpCmpEqual::evaluate(t4f_a[2],2.0f/t4f_c[2]));
    assert(OpCmpEqual::evaluate(t4f_a[3],2.0f/t4f_c[3]));
#endif

    cout << "Check construction of objects ..."
         << endl;
    Tuple<2,int> t2i_a(1,2),t2i_b(r.random(),r.random());

    Tuple<3,float> t3f_a(3.0f,4.0f,5.0f);
    Tuple<3,float> t3f_b(
        static_cast<float>(r.random0N(1000.0f)),
        static_cast<float>(r.random0N(1000.0f)),
        static_cast<float>(r.random0N(1000.0f))
    );

    Tuple<4,double> t4d_a(5.0,6.0,7.0,8.0);
    Tuple<4,double> t4d_b(
        static_cast<double>(r.random0N(1000.0)),
        static_cast<double>(r.random0N(1000.0)),
        static_cast<double>(r.random0N(1000.0)),
        static_cast<double>(r.random0N(1000.0))
    );

    cout << t2i_a << ", " << t2i_b << endl;
    cout << t3f_a << ", " << t3f_b << endl;
    cout << t4d_a << ", " << t4d_b << endl;

    cout << "Check the sizes in memory ..."
         << endl;
    assert(sizeof(t2i_a)==2*sizeof(int));
    assert(sizeof(t3f_a)==3*sizeof(float));
    assert(sizeof(t4d_a)==4*sizeof(double));

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
        assert(tmp[0]==t2i_a[0]+t2i_b[0]);
        assert(tmp[1]==t2i_a[1]+t2i_b[1]);
    }

    cout << "Check subtraction ..."
         << endl;
    {
        Tuple<2,int> tmp=t2i_a-t2i_b;
        assert(tmp[0]==t2i_a[0]-t2i_b[0]);
        assert(tmp[1]==t2i_a[1]-t2i_b[1]);
    }

    cout << "Check division (and implicitly multiplication) ..."
         << endl;
    {
        Tuple<2,int> tmp=t2i_a/t2i_b;
        assert(tmp[0]==t2i_a[0]/t2i_b[0]);
        assert(tmp[1]==t2i_a[1]/t2i_b[1]);
    }

    cout << "Check division (and implicitly multiplication) by a scalar ..."
         << endl;
    {
        float s=12.3f;
        Tuple<3,float> tmp=t3f_a/s;
        assert(OpCmpEqual::evaluate(tmp[0],t3f_a[0]/s));
        assert(OpCmpEqual::evaluate(tmp[1],t3f_a[1]/s));
        assert(OpCmpEqual::evaluate(tmp[2],t3f_a[2]/s));
    }

    cout << "Check minimum element determination ..."
         << endl;
    {
        float tmp=t3f_b.minElement();
        assert(tmp<=t3f_b[0]);
        assert(tmp<=t3f_b[1]);
        assert(tmp<=t3f_b[2]);
    }

    cout << "Check maximum element determination ..."
         << endl;
    {
        double tmp=t4d_b.maxElement();
        assert(tmp>=t4d_b[0]);
        assert(tmp>=t4d_b[1]);
        assert(tmp>=t4d_b[2]);
        assert(tmp>=t4d_b[3]);
    }

    cout << "Check unary sign operators and to get absolute min / max elements ..."
         << endl;
    {
        Tuple<3,float> tmp1=+t3f_b;
        Tuple<3,float> tmp2=-t3f_a;

        float tmp_min=tmp2.absMinElement();
        assert(tmp_min<=fabs(tmp2[0]));
        assert(tmp_min<=fabs(tmp2[1]));
        assert(tmp_min<=fabs(tmp2[2]));

        float tmp_max=tmp2.absMaxElement();
        assert(tmp_max>=fabs(tmp2[0]));
        assert(tmp_max>=fabs(tmp2[1]));
        assert(tmp_max>=fabs(tmp2[2]));
    }

    cout << "Check element-wise minimum determination ..."
         << endl;
    {
        Tuple<3,float> tmp=t3f_a.minElements(t3f_b);
        assert(tmp[0]<=t3f_a[0] && tmp[0]<=t3f_b[0]);
        assert(tmp[1]<=t3f_a[1] && tmp[1]<=t3f_b[1]);
        assert(tmp[2]<=t3f_a[2] && tmp[2]<=t3f_b[2]);
    }

    cout << "Check element-wise maximum determination ..."
         << endl;
    {
        Tuple<3,float> tmp=t3f_a.maxElements(t3f_b);
        assert(tmp[0]>=t3f_a[0] && tmp[0]>=t3f_b[0]);
        assert(tmp[1]>=t3f_a[1] && tmp[1]>=t3f_b[1]);
        assert(tmp[2]>=t3f_a[2] && tmp[2]>=t3f_b[2]);
    }

    cout << "Check linear interpolation ..."
         << endl;
    {
        double const s=0.63;
        Tuple<4,double> tmp=lerp(t4d_a,t4d_b,s);
        assert(OpCmpEqual::evaluate(tmp[0],t4d_a[0]+(t4d_b[0]-t4d_a[0])*s));
        assert(OpCmpEqual::evaluate(tmp[1],t4d_a[1]+(t4d_b[1]-t4d_a[1])*s));
        assert(OpCmpEqual::evaluate(tmp[2],t4d_a[2]+(t4d_b[2]-t4d_a[2])*s));
        assert(OpCmpEqual::evaluate(tmp[3],t4d_a[3]+(t4d_b[3]-t4d_a[3])*s));
    }

    cout << "Check remaining vector vs. vector operators ..."
         << endl;
    {
        Tuple<3,float> tmp(
            static_cast<float>(r.random0N(1000.0f)),
            static_cast<float>(r.random0N(1000.0f)),
            static_cast<float>(r.random0N(1000.0f))
        );

        Tuple<3,float> res=((t3f_a+t3f_b)-t3f_b)*tmp;
        assert(OpCmpEqual::evaluate(res[0],(t3f_a[0]+t3f_b[0]-t3f_b[0])*tmp[0]));
        assert(OpCmpEqual::evaluate(res[1],(t3f_a[1]+t3f_b[1]-t3f_b[1])*tmp[1]));
        assert(OpCmpEqual::evaluate(res[2],(t3f_a[2]+t3f_b[2]-t3f_b[2])*tmp[2]));

        Tuple<3,float> res2=tmp/res;
        assert(OpCmpEqual::evaluate(res2[0],tmp[0]/res[0]));
        assert(OpCmpEqual::evaluate(res2[1],tmp[1]/res[1]));
        assert(OpCmpEqual::evaluate(res2[2],tmp[2]/res[2]));
    }

    cout << "Check remaining vector vs. scalar operators ..."
         << endl;
    {
        float s=344.5f,t=24.6f;

        Tuple<3,float> res=t3f_a*s;
        assert(OpCmpEqual::evaluate(res[0],t3f_a[0]*s));
        assert(OpCmpEqual::evaluate(res[1],t3f_a[1]*s));
        assert(OpCmpEqual::evaluate(res[2],t3f_a[2]*s));

        Tuple<3,float> res2=res/t;
        assert(OpCmpEqual::evaluate(res2[0],res[0]/t));
        assert(OpCmpEqual::evaluate(res2[1],res[1]/t));
        assert(OpCmpEqual::evaluate(res2[2],res[2]/t));

    }

    cout << "Check comparison operators ..."
         << endl;
    {
        bool res_l=(t3f_a<t3f_b);
        assert(res_l==(t3f_a[0]<t3f_b[0] && t3f_a[1]<t3f_b[1] && t3f_a[2]<t3f_b[2]));

        bool res_le=(t3f_a<=t3f_b);
        assert(res_le==(t3f_a[0]<=t3f_b[0] && t3f_a[1]<=t3f_b[1] && t3f_a[2]<=t3f_b[2]));

        bool res_g=(t3f_a>t3f_b);
        assert(res_g==(t3f_a[0]>t3f_b[0] && t3f_a[1]>t3f_b[1] && t3f_a[2]>t3f_b[2]));

        bool res_ge=(t3f_a>=t3f_b);
        assert(res_ge==(t3f_a[0]>=t3f_b[0] && t3f_a[1]>=t3f_b[1] && t3f_a[2]>=t3f_b[2]));

        bool res_eq=(t2i_a==t2i_b);
        assert(res_eq==(t2i_a[0]==t2i_b[0] && t2i_a[1]==t2i_b[1]));

        bool res_ne=(t2i_a!=t2i_b);
        assert(res_ne==(t2i_a[0]!=t2i_b[0] || t2i_a[1]!=t2i_b[1]));
    }
}

void test_vector()
{
    double s;
    Timer t;
    unsigned int ms;

    Vec4d n(Vec4d::ZERO());
    Vec4d x(Vec4d::X()),y(Vec4d::Y()),z(Vec4d::Z()),w(Vec4d::W());

    cout << "Check predefined constants ..."
         << endl;
    assert(n[0]==0);
    assert(n[1]==0);
    assert(n[2]==0);
    assert(n[3]==0);

    assert(x[0]==1);
    assert(x[1]==0);
    assert(x[2]==0);
    assert(x[3]==0);

    assert(y[0]==0);
    assert(y[1]==1);
    assert(y[2]==0);
    assert(y[3]==0);

    assert(z[0]==0);
    assert(z[1]==0);
    assert(z[2]==1);
    assert(z[3]==0);

    assert(w[0]==0);
    assert(w[1]==0);
    assert(w[2]==0);
    assert(w[3]==1);

    cout << "Check conversion constructor ..."
         << endl;
    Vec4i a(12,34,56,78);
    Vec4d b(a);

    cout << "Check access methods ..."
         << endl;
    assert(double(a.getX())==b[0]);
    assert(double(a.getY())==b[1]);
    assert(double(a.getZ())==b[2]);
    assert(double(a.getW())==b[3]);

    cout << "Check magnitude related methods ..."
         << endl;
    b.normalize();
    assert(OpCmpEqual::evaluate(b.length2(),Vec4d::X()[0]));

    cout << "Check the cross product operator ..."
         << endl;
    assert((Vec3i::X()^Vec3i::Y()).equals(Vec3i::Z()));

    cout << "Check angle related methods ..."
         << endl;
    double axy1=Vec3i::X().angle(Vec3i::Y());
    double axy2=Vec3i::X().orientedAngle(Vec3i::Y(),Vec3i::Z());
    assert(OpCmpEqual::evaluate(axy1,axy2));

    cout << "Check getting an orthogonal vector ..."
         << endl;
    assert(Vec3f::X().orthoVector().equals(Vec3f::Z()));
    assert(Vec3f::Y().orthoVector().equals(-Vec3f::Z()));
    assert(Vec3f::Z().orthoVector().equals(Vec3f::Y()));

    cout << "Check unary sign, conversion constructor and collinear methods ..."
         << endl;
    assert(Vec3f::X().isCollinear(-Vec3i::X()));

    cout << "Check the dot product operator ..."
         << endl;
    assert((Vec3i::X()%Vec3i::Y())==0);
    assert((Vec3i::X()%Vec3i::Z())==0);
    assert((Vec3i::Y()%Vec3i::Z())==0);

    t.stop(s);
    cout << "Time elapsed: " << s << " seconds." << endl;

    cout << "Sleeping for " << (ms=2500) << " milliseconds (timed)." << endl;
    t.start();
    Timer::sleep(ms);
    t.stop(s);
    cout << "Time elapsed: " << s << " seconds." << endl;

    cout << "Sleeping for " << (ms=1000) << " milliseconds." << endl;
    Timer::sleep(ms);

    cout << "Sleeping for " << (ms=1500) << " milliseconds (timed)." << endl;
    t.start();
    Timer::sleep(ms);
    t.stop(s);
    cout << "Time elapsed: " << s << " seconds." << endl;
}

void test_color()
{
    Col3d black=Col3d::BLACK();
    static Col3d const col3d(Col3d::MIN_INTENSITY(),Col3d::MIN_INTENSITY(),Col3d::MIN_INTENSITY());
    assert(col3d==black);

    Col3f blue=Col3f::BLUE();
    static Col3f const col3f(Col3f::MIN_INTENSITY(),Col3f::MIN_INTENSITY(),Col3f::MAX_INTENSITY());
    assert(col3f==blue);

    Col3i green=Col3i::GREEN();
    static Col3i const col3i(Col3i::MIN_INTENSITY(),Col3i::MAX_INTENSITY(),Col3i::MIN_INTENSITY());
    assert(col3i==green);

    Col3ui cyan=Col3ui::CYAN();
    static Col3ui const col3ui(Col3ui::MIN_INTENSITY(),Col3ui::MAX_INTENSITY(),Col3ui::MAX_INTENSITY());
    assert(col3ui==cyan);

    Col3s red=Col3s::RED();
    static Col3s const col3s(Col3s::MAX_INTENSITY(),Col3s::MIN_INTENSITY(),Col3s::MIN_INTENSITY());
    assert(col3s==red);

    Col3us magenta=Col3us::MAGENTA();
    static Col3us const col3us(Col3us::MAX_INTENSITY(),Col3us::MIN_INTENSITY(),Col3us::MAX_INTENSITY());
    assert(col3us==magenta);

    Col3b yellow=Col3b::YELLOW();
    static Col3b const col3b(Col3b::MAX_INTENSITY(),Col3b::MAX_INTENSITY(),Col3b::MIN_INTENSITY());
    assert(col3b==yellow);

    Col3ub white=Col3ub::WHITE();
    static Col3ub const col3ub(Col3ub::MAX_INTENSITY(),Col3ub::MAX_INTENSITY(),Col3ub::MAX_INTENSITY());
    assert(col3ub==white);

    assert(white.inverse()==Col3ub::BLACK());

    Col4f white4=Col4f::WHITE();
    white4.setA(0.5);
    Col4f black4=Col4f::BLACK();
    black4.setA(0.5);
    assert(!white4==black4);
}
