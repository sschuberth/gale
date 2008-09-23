#include <gale/wrapgl/defaultwindow.h>

#include <gale/model/mesh.h>

using namespace gale::math;
using namespace gale::model;
using namespace gale::wrapgl;

class TestWindow:public DefaultWindow
{
  public:

    TestWindow()
    :   DefaultWindow("test_color")
    {
        //m_mesh=Mesh::Factory::Tetrahedron();
        //m_mesh=Mesh::Factory::Octahedron();
        m_mesh=Mesh::Factory::Hexahedron();

        Mesh::Subdivider s(*m_mesh);
        //s.divPolyhedral(3);
        s.divLoop(3);

        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        glEnable(GL_CULL_FACE);
    }

    ~TestWindow() {
        delete m_mesh;
    }

    void onResize(int width,int height) {
        m_camera.setScreenSpaceDimensions(width,height);
        m_camera.setProjection(Mat4d::Factory::PerspectiveProjection(width,height));
    }

    void onPaint() {
        glClear(GL_COLOR_BUFFER_BIT);

        m_camera.apply();

        Mesh::Renderer r(*m_mesh);
        r.compile();
        r.render();
    }

    void onMouseEvent(int x,int y,int wheel,int event) {
        static int mouse_prev_x=0,mouse_prev_y=0;

        // This is positive if the mouse cursor was moved to the right.
        int mouse_diff_x=x-mouse_prev_x;

        // This is positive if the mouse cursor was moved downwards.
        int mouse_diff_y=y-mouse_prev_y;

        if (event!=ME_BUTTON_NONE) {
            if (event&ME_BUTTON_LEFT) {
                HMat4f m=m_camera.getModelview();
                m*=HMat4f::Factory::Translation(-m_camera.getPosition());
                m*=HMat4f::Factory::RotationY(-mouse_diff_x*Constd::DEG_TO_RAD()*0.5);
                m*=HMat4f::Factory::RotationX(-mouse_diff_y*Constd::DEG_TO_RAD()*0.5);
                m*=HMat4f::Factory::Translation(m_camera.getPosition());
                m_camera.setModelview(m);
            }

            if (event&ME_BUTTON_MIDDLE) {
                m_camera.strafe(static_cast<float>(-mouse_diff_x)/100);
                m_camera.elevate(static_cast<float>(mouse_diff_y)/100);
            }

            if (event&ME_BUTTON_RIGHT) {
                m_camera.zoom(mouse_diff_y*Constf::DEG_TO_RAD());
            }

            repaint();
        }

        mouse_prev_x=x;
        mouse_prev_y=y;
    }

  private:

    Mesh* m_mesh;
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

#ifndef NDEBUG
    _CrtDumpMemoryLeaks();
#endif

    return 0;
}
