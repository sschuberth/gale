#ifndef TEST_WINDOW
#define TEST_WINDOW

#include <gale/wrapgl/renderwindow.h>
#include <gale/wrapgl/camera.h>

#include <iostream>

// warning C4355: 'this' : used in base member initializer list
#pragma warning(disable:4355)

class DefaultWindow:public gale::wrapgl::RenderWindow
{
  public:

    static gale::system::AttributeListi const& getDefaultAttributes() {
        static gale::system::AttributeListi attribs;
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

    DefaultWindow(LPCTSTR title):
      RenderWindow(500,500,getDefaultAttributes(),title),m_camera(*this) {
        std::cout << "Vendor   : " << glGetString(GL_VENDOR) << std::endl;
        std::cout << "Renderer : " << glGetString(GL_RENDERER) << std::endl;
        std::cout << "Version  : " << glGetString(GL_VERSION) << std::endl;
    }

  protected:

    gale::wrapgl::Camera m_camera;
};

// warning C4355: 'this' : used in base member initializer list
#pragma warning(default:4355)

#endif // TEST_WINDOW
