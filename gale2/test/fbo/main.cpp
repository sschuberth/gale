#include <gale/wrapgl/defaultwindow.h>
#include <gale/wrapgl/framebufferobject.h>

using namespace gale::math;
using namespace gale::wrapgl;

class TestWindow:public DefaultWindow
{
  public:

    TestWindow()
    :   DefaultWindow(_T("test_fbo"),800,600)
    ,   m_ping_pong(0)
    {
        m_camera.setProjection(Mat4d::Factory::OrthographicProjection(0.0,1.0,0.0,1.0));

        // Set up the read and draw textures.
        m_read_texture=&m_textures[m_ping_pong];
        m_ping_pong=(m_ping_pong+1)&1;
        m_draw_texture=&m_textures[m_ping_pong];

        unsigned char* buffer=new unsigned char[256*256*3];

        memset(buffer,0,256*256*3);
        for (int y=0;y<256;++y) {
            for (int x=0;x<256;++x) {
                if (x<0+16 || x>255-16 || y<0+16 || y>255-16) {
                    int offset=(y*256+x)*3;
                    buffer[offset+0]=206;
                    buffer[offset+1]=125;
                    buffer[offset+2]=143;
                }
            }
        }

        m_read_texture->setData(256,256,buffer,GL_RGB,GL_UNSIGNED_BYTE,GL_RGB);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

        delete [] buffer;

        glEnable(GL_TEXTURE_2D);

        //m_frame_buffer.attach(GL_COLOR_ATTACHMENT0,*m_draw_texture);
    }

    void onResize(int width,int height) {
        m_camera.setScreenSpaceDimensions(width,height);
        repaint();
    }

    void onRender() {
        m_camera.makeCurrent();

        // Render a screen-space aligned quad with the read texture applied.
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f,0.0f);
            glVertex2f(0.0f,0.0f);

            glTexCoord2f(1.0f,0.0f);
            glVertex2f(1.0f,0.0f);

            glTexCoord2f(1.0f,1.0f);
            glVertex2f(1.0f,1.0f);

            glTexCoord2f(0.0f,1.0f);
            glVertex2f(0.0f,1.0f);
        glEnd();
    }

  private:

    int m_ping_pong;

    Texture2D m_textures[2];
    Texture2D *m_read_texture,*m_draw_texture;

    FrameBufferObject m_frame_buffer;
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

#if !defined NDEBUG && !defined GALE_TINY_CODE
    _CrtDumpMemoryLeaks();
#endif

    return 0;
}
