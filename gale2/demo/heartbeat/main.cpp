#include <gale/math/interpolator.h>
#include <gale/wrapgl/defaultwindow.h>
#include <gale/wrapgl/renderer.h>
#include <gale/wrapgl/shaderobject.h>

// Enable memory leak detection, see:
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vsdebug/html/vxcondetectingisolatingmemoryleaks.asp
#if !defined NDEBUG && !defined GALE_TINY_CODE
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
#endif

using namespace gale::global;
using namespace gale::math;
using namespace gale::model;
using namespace gale::wrapgl;

#include "fake_sss_vert.inl"
#undef SHADER_CODE_H_
#include "fake_sss_frag.inl"
#undef SHADER_CODE_H_

#include "bg_frag.inl"
#undef SHADER_CODE_H_

// Modeled using <http://threekings.tk/mirror/Spline/MainForm.html>.
static Vec2f const ECG[]={
    Vec2f( -195,    0)
,   Vec2f( -179,    0)
,   Vec2f( -155,    1)
,   Vec2f( -133,    2)
,   Vec2f(  -95,  -10)
,   Vec2f(  -63,   28)
,   Vec2f(  -43,    0)
,   Vec2f(  -30,    6)
,   Vec2f(  -23,  -18)
,   Vec2f(   -1,  142)
,   Vec2f(    9,  -29)
,   Vec2f(   17,   -1)
,   Vec2f(   25,    0)
,   Vec2f(   35,    0)
,   Vec2f(   47,    0)
,   Vec2f(   60,    0)
,   Vec2f(   76,    3)
,   Vec2f(   91,   18)
,   Vec2f(  109,   31)
,   Vec2f(  133,   35)
,   Vec2f(  153,   31)
,   Vec2f(  177,   18)
,   Vec2f(  195,    2)
,   Vec2f(  204,    0)
};

class DemoWindow:public DefaultWindow
{
  public:

    DemoWindow()
    :   DefaultWindow(_T("demo_heartbeat"),800,600)
    ,   m_heart_vert(GL_VERTEX_SHADER)
    ,   m_heart_frag(GL_FRAGMENT_SHADER)
    ,   m_bg_frag(GL_FRAGMENT_SHADER)
    ,   m_points(ECG)
    {
        m_camera.rotate(Vec3f::Y(),-10*Constf::DEG_TO_RAD());
        m_camera.approach(-4.0f);
        m_camera.elevate(-0.3f);

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

        m_heart_vert.setSource(&shader_fake_sss_vert);
        if (!m_heart_vert.compile() || m_heart_vert.getParameter(GL_INFO_LOG_LENGTH)>0) {
            m_heart_vert.getLog(log,sizeof(log));
            puts(log);
        }

        m_heart_frag.setSource(&shader_fake_sss_frag);
        if (!m_heart_frag.compile() || m_heart_frag.getParameter(GL_INFO_LOG_LENGTH)>0) {
            m_heart_frag.getLog(log,sizeof(log));
            puts(log);
        }

        m_heart_prog.attach(m_heart_vert);
        m_heart_prog.attach(m_heart_frag);
        if (!m_heart_prog.link() || m_heart_prog.getParameter(GL_INFO_LOG_LENGTH)>0) {
            m_heart_prog.getLog(log,sizeof(log));
            puts(log);
        }

        m_bg_frag.setSource(&shader_bg_frag);
        if (!m_bg_frag.compile() || m_bg_frag.getParameter(GL_INFO_LOG_LENGTH)>0) {
            m_bg_frag.getLog(log,sizeof(log));
            puts(log);
        }

        m_bg_prog.attach(m_bg_frag);
        if (!m_bg_prog.link() || m_bg_prog.getParameter(GL_INFO_LOG_LENGTH)>0) {
            m_bg_prog.getLog(log,sizeof(log));
            puts(log);
        }

        // We need to bind the program before we can set its uniforms.
        GLint l;

        m_heart_prog.bind();

        // Vertex shader uniforms.
        l=glGetUniformLocation(m_heart_prog.handle(),"LightPosition");
        glUniform3f(l,0.25f,0.25f,0.5f);
        G_ASSERT_OPENGL

        // Fragment shader uniforms.
        l=glGetUniformLocation(m_heart_prog.handle(),"MaterialThickness");
        glUniform1f(l,0.6f);
        G_ASSERT_OPENGL

        l=glGetUniformLocation(m_heart_prog.handle(),"ExtinctionCoefficient");
        glUniform3f(l,0.8f,0.12f,0.2f);
        G_ASSERT_OPENGL

        l=glGetUniformLocation(m_heart_prog.handle(),"LightColor");
        glUniform4f(l,1.0f,1.0f,1.0f,1.0f);
        G_ASSERT_OPENGL

        l=glGetUniformLocation(m_heart_prog.handle(),"BaseColor");
        glUniform4f(l,0.82f,0.76f,0.8f,1.0f);
        G_ASSERT_OPENGL

        l=glGetUniformLocation(m_heart_prog.handle(),"SpecColor");
        glUniform4f(l,0.9f,0.9f,1.0f,1.0f);
        G_ASSERT_OPENGL

        l=glGetUniformLocation(m_heart_prog.handle(),"SpecPower");
        glUniform1f(l,1.0f);
        G_ASSERT_OPENGL

        l=glGetUniformLocation(m_heart_prog.handle(),"RimScalar");
        glUniform1f(l,1.0f);
        G_ASSERT_OPENGL

        m_bg_prog.bind();

        // Fragment shader uniforms.
        l=glGetUniformLocation(m_bg_prog.handle(),"sections");
        glUniform1i(l,50);
        G_ASSERT_OPENGL

        l=glGetUniformLocation(m_bg_prog.handle(),"color0");
        glUniform4f(l,0.88f,0.88f,0.88f,1.0f);
        G_ASSERT_OPENGL

        l=glGetUniformLocation(m_bg_prog.handle(),"color1");
        glUniform4f(l,0.9f,0.9f,0.9f,1.0f);
        G_ASSERT_OPENGL

        // Set some OpenGL states.
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
    }

    void onRender() {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        m_camera.apply();

        // Draw a background.
        Helper::pushOrtho2D();

        m_bg_prog.bind();

        GLint l=glGetUniformLocation(m_bg_prog.handle(),"viewport");
        glUniform2i(l,m_camera.getScreenSpace().width,m_camera.getScreenSpace().height);
        G_ASSERT_OPENGL

        glDepthMask(GL_FALSE);
        glRecti(0,0,m_camera.getScreenSpace().width,m_camera.getScreenSpace().height);
        glDepthMask(GL_TRUE);

        Helper::popOrtho2D();

        // Draw the heart.
        m_heart_prog.bind();
        Renderer::draw(m_heart_prep);
        m_heart_prog.release();

        Helper::pushOrtho2D();

        static int const SAMPLES=500;

        float px=m_camera.getScreenSpace().width/2;
        float py=m_camera.getScreenSpace().height/4;

        glBegin(GL_LINE_STRIP);
            glVertex2f(0,py);

        for (int i=0;i<=SAMPLES;++i) {
            Vec2f p=Interpolator::CatmullRom(m_points,static_cast<float>(i)/SAMPLES);
            p+=Vec2f(px,py);
            glVertex2fv(p);
        }

            glVertex2f(m_camera.getScreenSpace().width,py);
        glEnd();

        Helper::popOrtho2D();
    }

  private:

    PreparedMesh m_heart_prep;

    ShaderObject m_heart_vert,m_heart_frag;
    ProgramObject m_heart_prog;

    ShaderObject m_bg_frag;
    ProgramObject m_bg_prog;

    DynamicArray<Vec2f> m_points;
};

int __cdecl main()
{
    // Make sure the window is destroyed before dumping memory leaks.
    {
        DemoWindow window;

        ShowWindow(window.windowHandle(),SW_SHOW);
        window.processEvents();
    }

#if !defined NDEBUG && !defined GALE_TINY_CODE
    _CrtDumpMemoryLeaks();
#endif

    return 0;
}
