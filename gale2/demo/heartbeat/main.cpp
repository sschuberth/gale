#include <gale/wrapgl/defaultwindow.h>
#include <gale/wrapgl/renderer.h>

// Enable memory leak detection, see:
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vsdebug/html/vxcondetectingisolatingmemoryleaks.asp
#if !defined NDEBUG && !defined GALE_TINY_CODE
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
#endif

using namespace gale::math;
using namespace gale::model;
using namespace gale::wrapgl;

class TestWindow:public DefaultWindow
{
  public:

    TestWindow()
    :   DefaultWindow(_T("demo_heartbeat"),800,600)
    {
        m_camera.approach(-5);

        // Create a heart-shaped contour.
        Mesh::VectorArray heart_shape;
        Mesh::Factory::Shape::Heart(heart_shape,50);

        // Create a straight line path.
        Mesh::VectorArray line_path(5);
        for (int i=0;i<line_path.getSize();++i) {
            line_path[i]=Vec3f(0.0f,0.0f,i*0.1f);
        }

        // Create scaling matrices for the heart shapes.
        Mesh::Factory::MatrixArray heart_scale(5);
        for (int i=0;i<heart_scale.getSize();++i) {
            int dist=abs(i-2);
            float scale=1.0f-dist*dist*0.1f;
            heart_scale[i]=HMat4f::Factory::Scaling(scale,scale,1.0f);
        }

        // Create the heart mesh.
        Mesh* heart=Mesh::Factory::Extruder(line_path,heart_shape,false,&heart_scale);
        Mesh::Subdivider::Loop(*heart,2);

        m_heart_prep.compile(*heart);
        delete heart;

        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    }

    void onRender() {
        glClear(GL_COLOR_BUFFER_BIT);

        m_camera.makeCurrent();

        Renderer::draw(m_heart_prep);
    }

  private:

    PreparedMesh m_heart_prep;
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
