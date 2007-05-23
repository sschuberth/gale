#include <gale/wrapgl/renderwindow.h>
#include <gale/wrapgl/camera.h>

#include <iostream>

using namespace gale::math;
using namespace gale::system;
using namespace gale::wrapgl;

using namespace std;

class TestWindow:public RenderWindow
{
  public:

    static AttributeListi const& getDefaultAttributes() {
        static AttributeListi attribs;
        if (attribs.getSize()==0) {
            attribs.insert(WGL_PIXEL_TYPE_ARB,WGL_TYPE_RGBA_ARB);
            attribs.insert(WGL_COLOR_BITS_ARB,24);
            attribs.insert(WGL_RED_BITS_ARB,8);
            attribs.insert(WGL_GREEN_BITS_ARB,8);
            attribs.insert(WGL_BLUE_BITS_ARB,8);
            attribs.insert(WGL_ALPHA_BITS_ARB,8);
            attribs.insert(WGL_DEPTH_BITS_ARB,24);
        }
        return attribs;
    }

    TestWindow(LPCTSTR title):RenderWindow(500,500,getDefaultAttributes(),title) {
        cout << glGetString(GL_VENDOR) << endl;
        cout << glGetString(GL_RENDERER) << endl;
        cout << glGetString(GL_VERSION) << endl;
    }

    bool onIdle() {
        i=(i+1)%256;
        return true;
    }

    void onSize(int width,int height) {
        //m_camera.setViewport();
        cout << "Window size: " << width << " x " << height << endl;
    }

    void onPaint() {
        static Vec3f vertices[8]={
            Vec3f(-1.0f, -1.0f, 0.0f),
            Vec3f(+1.0f, -1.0f, 0.0f),
            Vec3f(+1.0f, +1.0f, 0.0f),
            Vec3f(-1.0f, +1.0f, 0.0f),
            //Vec3f(-1.0f, -1.0f, +1.0f),
            //Vec3f(+1.0f, -1.0f, +1.0f),
            //Vec3f(+1.0f, +1.0f, +1.0f),
            //Vec3f(-1.0f, +1.0f, +1.0f),
            Vec3f(+1.0f, -1.0f, -1.0f),
            Vec3f(-1.0f, -1.0f, -1.0f),
            Vec3f(-1.0f, +1.0f, -1.0f),
            Vec3f(+1.0f, +1.0f, -1.0f)
        };

        static GLuint indices[]={
            // Front & back quadrilaterals.
            0,1,2,3,
            4,5,6,7,
            // Bottom & top quadrilaterals.
            5,4,1,0,
            3,2,7,6,
            // Right & left quadrilaterals.
            1,4,7,2,
            5,0,3,6
        };

        glDisable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);
        glEnableClientState(GL_VERTEX_ARRAY);

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(1,0,0,0);
        G_ASSERT(glGetError()==GL_NO_ERROR);

        m_camera.activate(*this);
        GLint v[4];
        glGetIntegerv(GL_VIEWPORT,v);
        cout << "Viewport size: " << v[2] << " x " << v[3] << endl;
        //glVertexPointer(3,GL_FLOAT,0,vertices);
        //G_ASSERT(glGetError()==GL_NO_ERROR);

        //glDrawElements(GL_QUADS,6*4,GL_UNSIGNED_INT,indices);
        //G_ASSERT(glGetError()==GL_NO_ERROR);
    }

  private:

    Camera m_camera;
    int i;
};

int main()
{
    TestWindow window("test_opengl");

    ShowWindow(window.getWindowHandle(),SW_SHOW);
    window.processEvents();

#ifdef _WIN32
    system("pause");
#endif
    return 0;
}
