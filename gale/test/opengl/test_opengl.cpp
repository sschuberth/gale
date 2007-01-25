#include <gale/wrapgl/renderwindow.h>

#include <iostream>

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

    void onPaint() {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0,0,float(i)/255.0f,0);
    }

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
