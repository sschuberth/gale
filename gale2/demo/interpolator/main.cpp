// Enable memory leak detection, see:
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vsdebug/html/vxcondetectingisolatingmemoryleaks.asp
#if !defined NDEBUG && !defined GALE_TINY_CODE
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
#endif

#include <gale/math/color.h>
#include <gale/math/interpolator.h>
#include <gale/wrapgl/defaultwindow.h>

using namespace gale::global;
using namespace gale::math;
using namespace gale::wrapgl;

class TestWindow:public DefaultWindow
{
  public:

    TestWindow()
    :   DefaultWindow(_T("demo_interpolator"),800,600,NULL,0)
    {
        struct S:public Formula
        {
            float operator()(float const x) const {
                return -x*x*10+200;
            }
        } f;

        glPointSize(5.0f);

        // Resemble the graph at <http://www.alglib.net/interpolation/spline3.php>.
        for (int i=0;i<9;++i) {
            float x=i*80.0f+80.0f;
            float y=f(i-4.0f);

            if (i==3) {
                y+=300.0f;
            }

            m_points.insert(Vec2f(x,y));
        }

        static int const N=m_points.getSize()-1;

        // Calculate the tangents as described at the bottom (method 3) at
        // <http://local.wasp.uwa.edu.au/~pbourke/geometry/bezier/cubicbezier.html>.
        m_tangents.setSize(m_points.getSize());
        for (int i=0;i<m_tangents.getSize();++i) {
            int ip=(i==0)?0:i-1;
            int is=(i==N)?N:i+1;

            // Use the vector from the predecessor to the successor as the tangent.
            Vec2f t=m_points[is]-m_points[ip];

            // Get the adjacent vectors along the points.
            Vec2f p0=m_points[i]-m_points[ip];
            Vec2f p1=m_points[is]-m_points[i];

            // Limit the tangent length to half the minimum length of the
            // adjacent vectors.
            float minlen=min(p0.length2(),p1.length2())/(2*2);
            if (t.length2()>minlen) {
                t=~t*sqrt(minlen);
            }

            m_tangents[i]=t;
        }
    }

    void onResize(int width,int height) {
        m_camera.setProjection(Mat4d::Factory::OrthographicProjection(0,width,0,height));
        repaint();
    }

    void onRender() {
        glClear(GL_COLOR_BUFFER_BIT);

        m_camera.apply();

        // Render the points.
        glColor3ubv(Col3ub::RED());
        glBegin(GL_POINTS);
        for (int i=0;i<m_points.getSize();++i) {
            glVertex2fv(m_points[i]);
        }
        glEnd();

        static int const SAMPLES=500;

        // Linear interpolation.
        glColor3ubv(Col3ub::WHITE());
        glBegin(GL_LINE_STRIP);
        for (int i=0;i<=SAMPLES;++i) {
            Vec2f p=Interpolator::Linear(m_points,static_cast<float>(i)/SAMPLES);
            glVertex2fv(p);
        }
        glEnd();

        // Bezier interpolation.
        glColor3ubv(Col3ub::CYAN());
        glBegin(GL_LINE_STRIP);
        for (int i=0;i<=SAMPLES;++i) {
            Vec2f p=Interpolator::Bezier(m_points,m_tangents,static_cast<float>(i)/SAMPLES);
            glVertex2fv(p);
        }
        glEnd();

        // Render the tangents.
        glColor3ubv(Col3ub::BLUE());
        glBegin(GL_LINES);
        for (int i=0;i<m_points.getSize();++i) {
            glVertex2fv(m_points[i]+m_tangents[i]*0.5f);
            glVertex2fv(m_points[i]-m_tangents[i]*0.5f);
        }
        glEnd();

        // B-Spline approximation.
        glColor3ubv(Col3ub::YELLOW());
        glBegin(GL_LINE_STRIP);
        for (int i=0;i<=SAMPLES;++i) {
            Vec2f p=Interpolator::BSpline(m_points,static_cast<float>(i)/SAMPLES);
            glVertex2fv(p);
        }
        glEnd();

        // Catmull-Rom interpolation.
        glColor3ubv(Col3ub::GREEN());
        glBegin(GL_LINE_STRIP);
        for (int i=0;i<=SAMPLES;++i) {
            Vec2f p=Interpolator::CatmullRom(m_points,static_cast<float>(i)/SAMPLES);
            glVertex2fv(p);
        }
        glEnd();

        // Hermite interpolation.
        glColor3ubv(Col3ub::MAGENTA());
        glBegin(GL_LINE_STRIP);
        for (int i=0;i<=SAMPLES;++i) {
            Vec2f p=Interpolator::Hermite(m_points,m_tangents,static_cast<float>(i)/SAMPLES);
            glVertex2fv(p);
        }
        glEnd();
    }

  private:

    DynamicArray<Vec2f> m_points,m_tangents;
};

int __cdecl main()
{
    // Make sure the window is destroyed before dumping memory leaks.
    {
        TestWindow window;

        ShowWindow(window.windowHandle(),SW_SHOW);
        window.processEvents();
    }

#if !defined NDEBUG && !defined GALE_TINY_CODE
    _CrtDumpMemoryLeaks();
#endif

    return 0;
}
