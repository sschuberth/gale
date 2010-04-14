#include <gale/wrapgl/defaultwindow.h>
#include <gale/wrapgl/framebufferobject.h>

using namespace gale::math;
using namespace gale::wrapgl;

class TestWindow:public DefaultWindow
{
    static int const TEX_SIZE    = 256;
    static int const BORDER_SIZE =  16;

  public:

    TestWindow()
    :   DefaultWindow(_T("test_fbo"),600,600)
    ,   m_win_camera(m_camera)
    ,   m_read_texture(&m_fbo_textures[0])
    ,   m_draw_texture(&m_fbo_textures[1])
    ,   m_ping_pong(0)
    ,   m_angle1(0)
    ,   m_angle2(0)
    {
        unsigned char* buffer=new unsigned char[TEX_SIZE*TEX_SIZE*3];

        // Initialize the pattern texture to a frame pattern.
        for (int y=0;y<TEX_SIZE;++y) {
            for (int x=0;x<TEX_SIZE;++x) {
                int offset=(y*TEX_SIZE+x)*3;
                if (x<0+BORDER_SIZE || x>=TEX_SIZE-BORDER_SIZE || y<0+BORDER_SIZE || y>=TEX_SIZE-BORDER_SIZE) {
                    buffer[offset+0]=255;
                    buffer[offset+1]=255;
                    buffer[offset+2]=255;
                }
                else {
                    buffer[offset+0]=0;
                    buffer[offset+1]=0;
                    buffer[offset+2]=0;
                }
            }
        }

        m_pattern_texture.setData(TEX_SIZE,TEX_SIZE,buffer,GL_RGB,GL_UNSIGNED_BYTE,GL_RGB);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

        // Initialize the read and draw textures to 0.
        memset(buffer,0,TEX_SIZE*TEX_SIZE*3);

        m_read_texture->setData(TEX_SIZE,TEX_SIZE,buffer,GL_RGB,GL_UNSIGNED_BYTE,GL_RGB);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);

        m_draw_texture->setData(TEX_SIZE,TEX_SIZE,buffer,GL_RGB,GL_UNSIGNED_BYTE,GL_RGB);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);

        delete [] buffer;

        // Initialize OpenGL states.
        glEnable(GL_TEXTURE_2D);

        // Implicitly generate texture coordinates from vertex coordinates to be
        // able to use glRect() to draw textured screen-space aligned rectangles.
        glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
        glEnable(GL_TEXTURE_GEN_S);
        glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
        glEnable(GL_TEXTURE_GEN_T);

        // As only screen-aligned rectangles are to be rendered, both cameras
        // use an orthographic projection.
        static Mat4d const p=Mat4d::Factory::OrthographicProjection(0.0,1.0,0.0,1.0,0);
        m_fbo_camera.setProjection(p);
        m_win_camera.setProjection(p);

        // The FBO's viewport size needs to match the attached texture size.
        m_fbo_camera.setScreenSpaceDimensions(TEX_SIZE,TEX_SIZE);

        setTimeout(0.02);
    }

    bool onIdle() {
        m_fbo_camera.makeCurrent();

        m_frame_buffer.attach(GL_COLOR_ATTACHMENT0,*m_draw_texture);

        /*
         * Outer border frame
         */

        // Introduce some color that fades with the iterations.
        glColor3ub(182,212,100);

        // Render the current pattern.
        glPushMatrix();

        glTranslatef(0.5,0.5,0);
        glRotatef(m_angle1,0,0,1);
        glTranslatef(-0.5,-0.5,0);

        m_pattern_texture.makeCurrent();
        glRectf(0,0,1,1);

        glPopMatrix();

        /*
         * Inner border frame
         */

        // Scaling factor to take border size into account.
        static float const fb=static_cast<float>(TEX_SIZE-BORDER_SIZE*2)/TEX_SIZE;
        // Cathetus length for the original border frame.
        static float const kc=::sin(45*Constf::DEG_TO_RAD());

        // Angle of the rotated border frame.
        float ar=::fmod(::abs(m_angle2-m_angle1),90)+45;
        // Cathetus length for the rotated border frame.
        float kr=::sin(ar*Constf::DEG_TO_RAD());
        // Scaling factor to take the rotation into account.
        float fr=kc/kr;

        // Render the previous frame.
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();

        glTranslatef(0.5,0.5,0);
        glRotatef(m_angle1,0,0,1);
        glScalef(fb,fb,1);
        glTranslatef(-0.5,-0.5,0);

        glMatrixMode(GL_TEXTURE);
        glPushMatrix();

        glTranslatef(0.5,0.5,0);
        glRotatef(m_angle2-m_angle1,0,0,1);
        glScalef(1.0f/fr,1.0f/fr,1);
        glTranslatef(-0.5,-0.5,0);

        m_read_texture->makeCurrent();
        glRectf(0,0,1,1);

        glPopMatrix();

        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        // Swap read & draw textures.
        m_draw_texture=&m_fbo_textures[m_ping_pong];
        m_ping_pong=(m_ping_pong+1)&1;
        m_read_texture=&m_fbo_textures[m_ping_pong];

        return true;
    }

    void onTimeout() {
        static int counter1=0,counter2=10;

        if (counter1++>=360) {
            counter1-=360;
        }
        if (counter2++>=360) {
            counter2-=360;
        }

        float step1=::sin(counter1*Constf::DEG_TO_RAD())*4;
        float step2=::sin(counter2*Constf::DEG_TO_RAD())*4;

        m_angle1+=step1;
        m_angle2+=step2;
    }

    void onRender() {
        // Render a screen-space aligned quad to the window's viewport with the
        // current draw texture applied.
        m_win_camera.makeCurrent();

        m_frame_buffer.setCurrent(0);
        m_draw_texture->makeCurrent();

        glRectf(0,0,1,1);
    }

    void onResize(int width,int height) {
        // Just adjust the viewport, not the projection.
        m_win_camera.setScreenSpaceDimensions(width,height);
        repaint();
    }

  private:

    Camera m_fbo_camera;
    Camera& m_win_camera;

    Texture2D m_pattern_texture,m_fbo_textures[2];
    Texture2D *m_read_texture,*m_draw_texture;

    FrameBufferObject m_frame_buffer;

    int m_ping_pong;

    float m_angle1,m_angle2;
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
