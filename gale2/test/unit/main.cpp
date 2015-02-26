// Enable memory leak detection, see:
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vsdebug/html/vxcondetectingisolatingmemoryleaks.asp
#if !defined NDEBUG && !defined GALE_TINY_CODE
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
#endif

#define CATCH_CONFIG_RUNNER
#include <catch.hpp>

#include <gale/global/dynamicarray.h>

#include <gale/math/biasscale.h>
#include <gale/math/color.h>
#include <gale/math/colormodel.h>
#include <gale/math/hmatrix4.h>
#include <gale/math/matrix4.h>
#include <gale/math/quaternion.h>
#include <gale/math/random.h>

#include <gale/system/cpuinfo.h>
#include <gale/system/timer.h>

TEST_CASE("DynamicArray class tests") {
    using namespace gale::global;

    int i;
    DynamicArray<int> a;

    SECTION("Find in an empty array") {
        REQUIRE_FALSE(a.findSorted(42, i));
        REQUIRE(i == 0);
    }

    a.setSize(5);
    a = 1, 3, 5, 7, 11;

    SECTION("Find existing entries") {
        REQUIRE(a.findSorted(1, i));
        REQUIRE(i == 0);
        REQUIRE(a.findSorted(3, i));
        REQUIRE(i == 1);
        REQUIRE(a.findSorted(5, i));
        REQUIRE(i == 2);
        REQUIRE(a.findSorted(7, i));
        REQUIRE(i == 3);
        REQUIRE(a.findSorted(11, i));
        REQUIRE(i == 4);
    }

    SECTION("Find non-existing entries") {
        REQUIRE_FALSE(a.findSorted(0, i));
        REQUIRE(i == 0);
        REQUIRE_FALSE(a.findSorted(2, i));
        REQUIRE(i == 1);
        REQUIRE_FALSE(a.findSorted(10, i));
        REQUIRE(i == 4);
        REQUIRE_FALSE(a.findSorted(12, i));
        REQUIRE(i == 5);
    }

    a.insert(13);

    SECTION("Find after insert") {
        REQUIRE(a.findSorted(5, i));
        REQUIRE(i == 2);
        REQUIRE(a.findSorted(7, i));
        REQUIRE(i == 3);
        REQUIRE(a.findSorted(13, i));
        REQUIRE(i == 5);

        REQUIRE_FALSE(a.findSorted(0, i));
        REQUIRE(i == 0);
        REQUIRE_FALSE(a.findSorted(14, i));
        REQUIRE(i == 6);
    }
}

TEST_CASE("BiasScale class tests") {
    using namespace gale::math;

    RandomEcuyerf e;
    BiasScalef bs1(e.random01(), e.random01());

    BiasScalef bs2(bs1);
    bs2.invert();
    bs1.concat(bs2);
    REQUIRE(bs1.isNOP());

    BiasScalef bs3 = !bs1;
    REQUIRE((bs1 & bs3).isNOP());
}

TEST_CASE("ScaleBias class tests") {
    using namespace gale::math;

    RandomEcuyerf e;
    ScaleBiasf sb1(e.random01(), e.random01());

    ScaleBiasf sb2(sb1);
    sb2.invert();
    sb1.concat(sb2);
    REQUIRE(sb1.isNOP());

    ScaleBiasf sb3 = !sb1;
    REQUIRE((sb1 & sb3).isNOP());
}

TEST_CASE("Homogeneous matrix class tests") {
    using namespace gale::math;

    HMat4d m, n;
    RandomEcuyerd r;

    SECTION("Inversion") {
        Vec3d axis = Vec3d::random(r);
        double angle = r.random0ExclN(Constd::PI() * 2);

        Vec3d dir = Vec3d::random(r);
        double factor = r.randomExcl0N(10.0);

        double sx = r.randomExcl0N(10.0);
        double sy = r.randomExcl0N(10.0);
        double sz = r.randomExcl0N(10.0);

        m = HMat4d::IDENTITY();
        m *= HMat4d::Factory::Rotation(axis, angle);
        m *= HMat4d::Factory::Translation(dir, factor);
        m *= HMat4d::Factory::Scaling(sx, sy, sz);

        n = m;
        n.invert();

        REQUIRE((m * n) == HMat4d::IDENTITY());
    }

    SECTION("Normalization") {
        m = HMat4d::random(r);
        n = !m;
        m *= n;

        REQUIRE(m == HMat4d::IDENTITY());
        REQUIRE((n * HMat4d::IDENTITY()) == n);
    }

    SECTION("Arithmetics") {
        m(0, 3) = r.random01();
        m(1, 3) = r.random01();
        m(2, 3) = r.random01();

        n = m + m;

        REQUIRE(m == n / 2);
        REQUIRE((n - m) == m);
    }

    SECTION("Rotation") {
        Vec3d axis = Vec3d::random(r);
        double angle = r.random0ExclN(Constd::PI() * 2);
        Vec3d v = Vec3d::random(r);

        m = HMat4d::Factory::Rotation(axis, angle);
        Vec3d vrm = m * v;

        Quatd q(axis, angle);
        Vec3d vrq = q * v;

        REQUIRE(vrm.equals(vrq));
    }
}

TEST_CASE("Matrix class tests") {
    using namespace gale::math;

    SECTION("Projection") {
        Mat4f projection = Mat4f::Factory::Projection(Vec3f::Y(), Vec3f(-2, 2, 0));

        REQUIRE((projection * Vec3f(-1, 1, 0)) == Vec3f::ZERO());
        REQUIRE((projection * Vec3f(0, 1, 0)) == Vec3f(2, 0, 0));
        REQUIRE((projection * Vec3f(-2, 1, 0)) == Vec3f(-2, 0, 0));
    }

    SECTION("Inversion") {
        RandomEcuyerd r;
        Mat4d A = Mat4d::random(r), B = A, C;

        bool result;
        B.invert(&result);
        REQUIRE(result);

        C = A * B;
        REQUIRE(C == Mat4d::IDENTITY());
    }
}

TEST_CASE("Random generator tests") {
    using namespace gale::math;
    using namespace gale::system;

    const int N = 2000000;

    unsigned int half0, half1;
    unsigned int even, odd;

    RandomEcuyerf e;
    Timer timer;

    SECTION("RandomEcuyerf benchmark") {
        double s;
        timer.start();
        for (int i = N; i >= 0; --i) {
            e.random();
        }
        timer.stop(s);

        int p = static_cast<int>(N * (1.0 / s));
        INFO(p << " random numbers per second");
        REQUIRE(p > 40000000);
    }

    SECTION("RandomEcuyerf distribution") {
        half0 = half1 = 0;
        even = odd = 0;

        for (int i = N; i >= 0; --i) {
            unsigned int n = e.random();

            if (n < UINT_MAX / 2) {
                ++half0;
            }
            else {
                ++half1;
            }

            if (n & 1) {
                ++odd;
            }
            else {
                ++even;
            }
        }

        CAPTURE(half0);
        CAPTURE(half1);
        REQUIRE(abs(static_cast<double>(half0) / half1) < 1.01);

        CAPTURE(even);
        CAPTURE(odd);
        REQUIRE(abs(static_cast<double>(even) / odd) < 1.01);
    }

    srand(static_cast<unsigned>(time(NULL)));

    SECTION("rand() benchmark") {
        double s;
        timer.start();
        for (int i = N; i >= 0; --i) {
            rand();
        }
        timer.stop(s);

        int p = static_cast<int>(N * (1.0 / s));
        INFO(p << " random numbers per second");
        REQUIRE(p > 20000000);
    }

    SECTION("rand() distribution") {
        half0 = half1 = 0;
        even = odd = 0;

        for (int i = N; i >= 0; --i) {
            unsigned int r = rand();

            if (r < RAND_MAX / 2) {
                ++half0;
            }
            else {
                ++half1;
            }

            if (r & 1) {
                ++odd;
            }
            else {
                ++even;
            }
        }

        CAPTURE(half0);
        CAPTURE(half1);
        REQUIRE(abs(static_cast<double>(half0) / half1) < 1.01);

        CAPTURE(even);
        CAPTURE(odd);
        REQUIRE(abs(static_cast<double>(even) / odd) < 1.01);
    }
}

TEST_CASE("CPU class tests") {
    using namespace gale::system;

    INFO("Found " << CPU.processors() << " processor(s), " << CPU.coresPerPhysicalProc() << " core(s) per processor, " << CPU.logicalProcsPerCore() << " thread(s) per core.");
    unsigned int total_procs = CPU.processors() * CPU.coresPerPhysicalProc() * CPU.logicalProcsPerCore();
    REQUIRE(total_procs == atoi(getenv("NUMBER_OF_PROCESSORS")));

    REQUIRE(strlen(CPU.vendorString()) > 0);
    REQUIRE(CPU.isAMD() != CPU.isIntel());

    if (CPU.hasHTT()) {
        REQUIRE(CPU.logicalProcsPerCore() > 0);
    }

    if (CPU.hasMMXExt()) {
        REQUIRE(CPU.hasMMX());
    }

    if (CPU.has3DNowExt()) {
        REQUIRE(CPU.has3DNow());
    }

    if (CPU.hasSSE2()) {
        REQUIRE(CPU.hasSSE());
    }

    if (CPU.hasSSE3()) {
        REQUIRE(CPU.hasSSE2());
    }

    if (CPU.hasSSSE3()) {
        REQUIRE(CPU.hasSSE3());
    }
}

TEST_CASE("Tuple class tests") {
    using namespace gale::math;
    using gale::meta::OpCmpEqual;

    RandomEcuyerf e;

    Tuple<2, int> t2i_a(1, 2), t2i_b(e.random(), e.random());

    Tuple<3, float> t3f_a(3.0f, 4.0f, 5.0f);
    Tuple<3, float> t3f_b(
        e.random0N(1000.0f)
    ,   e.random0N(1000.0f)
    ,   e.random0N(1000.0f)
    );

    Tuple<4, double> t4d_a(5.0, 6.0, 7.0, 8.0);
    Tuple<4, double> t4d_b(
        e.random0N(1000.0f)
    ,   e.random0N(1000.0f)
    ,   e.random0N(1000.0f)
    ,   e.random0N(1000.0f)
    );

    SECTION("Construction") {
        CAPTURE(t2i_a);
        CAPTURE(t2i_b);

        REQUIRE(sizeof(t2i_a) == 2 * sizeof(int));
        REQUIRE(sizeof(t2i_b) == 2 * sizeof(int));

        CAPTURE(t3f_a);
        CAPTURE(t3f_b);

        REQUIRE(sizeof(t3f_a) == 3 * sizeof(float));
        REQUIRE(sizeof(t3f_b) == 3 * sizeof(float));

        CAPTURE(t4d_a);
        CAPTURE(t4d_b);

        REQUIRE(sizeof(t4d_a) == 4 * sizeof(double));
        REQUIRE(sizeof(t4d_b) == 4 * sizeof(double));
    }

    SECTION("Artithmetics") {
        float t4f_a0 = 2.0f, t4f_a1 = 3.0f, t4f_a2 = 5.0f, t4f_a3 = 7.0f;
        float t4f_b0 = 9.0f, t4f_b1 = 8.0f, t4f_b2 = 7.0f, t4f_b3 = 6.0f;

        Tuple<4, float> t4f_a(t4f_a0, t4f_a1, t4f_a2, t4f_a3);
        Tuple<4, float> t4f_b(t4f_b0, t4f_b1, t4f_b2, t4f_b3);

        Tuple<4, float> t4f_c = t4f_a.minElements(t4f_b);
        REQUIRE(t4f_c[0] == gale::math::min(t4f_a0, t4f_b0));
        REQUIRE(t4f_c[1] == gale::math::min(t4f_a1, t4f_b1));
        REQUIRE(t4f_c[2] == gale::math::min(t4f_a2, t4f_b2));
        REQUIRE(t4f_c[3] == gale::math::min(t4f_a3, t4f_b3));

        REQUIRE(t4f_c <= t4f_a);
        REQUIRE(t4f_c <= t4f_b);

        t4f_c = lerp(t4f_a, t4f_b, 0.5f);
        REQUIRE(t4f_c[0] == (t4f_a0 + t4f_b0) / 2);
        REQUIRE(t4f_c[1] == (t4f_a1 + t4f_b1) / 2);
        REQUIRE(t4f_c[2] == (t4f_a2 + t4f_b2) / 2);
        REQUIRE(t4f_c[3] == (t4f_a3 + t4f_b3) / 2);

        t4f_c = -t4f_a + t4f_b;
        REQUIRE(t4f_c[0] == -t4f_a0 + t4f_b0);
        REQUIRE(t4f_c[1] == -t4f_a1 + t4f_b1);
        REQUIRE(t4f_c[2] == -t4f_a2 + t4f_b2);
        REQUIRE(t4f_c[3] == -t4f_a3 + t4f_b3);

        t4f_a = t4f_b*t4f_c;
        REQUIRE(t4f_a[0] == t4f_b[0] * t4f_c[0]);
        REQUIRE(t4f_a[1] == t4f_b[1] * t4f_c[1]);
        REQUIRE(t4f_a[2] == t4f_b[2] * t4f_c[2]);
        REQUIRE(t4f_a[3] == t4f_b[3] * t4f_c[3]);

        t4f_c = t4f_a*t4f_b[3];
        REQUIRE(t4f_c[0] == t4f_a[0] * t4f_b[3]);
        REQUIRE(t4f_c[1] == t4f_a[1] * t4f_b[3]);
        REQUIRE(t4f_c[2] == t4f_a[2] * t4f_b[3]);
        REQUIRE(t4f_c[3] == t4f_a[3] * t4f_b[3]);

        t4f_c = t4f_a / t4f_b[3];
        REQUIRE(OpCmpEqual::evaluate(t4f_c[0], t4f_a[0] / t4f_b[3]));
        REQUIRE(OpCmpEqual::evaluate(t4f_c[1], t4f_a[1] / t4f_b[3]));
        REQUIRE(OpCmpEqual::evaluate(t4f_c[2], t4f_a[2] / t4f_b[3]));
        REQUIRE(OpCmpEqual::evaluate(t4f_c[3], t4f_a[3] / t4f_b[3]));

        t4f_a = 2.0f / t4f_c;
        REQUIRE(OpCmpEqual::evaluate(t4f_a[0], 2.0f / t4f_c[0], 1e-03f));
        REQUIRE(OpCmpEqual::evaluate(t4f_a[1], 2.0f / t4f_c[1], 1e-03f));
        REQUIRE(OpCmpEqual::evaluate(t4f_a[2], 2.0f / t4f_c[2], 1e-03f));
        REQUIRE(OpCmpEqual::evaluate(t4f_a[3], 2.0f / t4f_c[3], 1e-03f));

        SECTION("Addition") {
            Tuple<2, int> tmp = t2i_a + t2i_b;
            REQUIRE(tmp[0] == t2i_a[0] + t2i_b[0]);
            REQUIRE(tmp[1] == t2i_a[1] + t2i_b[1]);
        }

        SECTION("Subtraction") {
            Tuple<2, int> tmp = t2i_a - t2i_b;
            REQUIRE(tmp[0] == t2i_a[0] - t2i_b[0]);
            REQUIRE(tmp[1] == t2i_a[1] - t2i_b[1]);
        }

        SECTION("Division") {
            Tuple<2, int> tmp = t2i_a / t2i_b;
            REQUIRE(tmp[0] == t2i_a[0] / t2i_b[0]);
            REQUIRE(tmp[1] == t2i_a[1] / t2i_b[1]);
        }

        SECTION("Division (scalar)") {
            float s = 12.3f;
            Tuple<3, float> tmp = t3f_a / s;
            REQUIRE(OpCmpEqual::evaluate(tmp[0], t3f_a[0] / s));
            REQUIRE(OpCmpEqual::evaluate(tmp[1], t3f_a[1] / s));
            REQUIRE(OpCmpEqual::evaluate(tmp[2], t3f_a[2] / s));
        }

        SECTION("Unary operators") {
            Tuple<3, float> tmp1 = +t3f_a;
            REQUIRE(tmp1 == t3f_a);

            Tuple<3, float> tmp2 = -t3f_b;
            REQUIRE(tmp2 == -t3f_b);
        }

        SECTION("Minimum / maximum") {
            double min = t4d_b.minElement();
            REQUIRE(min <= t4d_b[0]);
            REQUIRE(min <= t4d_b[1]);
            REQUIRE(min <= t4d_b[2]);

            double max = t4d_b.maxElement();
            REQUIRE(max >= t4d_b[0]);
            REQUIRE(max >= t4d_b[1]);
            REQUIRE(max >= t4d_b[2]);
            REQUIRE(max >= t4d_b[3]);
        }

        SECTION("Minimum / maximum (absolute)") {
            float min = t3f_a.absMinElement();
            REQUIRE(min <= abs(t3f_a[0]));
            REQUIRE(min <= abs(t3f_a[1]));
            REQUIRE(min <= abs(t3f_a[2]));

            float max = t3f_a.absMaxElement();
            REQUIRE(max >= abs(t3f_a[0]));
            REQUIRE(max >= abs(t3f_a[1]));
            REQUIRE(max >= abs(t3f_a[2]));
        }

        SECTION("Minimum / maximum (element-wise)") {
            Tuple<3, float> min = t3f_a.minElements(t3f_b);
            REQUIRE(min[0] <= t3f_a[0]);
            REQUIRE(min[0] <= t3f_b[0]);
            REQUIRE(min[1] <= t3f_a[1]);
            REQUIRE(min[1] <= t3f_b[1]);
            REQUIRE(min[2] <= t3f_a[2]);
            REQUIRE(min[2] <= t3f_b[2]);

            Tuple<3, float> max = t3f_a.maxElements(t3f_b);
            REQUIRE(max[0] >= t3f_a[0]);
            REQUIRE(max[0] >= t3f_b[0]);
            REQUIRE(max[1] >= t3f_a[1]);
            REQUIRE(max[1] >= t3f_b[1]);
            REQUIRE(max[2] >= t3f_a[2]);
            REQUIRE(max[2] >= t3f_b[2]);
        }

        SECTION("Comparison") {
            bool res_l = (t3f_a < t3f_b);
            REQUIRE(res_l == (t3f_a[0] < t3f_b[0] && t3f_a[1] < t3f_b[1] && t3f_a[2] < t3f_b[2]));

            bool res_le = (t3f_a <= t3f_b);
            REQUIRE(res_le == (t3f_a[0] <= t3f_b[0] && t3f_a[1] <= t3f_b[1] && t3f_a[2] <= t3f_b[2]));

            bool res_g = (t3f_a > t3f_b);
            REQUIRE(res_g == (t3f_a[0] > t3f_b[0] && t3f_a[1] > t3f_b[1] && t3f_a[2] > t3f_b[2]));

            bool res_ge = (t3f_a >= t3f_b);
            REQUIRE(res_ge == (t3f_a[0] >= t3f_b[0] && t3f_a[1] >= t3f_b[1] && t3f_a[2] >= t3f_b[2]));

            bool res_eq = (t2i_a == t2i_b);
            REQUIRE(res_eq == (t2i_a[0] == t2i_b[0] && t2i_a[1] == t2i_b[1]));

            bool res_ne = (t2i_a != t2i_b);
            REQUIRE(res_ne == (t2i_a[0] != t2i_b[0] || t2i_a[1] != t2i_b[1]));
        }

        SECTION("Linear interpolation") {
            float const s = 0.63f;
            Tuple<4, double> tmp = lerp(t4d_a, t4d_b, s);
            REQUIRE(OpCmpEqual::evaluate(tmp[0], t4d_a[0] + (t4d_b[0] - t4d_a[0]) * s));
            REQUIRE(OpCmpEqual::evaluate(tmp[1], t4d_a[1] + (t4d_b[1] - t4d_a[1]) * s));
            REQUIRE(OpCmpEqual::evaluate(tmp[2], t4d_a[2] + (t4d_b[2] - t4d_a[2]) * s));
            REQUIRE(OpCmpEqual::evaluate(tmp[3], t4d_a[3] + (t4d_b[3] - t4d_a[3]) * s));
        }

        SECTION("Miscellaneous") {
            Tuple<3, float> tmp(
                e.random0N(1000.0f)
            ,   e.random0N(1000.0f)
            ,   e.random0N(1000.0f)
            );

            Tuple<3, float> res = ((t3f_a + t3f_b) - t3f_b) * tmp;
            REQUIRE(OpCmpEqual::evaluate(res[0], (t3f_a[0] + t3f_b[0] - t3f_b[0])*tmp[0]));
            REQUIRE(OpCmpEqual::evaluate(res[1], (t3f_a[1] + t3f_b[1] - t3f_b[1])*tmp[1]));
            REQUIRE(OpCmpEqual::evaluate(res[2], (t3f_a[2] + t3f_b[2] - t3f_b[2])*tmp[2]));

            Tuple<3, float> res2 = tmp / res;
            REQUIRE(OpCmpEqual::evaluate(res2[0], tmp[0] / res[0]));
            REQUIRE(OpCmpEqual::evaluate(res2[1], tmp[1] / res[1]));
            REQUIRE(OpCmpEqual::evaluate(res2[2], tmp[2] / res[2]));
        }

        SECTION("Miscellaneous (scalar)") {
            float s = 344.5f, t = 24.6f;

            Tuple<3, float> res = t3f_a * s;
            REQUIRE(OpCmpEqual::evaluate(res[0], t3f_a[0] * s));
            REQUIRE(OpCmpEqual::evaluate(res[1], t3f_a[1] * s));
            REQUIRE(OpCmpEqual::evaluate(res[2], t3f_a[2] * s));

            Tuple<3, float> res2 = res / t;
            REQUIRE(OpCmpEqual::evaluate(res2[0], res[0] / t));
            REQUIRE(OpCmpEqual::evaluate(res2[1], res[1] / t));
            REQUIRE(OpCmpEqual::evaluate(res2[2], res[2] / t));
        }
    }
}

TEST_CASE("Vector class tests") {
    using namespace gale::math;
    using gale::meta::OpCmpEqual;

    SECTION("Constants") {
        Vec4d n(Vec4d::ZERO());
        Vec4d x(Vec4d::X()), y(Vec4d::Y()), z(Vec4d::Z()), w(Vec4d::W());

        REQUIRE(n[0] == 0);
        REQUIRE(n[1] == 0);
        REQUIRE(n[2] == 0);
        REQUIRE(n[3] == 0);

        REQUIRE(x[0] == 1);
        REQUIRE(x[1] == 0);
        REQUIRE(x[2] == 0);
        REQUIRE(x[3] == 0);

        REQUIRE(y[0] == 0);
        REQUIRE(y[1] == 1);
        REQUIRE(y[2] == 0);
        REQUIRE(y[3] == 0);

        REQUIRE(z[0] == 0);
        REQUIRE(z[1] == 0);
        REQUIRE(z[2] == 1);
        REQUIRE(z[3] == 0);

        REQUIRE(w[0] == 0);
        REQUIRE(w[1] == 0);
        REQUIRE(w[2] == 0);
        REQUIRE(w[3] == 1);
    }

    SECTION("Constructors") {
        Vec4i a(12, 34, 56, 78);
        Vec4d b(a);

        REQUIRE(double(a.getX()) == b[0]);
        REQUIRE(double(a.getY()) == b[1]);
        REQUIRE(double(a.getZ()) == b[2]);
        REQUIRE(double(a.getW()) == b[3]);
    }

    SECTION("Arithmetics") {
        Vec4d a(1, 2, 3, 4);
        a.normalize();
        REQUIRE(OpCmpEqual::evaluate(a.length2(), Vec4d::X()[0]));

        REQUIRE((Vec3i::X() ^ Vec3i::Y()).equals(Vec3i::Z()));

        double axy1 = Vec3i::X().angle(Vec3i::Y());
        double axy2 = Vec3i::X().orientedAngle(Vec3i::Y(), Vec3i::Z());
        REQUIRE(OpCmpEqual::evaluate(axy1, axy2));

        REQUIRE(Vec3f::X().orthoVector().equals(Vec3f::Z()));
        REQUIRE(Vec3f::Y().orthoVector().equals(-Vec3f::Z()));
        REQUIRE(Vec3f::Z().orthoVector().equals(Vec3f::Y()));

        REQUIRE(Vec3f::X().isCollinear(-Vec3i::X()));

        REQUIRE((Vec3i::X() % Vec3i::Y()) == 0);
        REQUIRE((Vec3i::X() % Vec3i::Z()) == 0);
        REQUIRE((Vec3i::Y() % Vec3i::Z()) == 0);
    }
}

TEST_CASE("Color class tests") {
    using namespace gale::math;
    using gale::meta::OpCmpEqual;

    SECTION("Constants") {
        Col3d black = Col3d::BLACK();
        static Col3d const col3d(Col3d::MIN_VALUE(), Col3d::MIN_VALUE(), Col3d::MIN_VALUE());
        REQUIRE(col3d == black);

        Col3f blue = Col3f::BLUE();
        static Col3f const col3f(Col3f::MIN_VALUE(), Col3f::MIN_VALUE(), Col3f::MAX_VALUE());
        REQUIRE(col3f == blue);

        Col3i green = Col3i::GREEN();
        static Col3i const col3i(Col3i::MIN_VALUE(), Col3i::MAX_VALUE(), Col3i::MIN_VALUE());
        REQUIRE(col3i == green);

        Col3ui cyan = Col3ui::CYAN();
        static Col3ui const col3ui(Col3ui::MIN_VALUE(), Col3ui::MAX_VALUE(), Col3ui::MAX_VALUE());
        REQUIRE(col3ui == cyan);

        Col3s red = Col3s::RED();
        static Col3s const col3s(Col3s::MAX_VALUE(), Col3s::MIN_VALUE(), Col3s::MIN_VALUE());
        REQUIRE(col3s == red);

        Col3us magenta = Col3us::MAGENTA();
        static Col3us const col3us(Col3us::MAX_VALUE(), Col3us::MIN_VALUE(), Col3us::MAX_VALUE());
        REQUIRE(col3us == magenta);

        Col3b yellow = Col3b::YELLOW();
        static Col3b const col3b(Col3b::MAX_VALUE(), Col3b::MAX_VALUE(), Col3b::MIN_VALUE());
        REQUIRE(col3b == yellow);

        Col3ub white = Col3ub::WHITE();
        static Col3ub const col3ub(Col3ub::MAX_VALUE(), Col3ub::MAX_VALUE(), Col3ub::MAX_VALUE());
        REQUIRE(col3ub == white);
    }

    SECTION("Mixing") {
        REQUIRE(Col4i::RED().mixAdd(Col4i::GREEN()) == Col4i::YELLOW());
        REQUIRE(Col4i::GREEN().mixAdd(Col4i::BLUE()) == Col4i::CYAN());
        REQUIRE(Col4i::BLUE().mixAdd(Col4i::RED()) == Col4i::MAGENTA());

        REQUIRE(Col4i::RED().mixAdd(Col4i::GREEN()).mixAdd(Col4i::BLUE()) == Col4i::WHITE());

        REQUIRE(Col3us::CYAN().mixSub(Col3us::MAGENTA()) == Col3us::BLUE());
        REQUIRE(Col3us::MAGENTA().mixSub(Col3us::YELLOW()) == Col3us::RED());
        REQUIRE(Col3us::YELLOW().mixSub(Col3us::CYAN()) == Col3us::GREEN());

        REQUIRE(Col3us::CYAN().mixSub(Col3us::MAGENTA()).mixSub(Col3us::YELLOW()) == Col3us::BLACK());
    }

    SECTION("Complement") {
        REQUIRE(Col3ub::RED().complement() == Col3ub::CYAN());
        REQUIRE(Col3ub::GREEN().complement() == Col3ub::MAGENTA());
        REQUIRE(Col3ub::BLUE().complement() == Col3ub::YELLOW());

        REQUIRE(Col3ub::WHITE().complement() == Col3ub::BLACK());

        Col4f white = Col4f::WHITE();
        white.setA(0.5);
        Col4f black = Col4f::BLACK();
        black.setA(0.5);
        REQUIRE(!white == black);

        RandomEcuyerf e;

        Col3b srgb, neutral;
        for (int i = 0; i < 1000; ++i) {
            srgb = Col3b::random(e);

            neutral = srgb.mixAdd(srgb.complement());
            REQUIRE(neutral == Col3b::WHITE());

            neutral = srgb.mixSub(srgb.complement());
            REQUIRE(neutral == Col3b::BLACK());
        }
    }

    SECTION("Conversion") {
        REQUIRE(Col4i::RED() == Col4i(Col4ub::RED()));
        REQUIRE(Col3f::GREEN() == Col3f(Col3b::GREEN()));
        REQUIRE(Col4us::BLUE() == Col4us(Col4d::BLUE()));

        RandomEcuyerf e;

        Col3ub::Type r, g, b;
        ColorModelHSV hsv;
        Col3ub rgb;
        for (int i = 0; i < 1000; ++i) {
            r = static_cast<Col3ub::Type>(e.random(255));
            g = static_cast<Col3ub::Type>(e.random(255));
            b = static_cast<Col3ub::Type>(e.random(255));

            hsv.fromRGB(r / 255.0f, g / 255.0f, b / 255.0f);
            rgb = hsv.getRGB<Col3ub>();

            REQUIRE(OpCmpEqual::evaluate(r, rgb.getR(), Col3ub::Type(1)));
            REQUIRE(OpCmpEqual::evaluate(g, rgb.getG(), Col3ub::Type(1)));
            REQUIRE(OpCmpEqual::evaluate(b, rgb.getB(), Col3ub::Type(1)));
        }
    }
}

int __cdecl main() {
    int result = Catch::Session().run();

#if !defined NDEBUG && !defined GALE_TINY_CODE
    _CrtDumpMemoryLeaks();
#endif

    return result;
}
