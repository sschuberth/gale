//#include <gale/math/color.h>
//#include <gale/math/interpolator.h>
//#include <gale/math/random.h>
//#include <gale/wrapgl/defaultwindow.h>
//#include <gale/wrapgl/renderer.h>
//#include <gale/wrapgl/shaderobject.h>

//using namespace gale::global;
//using namespace gale::math;
//using namespace gale::model;
//using namespace gale::wrapgl;
//
//#include "bg_frag.inl"

class DemoWindow:public DefaultWindow
{
  public:

    DemoWindow()
    :   DefaultWindow(_T("demo_ssao"),800,600)
    {
        m_camera.approach(-4.0f);
    }

    void onRender() {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        m_camera.apply();

    }

  private:

    ShaderObject m_ssao_frag;
    ProgramObject m_ssao_prog;
};

int __cdecl main()
{
    DemoWindow window;

    ShowWindow(window.windowHandle(),SW_SHOW);
    window.processEvents();

    return 0;
}
