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

        // Create three spectator cameras that look along the negative z-, y-
        // and x-axes.
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
        Mesh* mesh=Mesh::Factory::Dodecahedron();
        m_mesh_prep.compile(*mesh);
        delete mesh;

        mesh=Mesh::Factory::Normals(m_mesh_prep.numVertices(),m_mesh_prep.vertexAccess(),m_mesh_prep.normalAccess(),0.2f);
        m_normals_prep.compile(*mesh);
        delete mesh;

        // Set some OpenGL states.
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
    }

    ~TestWindow() {
        for (int i=1;i<4;++i) {
            delete m_view_xyz[i];
        }
    }

    void onResize(int width,int height) {
        int x[]={width/2,width/2,0,0};
        int y[]={0,height/2,height/2,0};
        int w[]={(width+1)/2,(width+1)/2,width/2-1,width/2-1};
        int h[]={height/2-1,(height+1)/2,(height+1)/2,height/2-1};

        // 0 = lower right
        // 1 = upper right
        // 2 = upper left
        // 3 = lower left
        for (int i=0;i<4;++i) {
            // Set the viewport rectangle.
            m_view_xyz[i]->setScreenSpaceOrigin(x[i],y[i]);
            m_view_xyz[i]->setScreenSpaceDimensions(w[i],h[i]);
            m_view_xyz[i]->setProjection(Mat4d::Factory::PerspectiveProjection(w[i],h[i]));
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

    PreparedMesh m_mesh_prep;
    PreparedMesh m_normals_prep;
};

// Enable memory leak detection, see:
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vsdebug/html/vxcondetectingisolatingmemoryleaks.asp
#if !defined NDEBUG && !defined GALE_TINY_CODE
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
#endif

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
