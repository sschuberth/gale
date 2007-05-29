#include "../test_window.h"

#include <gale/math/color.h>
#include <gale/math/random.h>

using namespace gale::math;
using namespace gale::wrapgl;

class TestWindow:public DefaultWindow
{
  public:

    TestWindow():DefaultWindow("test_color"),m_value(255),m_mode(0) {
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    }

    bool onIdle() {
        static int step=2;

        m_value+=step;
        if ((m_value<=0)||(m_value>=255)) {
            if (step<0) {
                m_value=0;
            } else {
                m_value=255;
            }
            step=-step;
            m_mode=(m_mode+1)&3;
        }

        return true;
    }

    void onResize(int width,int height) {
        m_camera.setScreenSpaceDimensions(width,height);
        m_camera.setProjection(Mat4d::Factory::OrthographicProjection(0,width,0,height));
    }

    void onPaint() {
        glClear(GL_COLOR_BUFFER_BIT);

        m_camera.apply();

        unsigned int const QUADS_X=256;
        unsigned int const QUADS_Y=256;

        // Use fixed point integer math.
        Camera::ScreenSpace screen=m_camera.getScreenSpace();
        unsigned step_x=(screen.width<<16)/QUADS_X;
        unsigned step_y=(screen.height<<16)/QUADS_Y;

        if (m_mode<2) {
            Col3ub color;
            color.setV(m_value);
            glBegin(GL_QUADS);
            unsigned int y0=0,y1;
            for (unsigned int k=0;k<QUADS_Y;++k) {
                color.setS(k);
                y1=((k+1)*step_y)>>16;

                unsigned int x0=0,x1;
                for (unsigned int i=0;i<QUADS_X;++i) {
                    color.setH(i);
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
        } else {
            glEnable(GL_BLEND);

            m_rand.setSeed(0);

            Col4ub color;
            color.setV(255);
            glBegin(GL_QUADS);
            unsigned y0=0,y1;
            for (unsigned k=0;k<QUADS_Y;++k) {
                color.setS(k);
                y1=((k+1)*step_y)>>16;

                unsigned x0=0,x1;
                for (unsigned i=0;i<QUADS_X;++i) {
                    color.setH(i);
                    x1=((i+1)*step_x)>>16;

                    long long alpha=roundToEven(m_value*m_rand.getRandom01());
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

    int m_value,m_mode;
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
    TestWindow window;

    ShowWindow(window.getWindowHandle(),SW_SHOW);
    window.processEvents();

#ifndef NDEBUG
    _CrtDumpMemoryLeaks();
#endif
    return 0;
}
