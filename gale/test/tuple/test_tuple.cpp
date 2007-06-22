#include <gale/math/random.h>
#include <gale/math/tuple.h>
#include <gale/math/vector.h>
#include <gale/math/color.h>

#include <gale/system/cpuinfo.h>
#include <gale/system/timer.h>

#include <stdlib.h>

#include <ctime>
#include <iostream>

using namespace gale::math;
using namespace gale::system;

using gale::meta::OpCmpEqualEps;

using namespace std;

RandomEcuyerf r;

void test_tuple();
void test_vector();
void test_color();

int main()
{
    cout << "Found "
         << CPU.getCoresPerProcessor()
         << " core(s) per processor, each supporting "
         << CPU.getThreadsPerCore()
         << " hardware thread(s)."
         << endl;

    cout << "CPU vendor: "
         << CPU.getVendorString()
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
#ifdef GALE_SSE
    cout << "Check SIMD implementation ..."
         << endl;
    float t4f_a0=2.0f,t4f_a1=3.0f,t4f_a2=5.0f,t4f_a3=7.0f;
    float t4f_b0=9.0f,t4f_b1=8.0f,t4f_b2=7.0f,t4f_b3=6.0f;

    Tuple<4,float> t4f_a(t4f_a0,t4f_a1,t4f_a2,t4f_a3);
    Tuple<4,float> t4f_b(t4f_b0,t4f_b1,t4f_b2,t4f_b3);

    Tuple<4,float> t4f_c=t4f_a.getMinElements(t4f_b);
    G_ASSERT(t4f_c[0]==gale::math::min(t4f_a0,t4f_b0))
    G_ASSERT(t4f_c[1]==gale::math::min(t4f_a1,t4f_b1))
    G_ASSERT(t4f_c[2]==gale::math::min(t4f_a2,t4f_b2))
    G_ASSERT(t4f_c[3]==gale::math::min(t4f_a3,t4f_b3))

    G_ASSERT(t4f_c<=t4f_a && t4f_c<=t4f_b)

    t4f_c=t4f_a.lerpTo(t4f_b,0.5);
    G_ASSERT(t4f_c[0]==(t4f_a0+t4f_b0)/2)
    G_ASSERT(t4f_c[1]==(t4f_a1+t4f_b1)/2)
    G_ASSERT(t4f_c[2]==(t4f_a2+t4f_b2)/2)
    G_ASSERT(t4f_c[3]==(t4f_a3+t4f_b3)/2)

    t4f_c=-t4f_a+t4f_b;
    G_ASSERT(t4f_c[0]==-t4f_a0+t4f_b0)
    G_ASSERT(t4f_c[1]==-t4f_a1+t4f_b1)
    G_ASSERT(t4f_c[2]==-t4f_a2+t4f_b2)
    G_ASSERT(t4f_c[3]==-t4f_a3+t4f_b3)

    t4f_a=t4f_b*t4f_c;
    G_ASSERT(t4f_a[0]==t4f_b[0]*t4f_c[0])
    G_ASSERT(t4f_a[1]==t4f_b[1]*t4f_c[1])
    G_ASSERT(t4f_a[2]==t4f_b[2]*t4f_c[2])
    G_ASSERT(t4f_a[3]==t4f_b[3]*t4f_c[3])

    t4f_c=t4f_a*t4f_b[3];
    G_ASSERT(t4f_c[0]==t4f_a[0]*t4f_b[3])
    G_ASSERT(t4f_c[1]==t4f_a[1]*t4f_b[3])
    G_ASSERT(t4f_c[2]==t4f_a[2]*t4f_b[3])
    G_ASSERT(t4f_c[3]==t4f_a[3]*t4f_b[3])

    t4f_c=t4f_a/t4f_b[3];
    G_ASSERT(OpCmpEqualEps::evaluate(t4f_c[0],t4f_a[0]/t4f_b[3],0.000001f))
    G_ASSERT(OpCmpEqualEps::evaluate(t4f_c[1],t4f_a[1]/t4f_b[3],0.000001f))
    G_ASSERT(OpCmpEqualEps::evaluate(t4f_c[2],t4f_a[2]/t4f_b[3],0.000001f))
    G_ASSERT(OpCmpEqualEps::evaluate(t4f_c[3],t4f_a[3]/t4f_b[3],0.000001f))

    t4f_a=2.0f/t4f_c;
    G_ASSERT(OpCmpEqualEps::evaluate(t4f_a[0],2.0f/t4f_c[0],0.001f))
    G_ASSERT(OpCmpEqualEps::evaluate(t4f_a[1],2.0f/t4f_c[1],0.001f))
    G_ASSERT(OpCmpEqualEps::evaluate(t4f_a[2],2.0f/t4f_c[2],0.001f))
    G_ASSERT(OpCmpEqualEps::evaluate(t4f_a[3],2.0f/t4f_c[3],0.001f))
#endif

    cout << "Check construction of objects ..."
         << endl;
    Tuple<2,int> t2i_a(1,2),t2i_b(r.getRandom(),r.getRandom());

    Tuple<3,float> t3f_a(3.0f,4.0f,5.0f);
    Tuple<3,float> t3f_b(
        static_cast<float>(r.getRandom0N(1000.0f)),
        static_cast<float>(r.getRandom0N(1000.0f)),
        static_cast<float>(r.getRandom0N(1000.0f))
    );

    Tuple<4,double> t4d_a(5.0,6.0,7.0,8.0);
    Tuple<4,double> t4d_b(
        static_cast<double>(r.getRandom0N(1000.0)),
        static_cast<double>(r.getRandom0N(1000.0)),
        static_cast<double>(r.getRandom0N(1000.0)),
        static_cast<double>(r.getRandom0N(1000.0))
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
        double const s=0.63;
        Tuple<4,double> tmp=t4d_a.getLerp(t4d_b,s);
        G_ASSERT(OpCmpEqualEps::evaluate(tmp[0],t4d_a[0]+(t4d_b[0]-t4d_a[0])*s))
        G_ASSERT(OpCmpEqualEps::evaluate(tmp[1],t4d_a[1]+(t4d_b[1]-t4d_a[1])*s))
        G_ASSERT(OpCmpEqualEps::evaluate(tmp[2],t4d_a[2]+(t4d_b[2]-t4d_a[2])*s))
        G_ASSERT(OpCmpEqualEps::evaluate(tmp[3],t4d_a[3]+(t4d_b[3]-t4d_a[3])*s))
    }

    cout << "Check remaining vector vs. vector operators ..."
         << endl;
    {
        Tuple<3,float> tmp(
            static_cast<float>(r.getRandom0N(1000.0f)),
            static_cast<float>(r.getRandom0N(1000.0f)),
            static_cast<float>(r.getRandom0N(1000.0f))
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
    G_ASSERT((Vec3i::X()^Vec3i::Y()).equals(Vec3i::Z()))

    cout << "Check angle related methods ..."
         << endl;
    double axy1=Vec3i::X().getAngle(Vec3i::Y());
    double axy2=Vec3i::X().getAccurateAngle(Vec3i::Y());
    G_ASSERT(OpCmpEqualEps::evaluate(axy1,axy2))

    cout << "Check getting an orthogonal vector ..."
         << endl;
    G_ASSERT(Vec3f::X().getOrthoVector().equals(Vec3f::Z()))
    G_ASSERT(Vec3f::Y().getOrthoVector().equals(-Vec3f::Z()))
    G_ASSERT(Vec3f::Z().getOrthoVector().equals(Vec3f::Y()))

    cout << "Check unary sign, conversion constructor and collinear methods ..."
         << endl;
    G_ASSERT(Vec3f::X().isCollinear(-Vec3i::X()))

    cout << "Check the dot product operator ..."
         << endl;
    G_ASSERT((Vec3i::X()%Vec3i::Y())==0)
    G_ASSERT((Vec3i::X()%Vec3i::Z())==0)
    G_ASSERT((Vec3i::Y()%Vec3i::Z())==0)

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
    static Col3d const col3d(Col3d::getMinIntensity(),Col3d::getMinIntensity(),Col3d::getMinIntensity());
    G_ASSERT(col3d==black)

    Col3f blue=Col3f::BLUE();
    static Col3f const col3f(Col3f::getMinIntensity(),Col3f::getMinIntensity(),Col3f::getMaxIntensity());
    G_ASSERT(col3f==blue)

    Col3i green=Col3i::GREEN();
    static Col3i const col3i(Col3i::getMinIntensity(),Col3i::getMaxIntensity(),Col3i::getMinIntensity());
    G_ASSERT(col3i==green)

    Col3ui cyan=Col3ui::CYAN();
    static Col3ui const col3ui(Col3ui::getMinIntensity(),Col3ui::getMaxIntensity(),Col3ui::getMaxIntensity());
    G_ASSERT(col3ui==cyan)

    Col3s red=Col3s::RED();
    static Col3s const col3s(Col3s::getMaxIntensity(),Col3s::getMinIntensity(),Col3s::getMinIntensity());
    G_ASSERT(col3s==red)

    Col3us magenta=Col3us::MAGENTA();
    static Col3us const col3us(Col3us::getMaxIntensity(),Col3us::getMinIntensity(),Col3us::getMaxIntensity());
    G_ASSERT(col3us==magenta)

    Col3b yellow=Col3b::YELLOW();
    static Col3b const col3b(Col3b::getMaxIntensity(),Col3b::getMaxIntensity(),Col3b::getMinIntensity());
    G_ASSERT(col3b==yellow)

    Col3ub white=Col3ub::WHITE();
    static Col3ub const col3ub(Col3ub::getMaxIntensity(),Col3ub::getMaxIntensity(),Col3ub::getMaxIntensity());
    G_ASSERT(col3ub==white)

    G_ASSERT(white.invert()==Col3ub::BLACK())

    Col4f white4=Col4f::WHITE();
    white4.setA(0.5);
    Col4f black4=Col4f::BLACK();
    black4.setA(0.5);
    G_ASSERT(!white4==black4)
}
