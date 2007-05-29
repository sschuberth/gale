#include "gale/wrapgl/camera.h"

namespace gale {

namespace wrapgl {

Camera* Camera::s_current=NULL;

void Camera::apply() {
    bool camera_changed=(s_current!=this);

    if (camera_changed || m_screen_changed) {
        // Get the window's client area size.
        RECT rect;
        BOOL result=GetClientRect(m_surface.getWindowHandle(),&rect);
        G_ASSERT(result!=FALSE)

        // If the desired screen space is smaller than the window, we need to
        // enable scissoring to avoid glClear to affect the whole window plane.
        if (m_screen.x>0 || (m_screen.x<0 && m_screen.width<rect.right-m_screen.x)
         || m_screen.y>0 || (m_screen.y<0 && m_screen.height<rect.bottom-m_screen.y)) {
            glScissor(m_screen.x,m_screen.y,m_screen.width,m_screen.height);
            glEnable(GL_SCISSOR_TEST);
        } else {
            glDisable(GL_SCISSOR_TEST);
        }

        // Set the viewport OpenGL should render to.
        glViewport(m_screen.x,m_screen.y,m_screen.width,m_screen.height);
        m_screen_changed=false;
    }

    glPushAttrib(GL_TRANSFORM_BIT);

    if (camera_changed || m_modelview_changed) {
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(m_modelview);
        m_modelview_changed=false;
    }

    if (camera_changed || m_projection_changed) {
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixd(m_projection);
        m_projection_changed=false;
    }

    glPopAttrib();

    s_current=this;
}

} // namespace wrapgl

} // namespace gale
