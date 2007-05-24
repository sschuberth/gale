#ifndef CAMERA_H
#define CAMERA_H

#include "../system/rendersurface.h"
#include "../math/hmatrix4.h"
#include "../math/matrix4.h"

namespace gale {

namespace wrapgl {

// Make sure data members are tightly packed.
#pragma pack(push,1)

class Camera
{
  public:

    Camera() {
        screen.x=screen.y=0;
        screen.width=screen.height=-1;

        modelview=math::HMat4f::IDENTITY();
        projection=math::Mat4d::Factory::PerspectiveProjection();
    }

    void applyScreenSpace(RenderWindow const& window) {
        // Perform lazy initialization.
        if (screen.width<0 || screen.height<0) {
            glGetIntegerv(GL_VIEWPORT,reinterpret_cast<GLint*>(&screen));
        }

        // Get the window's client area size.
        RECT rect;
        BOOL result=GetClientRect(window.getWindowHandle(),&rect);
        G_ASSERT(result!=FALSE)

        // If the desired screen space is smaller than the window, we need to
        // enable scissoring to avoid glClear to affect the whole window plane.
        if (screen.x>0 || (screen.x<0 && screen.width<rect.right-screen.x)
         || screen.y>0 || (screen.y<0 && screen.height<rect.bottom-screen.y)) {
            glScissor(screen.x,screen.y,screen.width,screen.height);
            glEnable(GL_SCISSOR_TEST);
        } else {
            glDisable(GL_SCISSOR_TEST);
        }

        // Set the viewport OpenGL should render to.
        glViewport(screen.x,screen.y,screen.width,screen.height);
    }

    struct ScreenSpace {
        GLint x;        ///< Upper left origin of the screen space.
        GLint y;        ///< \copydoc x
        GLsizei width;  ///< Size of the screen space.
        GLsizei height; ///< \copydoc width
    } screen;

    math::HMat4f modelview;
    math::Mat4d projection;
};

#pragma pack(pop)

} // namespace wrapgl

} // namespace gale

#endif // CAMERA_H
