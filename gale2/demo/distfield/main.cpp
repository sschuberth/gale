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
        // See <http://sizecoding.blogspot.com/2008/02/chocolux-1k-intro.html>.
        int t=static_cast<int>(GetTickCount());
        glRecti(-t,-t,t,t);
    }
};

int __cdecl main()
{
    DemoWindow window;

    ShowWindow(window.windowHandle(),SW_SHOW);
    window.processEvents();

    return 0;
}
