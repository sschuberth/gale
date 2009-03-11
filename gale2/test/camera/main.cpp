#include <gale/math/color.h>

#include <gale/wrapgl/defaultwindow.h>
#include <gale/wrapgl/renderer.h>

using namespace gale::math;
using namespace gale::model;
using namespace gale::wrapgl;

class TestWindow:public DefaultWindow
{
  public:

    TestWindow()
    :   DefaultWindow(_T("test_camera"),800,600)
    {
        // Set narrow clipping planes for the main camera so we actually see the
        // camera frustum's top and bottom.
        m_camera.setNearClipping(0.5);
        m_camera.setFarClipping(20.0);

        // Create three spectator cameras that look down the negative z-, y- and
        // x-axes.
        m_view_xyz[0]=&m_camera;
        m_view_xyz[0]->approach(-5);
        for (int i=1;i<4;++i) {
            m_view_xyz[i]=new Camera(this);
            m_view_xyz[i]->approach(-10);
        }

        // Multiply the transformation matrix from the left to the camera's
        // modelview matrix to transform it in the world coordinate system.
        HMat4f m;

        m=HMat4f::Factory::Rotation(Vec3f::X(),-90*Constd::DEG_TO_RAD())*m_view_xyz[1]->getModelview();
        m_view_xyz[1]->setModelview(m);

        m=HMat4f::Factory::Rotation(Vec3f::Y(),+90*Constd::DEG_TO_RAD())*m_view_xyz[3]->getModelview();
        m_view_xyz[3]->setModelview(m);

        // Create an object to view at.
        m_mesh=Mesh::Factory::Dodecahedron();
        m_mesh_prep.compile(m_mesh);

        m_mesh_normals=Mesh::Factory::Normals(m_mesh_prep,0.2f);
        m_normals_prep.compile(m_mesh_normals);

        // Set some OpenGL states.
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
    }

    ~TestWindow() {
        delete m_mesh_normals;
        delete m_mesh;

        for (int i=1;i<4;++i) {
            delete m_view_xyz[i];
        }
    }

    void onResize(int width,int height) {
        int x[]={width/2,width/2,-1,-1};
        int y[]={0,height/2+1,height/2+1,0};

        for (int i=0;i<4;++i) {
            // Set the viewport rectangle.
            m_view_xyz[i]->setScreenSpaceOrigin(x[i],y[i]);
            m_view_xyz[i]->setScreenSpaceDimensions(width/2,height/2);
        }
    }

    void onRender() {
        // Clear the background to white for the gap between the viewports.
        glDisable(GL_SCISSOR_TEST);
        glClearColor(1,1,1,0);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glEnable(GL_SCISSOR_TEST);

        glClearColor(0,0,0,0);
        for (int i=0;i<4;++i) {
            m_view_xyz[i]->makeCurrent();
            glClear(GL_COLOR_BUFFER_BIT);

            glEnable(GL_LIGHTING);
            Renderer::draw(m_mesh_prep);

            glDisable(GL_LIGHTING);
            Renderer::draw(m_normals_prep);

            if (i>0) {
                glColor3fv(Col3f::RED());
                Renderer::draw(m_camera);
                glColor3fv(Col3f::WHITE());
            }
        }
    }

  private:

    Camera* m_view_xyz[4];

    Mesh* m_mesh;
    Mesh* m_mesh_normals;

    Mesh::Preparer m_mesh_prep;
    Mesh::Preparer m_normals_prep;
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

#if !defined NDEBUG && !defined GALE_TINY
    _CrtDumpMemoryLeaks();
#endif

    return 0;
}
