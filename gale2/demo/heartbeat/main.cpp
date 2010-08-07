#include <gale/math/colormodel.h>
#include <gale/math/interpolator.h>
#include <gale/math/random.h>
#include <gale/wrapgl/defaultwindow.h>
#include <gale/wrapgl/renderer.h>
#include <gale/wrapgl/shaderobject.h>

// Enable memory leak detection, see:
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vsdebug/html/vxcondetectingisolatingmemoryleaks.asp
#if !defined NDEBUG && !defined GALE_TINY_CODE
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
#endif

#define DRAW_BACKGROUND
#define DRAW_SMALL_HEARTS
#define DRAW_BIG_HEART
#define DRAW_ECG_CURVE

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

static int const TOTAL_SAMPLES=800;
static int const TAIL_SAMPLES=60;

static float const LINE_WIDTH=3.0f;

static int const NUM_SMALL_HEARTS=50;

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
    ,   m_base_color(0.82f,0.76f,0.8f,1.0f)
    ,   m_scale(1.0f)
    ,   m_heart_vert(GL_VERTEX_SHADER)
    ,   m_heart_frag(GL_FRAGMENT_SHADER)
    ,   m_bg_frag(GL_FRAGMENT_SHADER)
    ,   m_sample(0)
    ,   m_points(ECG)
    ,   m_hearts(NUM_SMALL_HEARTS)
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
            line_path[i]=Vec3f(0.0f,0.0f,-0.2f+i*0.1f);
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

        // Initialize the flying hearts.
        for (int i=0;i<m_hearts.getSize();++i) {
            m_hearts[i].position=Vec2f::ZERO();

            float speed=0.5f+m_rand.random0N(0.5f);
            m_hearts[i].velocity=m_rand.randomVec2()*speed;

            float vr=10.0f;
            float vs=m_rand.random0N(2*vr)-vr;
            ColorModelHSV hsv(m_base_color);
            hsv.setS(hsv.getS()+vs);
            m_hearts[i].color=hsv.rgb<Col4f>();
        }

        // Compile and link the shaders.
        GLchar log[4096];

        m_heart_vert.setSource(&shader_fake_sss_vert);
        if (!m_heart_vert.compile() || m_heart_vert.hasLog()) {
            m_heart_vert.getLog(log,sizeof(log));
            puts(log);
        }

        m_heart_frag.setSource(&shader_fake_sss_frag);
        if (!m_heart_frag.compile() || m_heart_frag.hasLog()) {
            m_heart_frag.getLog(log,sizeof(log));
            puts(log);
        }

        m_heart_prog.attach(m_heart_vert);
        m_heart_prog.attach(m_heart_frag);
        if (!m_heart_prog.link() || m_heart_prog.hasLog()) {
            m_heart_prog.getLog(log,sizeof(log));
            puts(log);
        }

        m_bg_frag.setSource(&shader_bg_frag);
        if (!m_bg_frag.compile() || m_bg_frag.hasLog()) {
            m_bg_frag.getLog(log,sizeof(log));
            puts(log);
        }

        m_bg_prog.attach(m_bg_frag);
        if (!m_bg_prog.link() || m_bg_prog.hasLog()) {
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
        glUniform4fv(l,1,m_base_color);
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

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_LINE_SMOOTH);
        glLineWidth(LINE_WIDTH);

        // Set the ECG animation timeout.
        setTimeout(0.002);
    }

    void onTimeout() {
        m_sample=wrap(++m_sample,TOTAL_SAMPLES);
        repaint();
    }

    void onRender() {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        m_camera.apply();

        Helper::pushOrtho2D();
        glDepthMask(GL_FALSE);

#ifdef DRAW_BACKGROUND
        // Draw the background.
        m_bg_prog.bind();

        GLint l=glGetUniformLocation(m_bg_prog.handle(),"viewport");
        glUniform2i(l,m_camera.getScreenSpace().width,m_camera.getScreenSpace().height);
        G_ASSERT_OPENGL

        glRecti(0,0,m_camera.getScreenSpace().width,m_camera.getScreenSpace().height);

        m_bg_prog.release();
#endif

#ifdef DRAW_SMALL_HEARTS
        // Draw the flying hearts.
        glPushMatrix();

        float cx=m_camera.getScreenSpace().width*0.5f;
        float cy=m_camera.getScreenSpace().height*0.5f;
        glTranslatef(cx,cy,0);

        for (int i=0;i<m_hearts.getSize();++i) {
            glPushMatrix();

            glTranslatef(m_hearts[i].position.getX(),m_hearts[i].position.getY(),0);
            float scale=20.0f*m_scale;
            glScalef(scale,scale,1.0f);

            glColor4fv(m_hearts[i].color);
            Renderer::draw(m_heart_prep);

            glPopMatrix();

            m_hearts[i].position+=m_hearts[i].velocity*m_scale*m_scale;
            if (m_hearts[i].position.getX()<-cx || m_hearts[i].position.getX()>cx
             || m_hearts[i].position.getY()<-cy || m_hearts[i].position.getY()>cy)
            {
                m_hearts[i].position=Vec2f::ZERO();

                float speed=0.5f+m_rand.random0N(0.5f);
                m_hearts[i].velocity=m_rand.randomVec2()*speed;
            }
        }

        glPopMatrix();
#endif

        glDepthMask(GL_TRUE);
        Helper::popOrtho2D();

#ifdef DRAW_BIG_HEART
        // Draw the heart.
        glPushMatrix();
        glScalef(m_scale,m_scale,1.0f);

        int rotation_sample=0;
        if (m_sample>TOTAL_SAMPLES/4*3) {
            rotation_sample=m_sample-TOTAL_SAMPLES/4*3;
        }
        if (m_sample<TOTAL_SAMPLES/4) {
            rotation_sample=m_sample+TOTAL_SAMPLES/4;
        }
        float s=static_cast<float>(rotation_sample)/(TOTAL_SAMPLES/2);
        glRotatef((1.0f-cos(Constf::PI()*s))*90.0f,0,1,0);

        m_heart_prog.bind();
        Renderer::draw(m_heart_prep);
        m_heart_prog.release();

        glPopMatrix();
#endif

#ifdef DRAW_ECG_CURVE
        // Draw the ECG curve.
        Helper::pushOrtho2D();

        float px=m_camera.getScreenSpace().width/2.0f;
        float py=m_camera.getScreenSpace().height/4.0f;

        Vec2f head=drawECGCurve(px,py,1.0f,1.0f,1.0f);
        drawECGCurve(px,py-LINE_WIDTH,m_base_color.getR(),m_base_color.getG(),m_base_color.getB());

        Helper::popOrtho2D();

        // Scale the heart according to the ECG value.
        m_scale=1.0f+static_cast<float>(head.getY()-py)/250.0f;
#endif
    }

    void onMouseEvent(int x,int y,int wheel,int event) {
        // Prevent mouse events.
    }

  private:

    Vec2f drawECGCurve(float px,float py,float r,float g,float b) {
        glBegin(GL_LINE_STRIP);

        Vec2f p0,p1,p;

        float alpha_max=0;
        Vec2f head;

        // Draw the first straight line segment.
        p0=Vec2f(0,py);
        p1=m_points.first()+Vec2f(px,py);
        for (int i=0;i<=TOTAL_SAMPLES/4;++i) {
            // Determine the color.
            int t=m_sample-i;
            float alpha=static_cast<float>(TAIL_SAMPLES-t)/TAIL_SAMPLES;
            if (alpha<0 || alpha>1) {
                alpha=0;
            }
            glColor4f(r,g,b,alpha);

            // Calculate the position.
            p=lerp(p0,p1,static_cast<float>(i)/(TOTAL_SAMPLES*0.25f));
            glVertex2fv(p);

            // Determine the head position.
            if (alpha>alpha_max) {
                alpha_max=alpha;
                head=p;
            }
        }

        // Draw the ECG curve.
        for (int i=0;i<=TOTAL_SAMPLES/2;++i) {
            // Determine the color.
            int t=m_sample-i-TOTAL_SAMPLES/4;
            float alpha=static_cast<float>(TAIL_SAMPLES-t)/TAIL_SAMPLES;
            if (alpha<0 || alpha>1) {
                alpha=0;
            }
            glColor4f(r,g,b,alpha);

            // Calculate the position.
            p=Interpolator::CatmullRom(m_points,static_cast<float>(i)/(TOTAL_SAMPLES*0.5f));
            p+=Vec2f(px,py);
            glVertex2fv(p);

            // Determine the head position.
            if (alpha>alpha_max) {
                alpha_max=alpha;
                head=p;
            }
        }

        // Draw the last straight line segment.
        p0=m_points.last()+Vec2f(px,py);
        p1=Vec2f(static_cast<float>(m_camera.getScreenSpace().width),py);
        for (int i=0;i<=TOTAL_SAMPLES/4;++i) {
            // Determine the color.
            int t=m_sample-i-TOTAL_SAMPLES/4-TOTAL_SAMPLES/2;
            float alpha=static_cast<float>(TAIL_SAMPLES-t)/TAIL_SAMPLES;
            if (alpha<0 || alpha>1) {
                alpha=0;
            }
            glColor4f(r,g,b,alpha);

            // Calculate the position.
            p=lerp(p0,p1,static_cast<float>(i)/(TOTAL_SAMPLES*0.25f));
            glVertex2fv(p);

            // Determine the head position.
            if (alpha>alpha_max) {
                alpha_max=alpha;
                head=p;
            }
        }

        glEnd();

        return head;
    }

    Col4f m_base_color;
    float m_scale;

    PreparedMesh m_heart_prep;

    ShaderObject m_heart_vert,m_heart_frag;
    ProgramObject m_heart_prog;

    ShaderObject m_bg_frag;
    ProgramObject m_bg_prog;

    int m_sample;
    DynamicArray<Vec2f> m_points;

    RandomEcuyerf m_rand;

    struct FlyingHeart {
        Vec2f position;
        Vec2f velocity;
        Col4f color;
    };

    DynamicArray<FlyingHeart> m_hearts;
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
