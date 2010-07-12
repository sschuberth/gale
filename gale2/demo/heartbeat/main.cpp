#include <gale/wrapgl/defaultwindow.h>
#include <gale/wrapgl/renderer.h>
#include <gale/wrapgl/shaderobject.h>

// Enable memory leak detection, see:
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vsdebug/html/vxcondetectingisolatingmemoryleaks.asp
#if !defined NDEBUG && !defined GALE_TINY_CODE
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
#endif

using namespace gale::math;
using namespace gale::model;
using namespace gale::wrapgl;

#include "fake_sss_vert.inl"
#undef SHADER_CODE_H_
#include "fake_sss_frag.inl"
#undef SHADER_CODE_H_

class TestWindow:public DefaultWindow
{
  public:

    TestWindow()
    :   DefaultWindow(_T("demo_heartbeat"),800,600)
    ,   m_vert_shader(GL_VERTEX_SHADER)
    ,   m_frag_shader(GL_FRAGMENT_SHADER)
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

        // Compile and link the shaders.
        GLchar log[4096];

        m_vert_shader.setSource(&shader_fake_sss_vert);
        if (!m_vert_shader.compile() || m_vert_shader.getParameter(GL_INFO_LOG_LENGTH)>0) {
            m_vert_shader.getLog(log,sizeof(log));
            puts(log);
        }

        m_frag_shader.setSource(&shader_fake_sss_frag);
        if (!m_frag_shader.compile() || m_frag_shader.getParameter(GL_INFO_LOG_LENGTH)>0) {
            m_frag_shader.getLog(log,sizeof(log));
            puts(log);
        }

        m_program.attach(m_vert_shader);
        m_program.attach(m_frag_shader);
        if (!m_program.link() || m_program.getParameter(GL_INFO_LOG_LENGTH)>0) {
            m_program.getLog(log,sizeof(log));
            puts(log);
        }

        // We need to use the program before we can set its uniforms.
        m_program.makeCurrent();

        GLint l;

        // Vertex shader uniforms.
        l=glGetUniformLocation(m_program.handle(),"LightPosition");
        glUniform3f(l,0.25f,0.25f,0.5f);
        G_ASSERT_OPENGL

        // Fragment shader uniforms.
        l=glGetUniformLocation(m_program.handle(),"MaterialThickness");
        glUniform1f(l,0.6f);
        G_ASSERT_OPENGL

        l=glGetUniformLocation(m_program.handle(),"ExtinctionCoefficient");
        glUniform3f(l,0.8f,0.12f,0.2f);
        G_ASSERT_OPENGL

        l=glGetUniformLocation(m_program.handle(),"LightColor");
        glUniform4f(l,1.0f,1.0f,1.0f,1.0f);
        G_ASSERT_OPENGL

        l=glGetUniformLocation(m_program.handle(),"BaseColor");
        glUniform4f(l,0.82f,0.76f,0.8f,1.0f);
        G_ASSERT_OPENGL

        l=glGetUniformLocation(m_program.handle(),"SpecColor");
        glUniform4f(l,0.9f,0.9f,1.0f,1.0f);
        G_ASSERT_OPENGL

        l=glGetUniformLocation(m_program.handle(),"SpecPower");
        glUniform1f(l,1.0f);
        G_ASSERT_OPENGL

        l=glGetUniformLocation(m_program.handle(),"RimScalar");
        glUniform1f(l,1.0f);
        G_ASSERT_OPENGL

        // Set some OpenGL states.
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
    }

    void onRender() {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        m_camera.makeCurrent();

        m_program.makeCurrent();
        Renderer::draw(m_heart_prep);
        m_program.setCurrent(0);
    }

  private:

    PreparedMesh m_heart_prep;

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
