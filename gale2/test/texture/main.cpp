#include <gale/math/color.h>
#include <gale/math/random.h>
#include <gale/wrapgl/defaultwindow.h>
#include <gale/wrapgl/textureobject.h>

using namespace gale::math;
using namespace gale::wrapgl;

class TestWindow:public DefaultWindow
{
  public:

    TestWindow()
    :   DefaultWindow(_T("test_texture"),800,600)
    {
        // Move the camera back to be able to see objects at the origin.
        m_camera.approach(-5);

        // Generate a minimum-sized texture for each face of the cube.
        RandomEcuyerf rand;
        Col3ub color,palette[5];
        Tuple<3,unsigned char> data[2*2];

        color.setHSV(rand.randomExcl0N(360),rand.random0N(100),rand.random0N(100));
        color.blend(palette);

        // The texture is smaller than the default pixel alignment.
        glPixelStorei(GL_UNPACK_ALIGNMENT,1);

        // Cube face 1.
        data[3].set(color.getR(),color.getG(),color.getB());
        data[0]=data[1]=data[2]=data[3];
        m_textures[0].setData(2,2,data,GL_RGB,GL_UNSIGNED_BYTE,GL_RGB);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

        for (int i=1;i<6;++i) {
            // Cube faces 2-6.
            data[3].set(palette[i-1].getR(),palette[i-1].getG(),palette[i-1].getB());
            data[0]=data[1]=data[2]=data[3];
            m_textures[i].setData(2,2,data,GL_RGB,GL_UNSIGNED_BYTE,GL_RGB);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        }

        // Set some OpenGL states.
        glEnable(GL_CULL_FACE);

        //glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glEnable(GL_TEXTURE_2D);
    }

    void onRender() {
        glClear(GL_COLOR_BUFFER_BIT);

        m_camera.makeCurrent();

        // http://mathworld.wolfram.com/Hexahedron.html
        static float const a=0.5f;

        static Vec3f const v[]={
            // Front face vertices.
            Vec3f(+a,+a,+a), // 0
            Vec3f(-a,+a,+a), // 1
            Vec3f(-a,-a,+a), // 2
            Vec3f(+a,-a,+a), // 3

            // Back face vertices.
            Vec3f(-a,+a,-a), // 4
            Vec3f(+a,+a,-a), // 5
            Vec3f(+a,-a,-a), // 6
            Vec3f(-a,-a,-a)  // 7
        };

        // Always sample the single-colored texture at its center.
        glTexCoord2f(0.5f,0.5f);

        // X+
        m_textures[4].makeCurrent();
        glBegin(GL_QUADS);
            glNormal3fv(Vec3f::X());
            glVertex3fv(v[0]);
            glVertex3fv(v[3]);
            glVertex3fv(v[6]);
            glVertex3fv(v[5]);
        glEnd();

        // X-
        m_textures[5].makeCurrent();
        glBegin(GL_QUADS);
            glNormal3fv(-Vec3f::X());
            glVertex3fv(v[1]);
            glVertex3fv(v[4]);
            glVertex3fv(v[7]);
            glVertex3fv(v[2]);
        glEnd();

        // Y+
        m_textures[2].makeCurrent();
        glBegin(GL_QUADS);
            glNormal3fv(Vec3f::Y());
            glVertex3fv(v[0]);
            glVertex3fv(v[5]);
            glVertex3fv(v[4]);
            glVertex3fv(v[1]);
        glEnd();

        // Y-
        m_textures[3].makeCurrent();
        glBegin(GL_QUADS);
            glNormal3fv(-Vec3f::Y());
            glVertex3fv(v[2]);
            glVertex3fv(v[7]);
            glVertex3fv(v[6]);
            glVertex3fv(v[3]);
        glEnd();

        // Z+
        m_textures[0].makeCurrent();
        glBegin(GL_QUADS);
            glNormal3fv(Vec3f::Z());
            glVertex3fv(v[0]);
            glVertex3fv(v[1]);
            glVertex3fv(v[2]);
            glVertex3fv(v[3]);
        glEnd();

        // Z-
        m_textures[1].makeCurrent();
        glBegin(GL_QUADS);
            glNormal3fv(-Vec3f::Z());
            glVertex3fv(v[4]);
            glVertex3fv(v[5]);
            glVertex3fv(v[6]);
            glVertex3fv(v[7]);
        glEnd();
    }

  private:

    Texture2D m_textures[6];
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
