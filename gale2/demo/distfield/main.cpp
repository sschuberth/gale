#include <gale/wrapgl/defaultwindow.h>

using namespace gale::wrapgl;

class DemoWindow:public MinimalWindow
{
  public:

    DemoWindow()
    :   MinimalWindow(_T("demo_distfield"),800,600)
    {
    }

    void onPaint() {
        glRects(-1,-1,1,1);
    }
};

int __cdecl main()
{
    DemoWindow window;

    ShowWindow(window.windowHandle(),SW_SHOW);
    window.processEvents();

    return 0;
}
