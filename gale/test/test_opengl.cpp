#include <windows.h>

class RenderTarget {
  public:
    RenderTarget();

  protected:
    HGLRC m_rendering_context;
};

RenderTarget::RenderTarget() {
    m_rendering_context=wglCreateContext
    
}

int main() {

    system("pause");
    return 0;
}
