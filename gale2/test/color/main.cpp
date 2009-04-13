#include <gale/math/color.h>
#include <gale/math/random.h>
#include <gale/wrapgl/defaultwindow.h>

using namespace gale::math;
using namespace gale::wrapgl;

class TestWindow:public DefaultWindow
{
  public:

    TestWindow()
    :   DefaultWindow(_T("test_color"))
    ,   m_value(100.0f)
    ,   m_mode(0)
    {
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

        // Set the clear color to blue to see if scissoring works.
        Col4f c=Col4f::YELLOW();
        c=c.complement();
        glClearColor(c.getR(),c.getG(),c.getB(),c.getA());
    }

    bool onIdle() {
        static int step=2;

        m_value+=step;
        if ((m_value<=0)||(m_value>=100)) {
            if (step<0) {
                m_value=0;
            }
            else {
                m_value=100;
            }
            step=-step;
            m_mode=(m_mode+1)&3;
        }

        return true;
    }

    void onResize(int width,int height) {
        m_camera.setProjection(Mat4d::Factory::OrthographicProjection(0,width,0,height));
        repaint();
    }

    void onRender() {
        unsigned int const QUADS_X=256;
        unsigned int const QUADS_Y=256;

        // Use fixed point integer math.
        RenderSurface::Dimensions dims=dimensions();
        unsigned step_x=(dims.width<<16)/QUADS_X;
        unsigned step_y=(dims.height<<16)/QUADS_Y;

        if (m_mode<2) {
            m_camera.setScreenSpaceOrigin(0,0);
            m_camera.setScreenSpaceDimensions(dims.width,dims.height);
            m_camera.makeCurrent();
            glClear(GL_COLOR_BUFFER_BIT);

            Col3ub color;
            color.setV(m_value);
            glBegin(GL_QUADS);
            unsigned int y0=0,y1;
            for (unsigned int k=0;k<QUADS_Y;++k) {
                color.setS(static_cast<float>(k)/QUADS_Y*100);
                y1=((k+1)*step_y)>>16;

                unsigned int x0=0,x1;
                for (unsigned int i=0;i<QUADS_X;++i) {
                    color.setH(static_cast<float>(i)/QUADS_X*360);
                    x1=((i+1)*step_x)>>16;

                    glColor3ubv(color);
                    glVertex2i(x0,y0);
                    glVertex2i(x1,y0);
                    glVertex2i(x1,y1);
                    glVertex2i(x0,y1);

                    x0=x1;
                }
                y0=y1;
            }
            glEnd();
        }
        else {
            m_camera.setScreenSpaceOrigin(dims.width/4,dims.height/4);
            m_camera.setScreenSpaceDimensions(dims.width/2,dims.height/2);
            m_camera.makeCurrent();
            glClear(GL_COLOR_BUFFER_BIT);

            glEnable(GL_BLEND);

            m_rand.init(0);

            Col4ub color;
            color.setV(100);
            glBegin(GL_QUADS);
            unsigned y0=0,y1;
            for (unsigned k=0;k<QUADS_Y;++k) {
                color.setS(static_cast<float>(k)/QUADS_Y*100);
                y1=((k+1)*step_y)>>16;

                unsigned x0=0,x1;
                for (unsigned i=0;i<QUADS_X;++i) {
                    color.setH(static_cast<float>(i)/QUADS_X*360);
                    x1=((i+1)*step_x)>>16;

                    long long alpha=roundToEven(m_value*2.55f*m_rand.random01());
                    color.setA(static_cast<Col4ub::Type>(alpha));
                    glColor4ubv(color);
                    glVertex2i(x0,y0);
                    glVertex2i(x1,y0);
                    glVertex2i(x1,y1);
                    glVertex2i(x0,y1);

                    x0=x1;
                }
                y0=y1;
            }
            glEnd();

            glDisable(GL_BLEND);
        }
    }

  private:

    float m_value;
    int m_mode;
    RandomEcuyerf m_rand;
};

// Enable memory leak detection, see:
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vsdebug/html/vxcondetectingisolatingmemoryleaks.asp
#ifdef _DEBUG
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
#endif

int main()
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
