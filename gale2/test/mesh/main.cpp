#include <gale/wrapgl/defaultwindow.h>

#include <gale/math/color.h>
#include <gale/model/mesh.h>

#include <stdio.h>

using namespace gale::math;
using namespace gale::model;
using namespace gale::wrapgl;

class TestWindow:public DefaultWindow
{
  public:

    TestWindow()
    :   DefaultWindow(_T("test_mesh"))
    ,   m_mesh(Mesh::Factory::Tetrahedron())
    ,   m_scheme(Mesh::Subdivider::Polyhedral)
    ,   m_step(0)
    {
        m_camera.approach(5);

        int error=m_mesh->isConsistent();
        if (error>=0) {
            printf("Mesh is inconsistent at vertex %d.\n",error);
        }

        m_renderer.compile(m_mesh);

        m_mesh_normals=Mesh::Factory::Normals(m_renderer,0.2f);
        m_renderer_normals.compile(m_mesh_normals);

        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_LIGHT0);
    }

    ~TestWindow() {
        delete m_mesh;
        delete m_mesh_normals;
    }

    void onRender() {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        m_camera.apply();
        glLightfv(GL_LIGHT0,GL_POSITION,m_camera.getPosition());

        glEnable(GL_LIGHTING);
        glColor3fv(Col3f::WHITE());
        m_renderer.render();

        glDisable(GL_LIGHTING);
        glColor3fv(Col3f::RED());
        m_renderer_normals.render();
    }

    void onKeyEvent(char key) {
        static char last_mesh_key='1';
        static char last_scheme_key='p';

        if (key=='+' && m_step<5) {
            ++m_step;
            key=last_scheme_key;
        }
        else if (key=='-' && m_step>0) {
            --m_step;
            key=last_scheme_key;
        }

        switch (key) {
            case 'p': {
                last_scheme_key=key;
                printf("Polyhedral subdivision");
                if (last_mesh_key!='1' && last_mesh_key!='2' && last_mesh_key!='4') {
                    printf(" not supported on this mesh");
                }
                else {
                    m_scheme=Mesh::Subdivider::Polyhedral;
                    key=last_mesh_key;
                }
                printf(" (step %d).\n",m_step);
                break;
            }
            case 'b': {
                last_scheme_key=key;
                printf("Butterfly subdivision");
                if (last_mesh_key!='1' && last_mesh_key!='2' && last_mesh_key!='4') {
                    printf(" not supported on this mesh");
                }
                else {
                    m_scheme=Mesh::Subdivider::Butterfly;
                    key=last_mesh_key;
                }
                printf(" (step %d).\n",m_step);
                break;
            }
            case 'l': {
                last_scheme_key=key;
                printf("Loop subdivision");
                if (last_mesh_key!='1' && last_mesh_key!='2' && last_mesh_key!='4') {
                    printf(" not supported on this mesh");
                }
                else {
                    m_scheme=Mesh::Subdivider::Loop;
                    key=last_mesh_key;
                }
                printf(" (step %d).\n",m_step);
                break;
            }
            case 's': {
                last_scheme_key=key;
                printf("Sqrt3 subdivision");
                if (last_mesh_key!='1' && last_mesh_key!='2' && last_mesh_key!='4') {
                    printf(" not supported on this mesh");
                }
                else {
                    m_scheme=Mesh::Subdivider::Sqrt3;
                    key=last_mesh_key;
                }
                printf(" (step %d).\n",m_step);
                break;
            }
            case 'c': {
                last_scheme_key=key;
                printf("Catmull-Clark subdivision");
                if (last_mesh_key!='3') {
                    printf(" not supported on this mesh");
                }
                else {
                    m_scheme=Mesh::Subdivider::CatmullClark;
                    key=last_mesh_key;
                }
                printf(" (step %d).\n",m_step);
                break;
            }
            case 'd': {
                last_scheme_key=key;
                printf("Doo-Sabin subdivision (step %d).\n",m_step);
                m_scheme=Mesh::Subdivider::DooSabin;
                key=last_mesh_key;
                break;
            }
        }

        switch (key) {
            case '1': {
                delete m_mesh;
                m_mesh=Mesh::Factory::Tetrahedron();
                puts("Tetrahedron mesh.");
                if (m_step>0 && last_scheme_key=='c') {
                    puts("Current subdivision scheme not supported on this mesh, resetting.");
                    m_step=0;
                }
                break;
            }
            case '2': {
                delete m_mesh;
                m_mesh=Mesh::Factory::Octahedron();
                puts("Octahedron mesh.");
                if (m_step>0 && last_scheme_key=='c') {
                    puts("Current subdivision scheme not supported on this mesh, resetting.");
                    m_step=0;
                }
                break;
            }
            case '3': {
                delete m_mesh;
                m_mesh=Mesh::Factory::Hexahedron();
                puts("Hexahedron mesh.");
                if (m_step>0 && (last_scheme_key=='p' || last_scheme_key=='b' || last_scheme_key=='l' || last_scheme_key=='s')) {
                    puts("Current subdivision scheme not supported on this mesh, resetting.");
                    m_step=0;
                }
                break;
            }
            case '4': {
                delete m_mesh;
                m_mesh=Mesh::Factory::Icosahedron();
                puts("Icosahedron mesh.");
                if (m_step>0 && last_scheme_key=='c') {
                    puts("Current subdivision scheme not supported on this mesh, resetting.");
                    m_step=0;
                }
                break;
            }
            case '5': {
                delete m_mesh;
                m_mesh=Mesh::Factory::Dodecahedron();
                puts("Dodecahedron mesh.");
                if (m_step>0 && (last_scheme_key=='p' || last_scheme_key=='b' || last_scheme_key=='l' || last_scheme_key=='s' || last_scheme_key=='c')) {
                    puts("Current subdivision scheme not supported on this mesh, resetting.");
                    m_step=0;
                }
                break;
            }
        }

        if (key>='1' && key<='5') {
            m_scheme(*m_mesh,m_step);
            int error=m_mesh->isConsistent();
            if (error>=0) {
                printf("Mesh is inconsistent at vertex %d.\n",error);
            }
            m_renderer.compile(m_mesh);

            delete m_mesh_normals;
            m_mesh_normals=Mesh::Factory::Normals(m_renderer,0.2f);
            m_renderer_normals.compile(m_mesh_normals);

            repaint();

            last_mesh_key=key;
        }
        else {
            DefaultWindow::onKeyEvent(key);
        }
    }

    void onMouseEvent(int x,int y,int wheel,int event) {
        UNREFERENCED_PARAMETER(wheel);

        static int mouse_prev_x=0,mouse_prev_y=0;

        // This is positive if the mouse cursor was moved to the right.
        int mouse_diff_x=x-mouse_prev_x;

        // This is positive if the mouse cursor was moved downwards.
        int mouse_diff_y=y-mouse_prev_y;

        if (event!=ME_BUTTON_NONE) {
            if (event&ME_BUTTON_LEFT) {
                HMat4f m=m_camera.getModelview();
                m=HMat4f::Factory::Rotation(m.getUpVector(),-mouse_diff_x*Constd::DEG_TO_RAD()*0.5)*m;
                m=HMat4f::Factory::Rotation(m.getRightVector(),-mouse_diff_y*Constd::DEG_TO_RAD()*0.5)*m;
                m_camera.setModelview(m);
            }

            if (event&ME_BUTTON_MIDDLE) {
                m_camera.strafe(static_cast<float>(-mouse_diff_x)/100);
                m_camera.elevate(static_cast<float>(mouse_diff_y)/100);
            }

            if (event&ME_BUTTON_RIGHT) {
                m_camera.approach(mouse_diff_y*Constf::DEG_TO_RAD());
            }

            repaint();
        }

        mouse_prev_x=x;
        mouse_prev_y=y;
    }

  private:

    Mesh* m_mesh;
    Mesh* m_mesh_normals;

    Mesh::Subdivider::Scheme m_scheme;
    int m_step;

    Mesh::Renderer m_renderer;
    Mesh::Renderer m_renderer_normals;
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