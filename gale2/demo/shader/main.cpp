// Enable memory leak detection, see:
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vsdebug/html/vxcondetectingisolatingmemoryleaks.asp
#if !defined NDEBUG && !defined GALE_TINY_CODE
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
#endif

#include <gale/wrapgl/defaultwindow.h>
#include <gale/wrapgl/renderer.h>
#include <gale/wrapgl/shaderobject.h>

#ifndef GALE_TINY_CODE
    #include <stdio.h>
#else
    #define printf print_str
#endif

using namespace gale::math;
using namespace gale::model;
using namespace gale::wrapgl;

#include "aa_cel_vert.inl"
#include "aa_cel_frag.inl"

class TestWindow:public DefaultWindow
{
  public:

    TestWindow()
    :   DefaultWindow(_T("demo_shader"),500,500)
    ,   m_vert_shader(GL_VERTEX_SHADER)
    ,   m_frag_shader(GL_FRAGMENT_SHADER)
    {
        m_camera.approach(-10);

        // Set some OpenGL states.
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        // Create a cube mesh and prepare it for rendering.
        Mesh* mesh=Mesh::Factory::TorusKnot(2.5f,0.2f,500,20,3,7);
        m_mesh_prep.compile(*mesh);
        delete mesh;

        GLchar log[4096];

        m_vert_shader.setSource(&aa_cel_vert_glsl);
        if (!m_vert_shader.compile() || m_vert_shader.hasLog()) {
            m_vert_shader.getLog(log,sizeof(log));
            puts(log);
        }

        m_frag_shader.setSource(&aa_cel_frag_glsl);
        if (!m_frag_shader.compile() || m_frag_shader.hasLog()) {
            m_frag_shader.getLog(log,sizeof(log));
            puts(log);
        }

        m_program.attach(m_vert_shader);
        m_program.attach(m_frag_shader);
        if (!m_program.link() || m_program.hasLog()) {
            m_program.getLog(log,sizeof(log));
            puts(log);
        }

        // We need to use the program before we can set its uniforms.
        m_program.bind();

        GLint l;

        // Vertex shader uniforms.
        l=glGetUniformLocation(m_program.handle(),"DiffuseMaterial");
        glUniform3f(l,0.0f,0.75f,0.75f);
        G_ASSERT_OPENGL

        // Fragment shader uniforms.
        l=glGetUniformLocation(m_program.handle(),"LightPosition");
        glUniform3f(l,0.25f,0.25f,1.0f);
        G_ASSERT_OPENGL

        l=glGetUniformLocation(m_program.handle(),"AmbientMaterial");
        glUniform3f(l,0.04f,0.04f,0.04f);
        G_ASSERT_OPENGL

        l=glGetUniformLocation(m_program.handle(),"SpecularMaterial");
        glUniform3f(l,0.5f,0.5f,0.5f);
        G_ASSERT_OPENGL

        l=glGetUniformLocation(m_program.handle(),"Shininess");
        glUniform1f(l,50.0f);
        G_ASSERT_OPENGL
    }

    void onRender() {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        m_camera.apply();

        m_program.bind();
        Renderer::draw(m_mesh_prep);
        m_program.release();
    }

  private:

    PreparedMesh m_mesh_prep;

    ShaderObject m_vert_shader,m_frag_shader;
    ProgramObject m_program;
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
