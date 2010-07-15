#include <gale/math/color.h>
#include <gale/math/interpolator.h>
#include <gale/wrapgl/defaultwindow.h>

// Enable memory leak detection, see:
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vsdebug/html/vxcondetectingisolatingmemoryleaks.asp
#if !defined NDEBUG && !defined GALE_TINY_CODE
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
#endif

using namespace gale::global;
using namespace gale::math;
using namespace gale::wrapgl;

class TestWindow:public DefaultWindow
{
  public:

    TestWindow()
    :   DefaultWindow(_T("test_interpolator"),800,600)
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

        // Linear interpolation.
        glColor3ubv(Col3ub::WHITE());
        glBegin(GL_LINE_STRIP);
        for (int i=0;i<=dimensions().width;i+=10) {
            Vec2f p=Interpolator::Linear(m_points,static_cast<float>(i)/dimensions().width);
            glVertex2fv(p);
        }
        glEnd();

        // BSpline interpolation.
        glColor3ubv(Col3ub::GREEN());
        glBegin(GL_LINE_STRIP);
        for (int i=0;i<=dimensions().width;i+=10) {
            Vec2f p=Interpolator::BSpline(m_points,static_cast<float>(i)/dimensions().width);
            glVertex2fv(p);
        }
        glEnd();

        // CatmullRom interpolation.
        glColor3ubv(Col3ub::YELLOW());
        glBegin(GL_LINE_STRIP);
        for (int i=0;i<=dimensions().width;i+=10) {
            Vec2f p=Interpolator::CatmullRom(m_points,static_cast<float>(i)/dimensions().width);
            glVertex2fv(p);
        }
        glEnd();
    }

  private:

    DynamicArray<Vec2f> m_points;
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
