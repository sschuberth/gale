#include <gale/math/color.h>
#include <gale/wrapgl/defaultwindow.h>
#include <gale/wrapgl/renderer.h>

#ifndef GALE_TINY_CODE
    #include <stdio.h>
#else
    #define printf print
#endif

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
    ,   m_show_box(true)
    ,   m_show_normals(true)
    ,   m_lighting(true)
    ,   m_culling(true)
    {
        m_camera.approach(-5);

#ifndef GALE_TINY_CODE
        int error=m_mesh->check();
        if (error>=0) {
            printf("Mesh is inconsistent at vertex %d.\n",error);
        }
#endif
        m_mesh_prep.compile(m_mesh);
#ifndef GALE_TINY_CODE
        printf("Bounding box size is %f x %f.\n",m_mesh_prep.box.getWidth(),m_mesh_prep.box.getHeight());
#endif

        m_mesh_normals=Mesh::Factory::Normals(m_mesh_prep,0.2f);
        m_normals_prep.compile(m_mesh_normals);

        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_LIGHT0);
    }

    ~TestWindow() {
        delete m_mesh;
        delete m_mesh_normals;
    }

    void onRender() {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        if (m_culling) {
            glEnable(GL_CULL_FACE);
        }
        else {
            glDisable(GL_CULL_FACE);
        }

        m_camera.makeCurrent();
        glLightfv(GL_LIGHT0,GL_POSITION,m_camera.getPosition());

        if (m_camera.frustum().contains(m_mesh_prep.box) || !m_culling) {
            if (m_lighting) {
                glEnable(GL_LIGHTING);
            }
            else {
                glDisable(GL_LIGHTING);
            }
            glColor3fv(Col3f::WHITE());
            Renderer::draw(m_mesh_prep);

            if (m_show_box) {
                glDisable(GL_LIGHTING);
                static Col3f const orange=Col3f::RED()&Col3f::YELLOW();
                glColor3fv(orange);
                Renderer::draw(m_mesh_prep.box);
            }
        }
        else {
            puts("Mesh out of frustum.");
        }

        if (m_show_normals) {
            glDisable(GL_LIGHTING);
            glColor3fv(Col3f::RED());
            if (m_camera.frustum().contains(m_normals_prep.box) || !m_culling) {
                Renderer::draw(m_normals_prep);
            }
            else {
                puts("Normals out of frustum.");
            }
        }
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

        static char buffer[]=" (step 0).\n";
        switch (key) {
            case 'p': {
                last_scheme_key=key;
                printf("Polyhedral subdivision");
                if (last_mesh_key!='1' && last_mesh_key!='2' && last_mesh_key!='4' && last_mesh_key<6) {
                    printf(" not supported on this mesh");
                }
                else {
                    m_scheme=Mesh::Subdivider::Polyhedral;
                    key=last_mesh_key;
                }
                buffer[7]='0'+m_step;
                printf(buffer);
                break;
            }
            case 'b': {
                last_scheme_key=key;
                printf("Butterfly subdivision");
                if (last_mesh_key!='1' && last_mesh_key!='2' && last_mesh_key!='4' && last_mesh_key<6) {
                    printf(" not supported on this mesh");
                }
                else {
                    m_scheme=Mesh::Subdivider::Butterfly;
                    key=last_mesh_key;
                }
                buffer[7]='0'+m_step;
                printf(buffer);
                break;
            }
            case 'l': {
                last_scheme_key=key;
                printf("Loop subdivision");
                if (last_mesh_key!='1' && last_mesh_key!='2' && last_mesh_key!='4' && last_mesh_key<6) {
                    printf(" not supported on this mesh");
                }
                else {
                    m_scheme=Mesh::Subdivider::Loop;
                    key=last_mesh_key;
                }
                buffer[7]='0'+m_step;
                printf(buffer);
                break;
            }
            case 's': {
                last_scheme_key=key;
                printf("Sqrt3 subdivision");
                if (last_mesh_key!='1' && last_mesh_key!='2' && last_mesh_key!='4' && last_mesh_key<6) {
                    printf(" not supported on this mesh");
                }
                else {
                    m_scheme=Mesh::Subdivider::Sqrt3;
                    key=last_mesh_key;
                }
                buffer[7]='0'+m_step;
                printf(buffer);
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
                buffer[7]='0'+m_step;
                printf(buffer);
                break;
            }
            case 'd': {
                last_scheme_key=key;
                printf("Doo-Sabin subdivision");
                buffer[7]='0'+m_step;
                printf(buffer);
                m_scheme=Mesh::Subdivider::DooSabin;
                key=last_mesh_key;
                break;
            }

            case 'x': {
                m_show_box=!m_show_box;
                repaint();
                break;
            }
            case 'n': {
                m_show_normals=!m_show_normals;
                repaint();
                break;
            }
            case 'r': {
                m_lighting=!m_lighting;
                repaint();
                break;
            }
            case 'f': {
                m_culling=!m_culling;
                repaint();
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

            case '6': {
                delete m_mesh;
                m_mesh=Mesh::Factory::MoebiusStrip(1.2f,1.0f,0.4f,0.1f,20,10);
                puts("Moebius Strip mesh.");
                if (m_step>0 && last_scheme_key=='c') {
                    puts("Current subdivision scheme not supported on this mesh, resetting.");
                    m_step=0;
                }
                break;
            }

            case '7': {
                delete m_mesh;

                static Superformula r1(7.0f,0.2f,1.7f,1.7f);
                static Superformula r2(7.0f,0.2f,1.7f,1.7f);
                m_mesh=Mesh::Factory::SphericalProduct(r1,60,r2,30);

                puts("Spherical Supershape mesh.");
                if (m_step>0 && last_scheme_key=='c') {
                    puts("Current subdivision scheme not supported on this mesh, resetting.");
                    m_step=0;
                }
                break;
            }
            case '8': {
                delete m_mesh;

                static Superformula r1(6.0f,-0.68f,22.77f,0.75f,1.99f,0.64f);
                static Superformula r2(3.0f,15.24f,0.35f,49.59f,1.05f,0.28f);
                m_mesh=Mesh::Factory::ToroidalProduct(r1,40,r2,20);

                puts("Toroidal Supershape mesh.");
                if (m_step>0 && last_scheme_key=='c') {
                    puts("Current subdivision scheme not supported on this mesh, resetting.");
                    m_step=0;
                }
                break;
            }
        }

        if (key>='1' && key<='8') {
            m_scheme(*m_mesh,m_step);
#ifndef GALE_TINY_CODE
            int error=m_mesh->check();
            if (error>=0) {
                printf("Mesh is inconsistent at vertex %d.\n",error);
            }
#endif
            m_mesh_prep.compile(m_mesh);
#ifndef GALE_TINY_CODE
            printf("Bounding box size is %f x %f.\n",m_mesh_prep.box.getWidth(),m_mesh_prep.box.getHeight());
#endif

            delete m_mesh_normals;
            m_mesh_normals=Mesh::Factory::Normals(m_mesh_prep,0.2f);
            m_normals_prep.compile(m_mesh_normals);

            repaint();

            last_mesh_key=key;
        }
        else {
            DefaultWindow::onKeyEvent(key);
        }
    }

  private:

    Mesh* m_mesh;
    Mesh* m_mesh_normals;

    Mesh::Subdivider::Scheme m_scheme;
    int m_step;

    Mesh::Preparer m_mesh_prep;
    Mesh::Preparer m_normals_prep;

    bool m_show_box,m_show_normals,m_lighting,m_culling;
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
#ifndef NDEBUG
    puts("Base mesh selection");
    puts("-------------------");
    puts("1: Tetrahedron");
    puts("2: Octahedron");
    puts("3: Hexahedron");
    puts("4: Icosahedron");
    puts("5: Dodecahedron");
    puts("6: Moebius Strip");
    puts("7: Spherical Supershape");
    puts("8: Toroidal Supershape");
    puts("\n");
    puts("Sub-division mode");
    puts("-----------------");
    puts("p: Polyhedral");
    puts("b: Butterfly");
    puts("l: Loop");
    puts("s: Sqrt3");
    puts("c: Catmull-Clark");
    puts("d: Doo-Sabin");
    puts("\n");
    puts("Display toggles");
    puts("---------------");
    puts("x: Bounding box");
    puts("n: Vertex normals");
    puts("r: Lighting");
    puts("f: Face / frustum culling");
    puts("\n");
#endif

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
