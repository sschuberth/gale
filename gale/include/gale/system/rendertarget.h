#ifndef RENDERCONTEXT_H
#define RENDERCONTEXT_H

#include "../global/platform.h"

namespace gale {

namespace system {

class RenderContext
{
  public:

    RenderContext();
    ~RenderContext();

    virtual HWND getWindowHandle() const {
        return m_wnd;
    }

    virtual HDC getDeviceHandle() const {
        return m_dc;
    }

    virtual HGLRC getRenderHandle() const {
        return m_rc;
    }

  protected:

    void destroy();

    static ATOM s_atom;

  private:

    static int s_instances;

    HWND m_wnd;
    HDC m_dc;
    HGLRC m_rc;
};

} // namespace system

} // namespace gale

#endif // RENDERCONTEXT_H
