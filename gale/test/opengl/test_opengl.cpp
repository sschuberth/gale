#include <stdlib.h>

#include <iostream>

#include <gale/wrapgl/renderwindow.h>

using namespace std;

using namespace gale::system;
using namespace gale::wrapgl;

int main()
{
    AttributeListi attribs;
    attribs.insert(WGL_PIXEL_TYPE_ARB,WGL_TYPE_RGBA_ARB);
    attribs.insert(WGL_COLOR_BITS_ARB,24);
    attribs.insert(WGL_RED_BITS_ARB,8);
    attribs.insert(WGL_GREEN_BITS_ARB,8);
    attribs.insert(WGL_BLUE_BITS_ARB,8);
    attribs.insert(WGL_ALPHA_BITS_ARB,8);
    attribs.insert(WGL_DEPTH_BITS_ARB,24);

    RenderWindow window(300,300,attribs,"test_opengl");
    ShowWindow(window.getWindowHandle(),SW_SHOW);

    MSG queue;
    do {
        while (PeekMessage(&queue,NULL,0,0,PM_REMOVE)) {
            TranslateMessage(&queue);
            DispatchMessage(&queue);
        }
        // Idle if there are no messages to process.
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0,0,1,0);
        glFlush();
        SwapBuffers(window.getDeviceHandle());
    } while (queue.message!=WM_QUIT);

    //cout << glGetString(GL_VENDOR) << endl;
    //cout << glGetString(GL_RENDERER) << endl;
    //cout << glGetString(GL_VERSION) << endl;
    //cout << glGetString(GL_EXTENSIONS) << endl;

#ifdef _WIN32
    system("pause");
#endif
    return 0;
}
