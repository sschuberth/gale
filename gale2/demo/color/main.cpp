// Enable memory leak detection, see:
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vsdebug/html/vxcondetectingisolatingmemoryleaks.asp
#if !defined NDEBUG && !defined GALE_TINY_CODE
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
#endif

#include <gale/math/colormodel.h>
#include <gale/math/random.h>
#include <gale/wrapgl/defaultwindow.h>

//#define BACKGROUND_RYB

using namespace gale::math;
using namespace gale::wrapgl;

class TestWindow:public DefaultWindow
{
  public:

    TestWindow()
    :   DefaultWindow(_T("demo_color"),500,500)
    ,   m_value(100.0f)
    ,   m_mode(0)
    {
        // Check the RGB / HSV color conversion.
        double r,g,b,s,v;
        ColorSpaceHSV hsv;
        Col3d rgb_hue_only,rgb_with_saturation,rgb;
        for (int i=0;i<10000;++i) {
            r=m_rand.random01();
            g=m_rand.random01();
            b=m_rand.random01();
            hsv.fromRGB(r,g,b);

            // Converting from HSV to RGB color space is the same as converting
            // only hue to a pure RGB color, and then mixing in white reverse
            // proportionally to the saturation (producing a tint) and mixing in
            // black reverse proportionally to the value (producing a shade).
            s=hsv.getS();
            v=hsv.getV();
            hsv.setS(100);
            hsv.setV(100);

            hsv.toRGB(rgb_hue_only[0],rgb_hue_only[1],rgb_hue_only[2]);
            rgb_with_saturation=lerp(Col3d::WHITE(),rgb_hue_only,s/100.0);
            rgb=lerp(Col3d::BLACK(),rgb_with_saturation,v/100.0);

            assert(gale::meta::OpCmpEqual::evaluate(r,rgb[0]));
            assert(gale::meta::OpCmpEqual::evaluate(g,rgb[1]));
            assert(gale::meta::OpCmpEqual::evaluate(b,rgb[2]));
        }

        // Check the RGB / RYB hue conversion.
        double angle,h_ryb,h_rgb;
        for (int i=0;i<10000;++i) {
            angle=m_rand.random0ExclN(360);
            ColorModelRYB::RYBHueToRGBHue(h_ryb=angle,h_rgb);
            ColorModelRYB::RGBHueToRYBHue(h_rgb,h_ryb);
            assert(gale::meta::OpCmpEqual::evaluate(h_ryb,angle,1e-4));
        }

        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

        // Set the clear color to blue to see if scissoring works.
        Col4f c=Col4f::YELLOW();
        c=c.complement();
        glClearColor(c.getR(),c.getG(),c.getB(),c.getA());
    }

    bool onIdle() {
        static int step=2;

        m_value+=step;
        if (m_value<=0 || m_value>=100) {
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
            m_camera.apply();
            glClear(GL_COLOR_BUFFER_BIT);

            ColorSpaceHSV hsv;
            hsv.setV(m_value);
            glBegin(GL_QUADS);
            unsigned int y0=0,y1;
            for (unsigned int k=0;k<QUADS_Y;++k) {
                hsv.setS(static_cast<float>(k)/QUADS_Y*100);
                y1=((k+1)*step_y)>>16;

                unsigned int x0=0,x1;
                for (unsigned int i=0;i<QUADS_X;++i) {
                    hsv.setH(static_cast<float>(i)/QUADS_X*360);
                    x1=((i+1)*step_x)>>16;

#ifdef BACKGROUND_RYB
                    double hue;
                    ColorModelRYB::RYBHueToRGBHue(hsv.getH(),hue);
                    hsv.setH(hue);
#endif

                    glColor3ubv(hsv.getRGB<Col3ub>());
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
            m_camera.apply();
            glClear(GL_COLOR_BUFFER_BIT);

            glEnable(GL_BLEND);

            m_rand.init(0xdeadbeef);

            ColorSpaceHSV hsv;
            hsv.setV(100);
            glBegin(GL_QUADS);
            unsigned y0=0,y1;
            for (unsigned k=0;k<QUADS_Y;++k) {
                hsv.setS(static_cast<float>(k)/QUADS_Y*100);
                y1=((k+1)*step_y)>>16;

                unsigned x0=0,x1;
                for (unsigned i=0;i<QUADS_X;++i) {
                    hsv.setH(static_cast<float>(i)/QUADS_X*360);
                    x1=((i+1)*step_x)>>16;

                    int alpha=roundToEven(m_value*2.55f*m_rand.random01());
                    Col4ub rgb=hsv.getRGB<Col4ub>();
                    rgb.setA(static_cast<Col4ub::Type>(alpha));
                    glColor4ubv(rgb);
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
