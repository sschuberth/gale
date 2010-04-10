#include <gale/math/color.h>
#include <gale/wrapgl/defaultwindow.h>
#include <gale/wrapgl/renderer.h>

#ifndef GALE_TINY_CODE
    #include <stdio.h>
#else
    #define printf print_str
#endif

using namespace gale::math;
using namespace gale::model;
using namespace gale::wrapgl;

static char const* base_names[]={
    "Tetrahedron"
,   "Octahedron"
,   "Hexahedron"
,   "Icosahedron"
,   "Dodecahedron"
,   "Moebius Strip"
,   "Spherical Supershape"
,   "Toroidal Supershape"
,   "Shell"
};

static char const* mode_names[]={
    "Polyhedral"
,   "Butterfly"
,   "Loop"
,   "Sqrt3"
,   "Catmull-Clark"
,   "Doo-Sabin"
};

class TestWindow:public DefaultWindow
{
  public:

    TestWindow()
    :   DefaultWindow(_T("test_mesh"))
    ,   m_base(0)
    ,   m_mode(0)
    ,   m_step(0)
    ,   m_show_box(false)
    ,   m_show_normals(false)
    ,   m_lighting(false)
    ,   m_culling(true)
    {
        m_camera.approach(-7);

        // The cast is required as "new MeshTable" seems to return "MeshCache (*)[6][5]".
        m_meshes=reinterpret_cast<MeshTable*>(new MeshTable);

        // Only initialize the base meshes for the first subdivision mode, as
        // they are the same for all modes.
        (*m_meshes)[0][0][0].init(Mesh::Factory::Tetrahedron());
        (*m_meshes)[1][0][0].init(Mesh::Factory::Octahedron());
        (*m_meshes)[2][0][0].init(Mesh::Factory::Hexahedron());
        (*m_meshes)[3][0][0].init(Mesh::Factory::Icosahedron());
        (*m_meshes)[4][0][0].init(Mesh::Factory::Dodecahedron());

        (*m_meshes)[5][0][0].init(Mesh::Factory::MoebiusStrip(1.2f,1.0f,0.4f,0.1f,20,10));

        SuperFormula sp_r1(7.0f,0.2f,1.7f,1.7f);
        SuperFormula sp_r2(7.0f,0.2f,1.7f,1.7f);
        (*m_meshes)[6][0][0].init(Mesh::Factory::SphericalProduct(sp_r1,60,sp_r2,30));

        SuperFormula tp_r1(6.0f,-0.68f,22.77f,0.75f,1.99f,0.64f);
        SuperFormula tp_r2(3.0f,15.24f,0.35f,49.59f,1.05f,0.28f);
        (*m_meshes)[7][0][0].init(Mesh::Factory::ToroidalProduct(tp_r1,40,tp_r2,20));

        (*m_meshes)[8][0][0].init(Mesh::Factory::Shell(20,80,0.4f,0.2f,2.5f,3));

#ifndef GALE_TINY_CODE
        for (int b=0;b<8;++b) {
            int error=(*m_meshes)[b][0][0].mesh->check();
            if (error>=0) {
                printf("DEBUG: Mesh is inconsistent at vertex %d.\n",error);
            }

            printf("DEBUG: Bounding box size is %f x %f.\n"
            ,   (*m_meshes)[b][0][0].mesh_prep.box.getWidth()
            ,   (*m_meshes)[b][0][0].mesh_prep.box.getHeight()
            );
        }
#endif

        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_LIGHT0);
    }

    ~TestWindow() {
        delete [] m_meshes;
    }

    void onRender() {
        // We store the base meshes only once, not for every subdivision mode.
        int mode=(m_step==0)?0:m_mode;
        MeshCache const& mc=(*m_meshes)[m_base][mode][m_step];

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        if (m_culling) {
            glEnable(GL_CULL_FACE);
        }
        else {
            glDisable(GL_CULL_FACE);
        }

        m_camera.makeCurrent();
        glLightfv(GL_LIGHT0,GL_POSITION,m_camera.getPosition());

        if (m_camera.frustum().contains(mc.mesh_prep.box) || !m_culling) {
            if (m_lighting) {
                glEnable(GL_LIGHTING);
            }
            else {
                glDisable(GL_LIGHTING);
            }
            glColor3fv(Col3f::WHITE());
            Renderer::draw(mc.mesh_prep);

            if (m_show_box) {
                glDisable(GL_LIGHTING);
                static Col3f const orange=Col3f::RED()&Col3f::YELLOW();
                glColor3fv(orange);
                Renderer::draw(mc.mesh_prep.box);
            }
        }
        else {
            puts("STATUS: Mesh out of frustum.");
        }

        if (m_show_normals) {
            glDisable(GL_LIGHTING);
            glColor3fv(Col3f::RED());
            if (m_camera.frustum().contains(mc.normals_prep.box) || !m_culling) {
                Renderer::draw(mc.normals_prep);
            }
            else {
                puts("STATUS: Normals out of frustum.");
            }
        }
    }

    void onKeyEvent(char key) {
        int new_mode=0;

        static Mesh::Subdivider::Scheme const scheme_ptrs[]={
            Mesh::Subdivider::Polyhedral
        ,   Mesh::Subdivider::Butterfly
        ,   Mesh::Subdivider::Loop
        ,   Mesh::Subdivider::Sqrt3
        ,   Mesh::Subdivider::CatmullClark
        ,   Mesh::Subdivider::DooSabin
        };

        switch (key) {
            // Base meshes.
            case '1': // Tetrahedron
            case '2': // Octahedron
            case '4': // Icosahedron
            case '6': // Moebius Strip
            case '7': // Spherical Supershape
            case '8': // Toroidal Supershape
            case '9': // Shell
            {
                int new_base=key-'0'-1;
                if (m_mode==4) {
                    printf("ERROR: ");
                    printf(mode_names[m_mode]);
                    printf(" not supported on ");
                    printf(base_names[new_base]);
                    printf(", resetting to ");
                    m_mode=0;
                    printf(mode_names[m_mode]);
                    puts(".");
                    m_step=0;
                }

                m_base=new_base;

                break;
            }
            case '3': // Hexahedron
            {
                int new_base=key-'0'-1;
                if (m_mode<4) {
                    printf("ERROR: ");
                    printf(mode_names[m_mode]);
                    printf(" not supported on ");
                    printf(base_names[new_base]);
                    printf(", resetting to ");
                    m_mode=4;
                    printf(mode_names[m_mode]);
                    puts(".");
                    m_step=0;
                }

                m_base=new_base;

                break;
            }
            case '5': // Dodecahedron
            {
                int new_base=key-'0'-1;
                if (m_mode!=5) {
                    printf("ERROR: ");
                    printf(mode_names[m_mode]);
                    printf(" not supported on ");
                    printf(base_names[new_base]);
                    printf(", resetting to ");
                    m_mode=5;
                    printf(mode_names[m_mode]);
                    puts(".");
                    m_step=0;
                }

                m_base=new_base;

                break;
            }

            // Subdivision modes.
            case 's':
                ++new_mode;
            case 'l':
                ++new_mode;
            case 'b':
                ++new_mode;
            case 'p':
            {
                // Check if the base mesh only consists of supported primitives.
                MeshCache const& mc=(*m_meshes)[m_base][0][0];
                if (mc.mesh_prep.hasPoints() || mc.mesh_prep.hasLines() || mc.mesh_prep.hasQuads() || mc.mesh_prep.hasPolygons()) {
                    printf("ERROR: ");
                    printf(base_names[m_base]);
                    printf(" mesh cannot be subdivided using ");
                    printf(mode_names[new_mode]);
                    puts(".");
                }
                else {
                    m_mode=new_mode;
                }
                break;
            }
            case 'c':
            {
                new_mode=4;

                // Check if the base mesh only consists of supported primitives.
                MeshCache const& mc=(*m_meshes)[m_base][0][0];
                if (mc.mesh_prep.hasPoints() || mc.mesh_prep.hasLines() || mc.mesh_prep.hasTriangles() || mc.mesh_prep.hasPolygons()) {
                    printf("ERROR: ");
                    printf(base_names[m_base]);
                    printf(" mesh cannot be subdivided using ");
                    printf(mode_names[new_mode]);
                    puts(".");
                }
                else {
                    m_mode=new_mode;
                }
                break;
            }
            case 'd':
            {
                new_mode=5;

                // Doo-Sabin subdivision supports all primitives.
                m_mode=new_mode;
                break;
            }

            // Subdivision steps.
            case '+': {
                m_step+=(m_step<5);
                break;
            }
            case '-': {
                m_step-=(m_step>0);
                break;
            }

            // Display toggles.
            case 'x': {
                m_show_box=!m_show_box;
                break;
            }
            case 'n': {
                m_show_normals=!m_show_normals;
                break;
            }
            case 'r': {
                m_lighting=!m_lighting;
                break;
            }
            case 'f': {
                m_culling=!m_culling;
                break;
            }

            default: {
                DefaultWindow::onKeyEvent(key);

                // No further processing or repaint action.
                return;
            }
        }

        for (int s=1;s<=m_step;++s) {
            MeshCache& mc=(*m_meshes)[m_base][m_mode][s];

            if (mc.mesh!=NULL) {
                // Skip if the mesh for this subdivision step already exists.
                continue;
            }

            // Perform a deep-copy of the previous mesh, and subdivide the copy.
            int mode=(s==1)?0:m_mode;
            mc.mesh=new Mesh(*(*m_meshes)[m_base][mode][s-1].mesh);
            scheme_ptrs[m_mode](*mc.mesh,1);
            mc.init(mc.mesh);

#ifndef GALE_TINY_CODE
            int error=mc.mesh->check();
            if (error>=0) {
                printf("DEBUG: Mesh is inconsistent at vertex %d.\n",error);
            }

            printf("DEBUG: Bounding box size is %f x %f.\n"
            ,   mc.mesh_prep.box.getWidth()
            ,   mc.mesh_prep.box.getHeight()
            );
#endif
        }

        printf("STATUS: ");
        printf(base_names[m_base]);
        printf(" mesh, ");
        printf(mode_names[m_mode]);
        printf(" subdivision, ");
        static char buffer[]="step 0.";
        buffer[5]='0'+m_step;
        puts(buffer);

        repaint();
    }

  private:

    struct MeshCache {
        MeshCache()
        :   mesh(NULL)
        ,   normals(NULL) {}

        ~MeshCache() {
            delete mesh;
            delete normals;
        }

        void init(Mesh* m,float s=0.2f) {
            mesh=m;
            mesh_prep.compile(m);

            normals=Mesh::Factory::Normals(mesh_prep,s);
            normals_prep.compile(normals);
        }

        Mesh* mesh;
        Mesh::Preparer mesh_prep;

        Mesh* normals;
        Mesh::Preparer normals_prep;
    };

    // Array of base meshes, subdivision modes, and subdivision steps.
    typedef MeshCache MeshTable[G_ARRAY_LENGTH(base_names)][G_ARRAY_LENGTH(mode_names)][5];

    MeshTable* m_meshes;
    int m_base,m_mode,m_step;

    bool m_show_box,m_show_normals,m_lighting,m_culling;
};

// Enable memory leak detection, see:
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vsdebug/html/vxcondetectingisolatingmemoryleaks.asp
#if !defined NDEBUG && !defined GALE_TINY_CODE
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
#endif

int main()
{
#if !defined NDEBUG && !defined GALE_TINY_CODE
    puts("Base meshes");
    puts("-----------");
    for (int b=0;b<8;++b) {
        printf("%d: %s\n",b+1,base_names[b]);
    }
    puts("\n");
    puts("Sub-division modes");
    puts("------------------");
    puts("p: Polyhedral");
    puts("b: Butterfly");
    puts("l: Loop");
    puts("s: Sqrt3");
    puts("c: Catmull-Clark");
    puts("d: Doo-Sabin");
    puts("\n");
    puts("Sub-division steps");
    puts("------------------");
    puts("+: Increase");
    puts("-: Decrease");
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
