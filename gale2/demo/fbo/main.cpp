// Enable memory leak detection, see:
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vsdebug/html/vxcondetectingisolatingmemoryleaks.asp
#if !defined NDEBUG && !defined GALE_TINY_CODE
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
#endif

#include <gale/wrapgl/defaultwindow.h>
#include <gale/wrapgl/framebufferobject.h>

// Defined as part of ARB_texture_border_clamp or OpenGL 1.3.
#ifndef GL_CLAMP_TO_BORDER
    #define GL_CLAMP_TO_BORDER 0x812D
#endif

// Defined as part of EXT_texture_edge_clamp or OpenGL 1.2.
#ifndef GL_CLAMP_TO_EDGE
    #define GL_CLAMP_TO_EDGE   0x812F
#endif

// Switching the draw buffer to a different color attachment should be fastest,
// see page 29 in http://download.nvidia.com/developer/presentations/2005/GDC/OpenGL_Day/OpenGL_FrameBuffer_Object.pdf.
#define SWITCH_DRAW_BUFFER

using namespace gale::math;
using namespace gale::wrapgl;

class TestWindow:public DefaultWindow
{
    static int const TEX_SIZE     = 256;
    static int const TILE_SIZE    = 8;
    static int const BORDER_TILES = 5;

  public:

    TestWindow()
    :   DefaultWindow(_T("demo_fbo"),600,600)
    ,   m_win_camera(m_camera)
    ,   m_draw_texture(&m_fbo_textures[0])
    ,   m_read_texture(&m_fbo_textures[1])
    ,   m_ping_pong(0)
    ,   m_angle(0)
    {
        unsigned char* buffer=new unsigned char[TEX_SIZE*TEX_SIZE*3];

        // Initialize the pattern texture to a frame pattern.
        for (int y=0;y<TEX_SIZE;++y) {
            for (int x=0;x<TEX_SIZE;++x) {
                int offset=(y*TEX_SIZE+x)*3;

                int xc=((x%TILE_SIZE*2)<TILE_SIZE);
                int yc=((y%TILE_SIZE*2)<TILE_SIZE);
                unsigned char color=((xc+yc)&1)*255;

                buffer[offset+0]=color;
                buffer[offset+1]=color;
                buffer[offset+2]=color;
            }
        }

        m_pattern_texture.setData(TEX_SIZE,TEX_SIZE,buffer,GL_RGB,GL_UNSIGNED_BYTE,GL_RGB);

        // NOTE: Enabling this filter causes garbage to be rendered to the FBO
        // on my notebook's GeForce Go 7700 using version 179.48 drivers, but
        // leaving it at the default of GL_NEAREST_MIPMAP_LINEAR is no option as
        // we have no mipmaps.
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

        // Initialize the read and draw textures to 0.
        memset(buffer,0,TEX_SIZE*TEX_SIZE*3);

        m_draw_texture->setData(TEX_SIZE,TEX_SIZE,buffer,GL_RGB,GL_UNSIGNED_BYTE,GL_RGB);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);

        m_read_texture->setData(TEX_SIZE,TEX_SIZE,buffer,GL_RGB,GL_UNSIGNED_BYTE,GL_RGB);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);

        delete [] buffer;

#ifdef SWITCH_DRAW_BUFFER
        m_frame_buffer.attach(GL_COLOR_ATTACHMENT0,*m_draw_texture);
        m_frame_buffer.attach(GL_COLOR_ATTACHMENT1,*m_read_texture);
#endif

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
        // As FBO do not have their own rendering context as PBuffers do, we can
        // apply the camera settings before binding the FBO.
        m_fbo_camera.apply();

#ifdef SWITCH_DRAW_BUFFER
        m_frame_buffer.bind();
        glDrawBuffer(GL_COLOR_ATTACHMENT0+m_ping_pong);
        G_ASSERT_OPENGL
#else
        m_frame_buffer.attach(GL_COLOR_ATTACHMENT0,*m_draw_texture);
#endif

        glClear(GL_COLOR_BUFFER_BIT);

        // Introduce some color that also fades the image as it is iteratively
        // modulated to the texture color.
        glColor3ub(182,212,100);

        /*
         * Render a large rotated rectangle with the pattern texture applied.
         * Scale the rectangle so it will always fit the viewport no matter
         * of the rotation angle.
         */

        // Minimum opposite cathetus length.
        static float const cath_min=sin(45*Constf::DEG_TO_RAD());

        // Current opposite cathetus length.
        float angle=fmod(abs(m_angle),90)+45;
        float cath=sin(angle*Constf::DEG_TO_RAD());

        float factor=cath_min/cath;

        glPushMatrix();

        glTranslatef(0.5,0.5,0);
        glRotatef(m_angle,0,0,1);
        glScalef(factor,factor,1.0f);
        glTranslatef(-0.5,-0.5,0);

        m_pattern_texture.bind();
        glRectf(0,0,1,1);

        glPopMatrix();

        /*
         * Render a small rotated rectangle with the previous frame applied.
         */

        // Calculate the the additional scaling for the border.
        float scale=(1-static_cast<float>(BORDER_TILES)/(TEX_SIZE/TILE_SIZE));
        factor*=scale;

        glPushMatrix();

        glTranslatef(0.5,0.5,0);
        glRotatef(m_angle,0,0,1);
        glScalef(factor,factor,1.0f);
        glTranslatef(-0.5,-0.5,0);

        m_read_texture->bind();
        glRectf(0,0,1,1);

        glPopMatrix();

        // Swap read & draw textures.
        m_read_texture=&m_fbo_textures[m_ping_pong];
        m_ping_pong=(m_ping_pong+1)&1;
        m_draw_texture=&m_fbo_textures[m_ping_pong];

        return true;
    }

    void onTimeout() {
        static int counter=0;

        if (counter++>=360) {
            counter-=360;
        }

        float step1=sin(counter*Constf::DEG_TO_RAD())*2;

        m_angle+=step1;
    }

    void onRender() {
        // Render a screen-space aligned quad to the window's viewport with the
        // current draw texture applied.
        m_win_camera.apply();

        m_frame_buffer.release();
        m_draw_texture->bind();

        glRectf(0,0,1,1);
    }

    void onResize(int width,int height) {
        // Just adjust the viewport, not the projection.
        m_win_camera.setScreenSpaceDimensions(width,height);
        repaint();
    }

  private:

    Camera m_fbo_camera;
    Texture2D m_pattern_texture;

    Camera& m_win_camera;
    Texture2D m_fbo_textures[2],*m_draw_texture,*m_read_texture;
    FrameBufferObject m_frame_buffer;
    int m_ping_pong;

    float m_angle;
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
