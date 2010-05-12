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

// http://prideout.net/blog/?p=22
static char const* s_vert_shader_source=
    "uniform vec3 DiffuseMaterial;\n"
    "\n"
    "varying vec3 EyespaceNormal;\n"
    "varying vec3 Diffuse;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    EyespaceNormal = gl_NormalMatrix * gl_Normal;\n"
    "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
    "    Diffuse = DiffuseMaterial;\n"
    "}\n"
;

// http://prideout.net/blog/?p=22
static char const* s_frag_shader_source=
    "varying vec3 EyespaceNormal;\n"
    "varying vec3 Diffuse;\n"
    "\n"
    "uniform vec3 LightPosition;\n"
    "uniform vec3 AmbientMaterial;\n"
    "uniform vec3 SpecularMaterial;\n"
    "uniform float Shininess;\n"
    "\n"
    "float stepmix(float edge0, float edge1, float E, float x)\n"
    "{\n"
    "    float T = clamp(0.5 * (x - edge0) / E, 0.0, 1.0);\n"
    "    return mix(edge0, edge1, T);\n"
    "}\n"
    "\n"
    "void main()\n"
    "{\n"
    "    vec3 N = normalize(EyespaceNormal);\n"
    "    vec3 L = normalize(LightPosition);\n"
    "    vec3 Eye = vec3(0, 0, 1);\n"
    "    vec3 H = normalize(L + Eye);\n"
    "    \n"
    "    float df = max(0.0, dot(N, L));\n"
    "    float sf = max(0.0, dot(N, H));\n"
    "    sf = pow(sf, Shininess);\n"
    "\n"
    "    const float A = 0.1;\n"
    "    const float B = 0.3;\n"
    "    const float C = 0.6;\n"
    "    const float D = 1.0;\n"
    "    float E = fwidth(df);\n"
    "\n"
    "    if      (df > A - E && df < A + E) df = stepmix(A, B, E, df);\n"
    "    else if (df > B - E && df < B + E) df = stepmix(B, C, E, df);\n"
    "    else if (df > C - E && df < C + E) df = stepmix(C, D, E, df);\n"
    "    else if (df < A) df = 0.0;\n"
    "    else if (df < B) df = B;\n"
    "    else if (df < C) df = C;\n"
    "    else df = D;\n"
    "\n"
    "    E = fwidth(sf);\n"
    "    if (sf > 0.5 - E && sf < 0.5 + E)\n"
    "    {\n"
    "        sf = clamp(0.5 * (sf - 0.5 + E) / E, 0.0, 1.0);\n"
    "    }\n"
    "    else\n"
    "    {\n"
    "        sf = step(0.5, sf);\n"
    "    }\n"
    "\n"
    "    vec3 color = AmbientMaterial + df * Diffuse + sf * SpecularMaterial;\n"
    "    gl_FragColor = vec4(color, 1.0);\n"
    "}\n"
;

class TestWindow:public DefaultWindow
{
  public:

    TestWindow()
    :   DefaultWindow(_T("test_shader"))
    ,   m_vert_shader(GL_VERTEX_SHADER)
    ,   m_frag_shader(GL_FRAGMENT_SHADER)
    {
        m_camera.approach(-10);

        // Set some OpenGL states.
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        // Create a cube mesh and prepare it for rendering.
        m_mesh=Mesh::Factory::TorusKnot(2.5f,0.2f,500,20,3,7);
        m_mesh_prep.compile(*m_mesh);

        GLchar log[4096];

        m_vert_shader.setSource(&s_vert_shader_source);
        if (!m_vert_shader.compile() || m_vert_shader.getParameter(GL_INFO_LOG_LENGTH)>0) {
            m_vert_shader.getLog(log,sizeof(log));
            puts(log);
        }

        m_frag_shader.setSource(&s_frag_shader_source);
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

    ~TestWindow() {
        delete m_mesh;
    }

    void onRender() {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        m_camera.makeCurrent();

        m_program.makeCurrent();
        Renderer::draw(m_mesh_prep);
        m_program.setCurrent(0);
    }

  private:

    Mesh* m_mesh;
    PreparedMesh m_mesh_prep;

    ShaderObject m_vert_shader,m_frag_shader;
    ProgramObject m_program;
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
