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
        projection=math::Mat4d::IDENTITY();
    }

    void setScreenSpace(RenderWindow const& window) {
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

    /// Sets the camera to orthographic projection using the specified clipping.
    void setOrthographic(
      double clip_top,       double clip_bottom,
      double clip_left,      double clip_right,
      double clip_near=-1.0, double clip_far=1.0)
    {
        double r_l=clip_right-clip_left;
        double t_b=clip_top-clip_bottom;
        double f_n=clip_far-clip_near;

        projection[0]  = 2.0/r_l;
        projection[1]  = 0.0;
        projection[2]  = 0.0;
        projection[3]  = 0.0;

        projection[4]  = 0.0;
        projection[5]  = 2.0/t_b;
        projection[6]  = 0.0;
        projection[7]  = 0.0;

        projection[8]  = 0.0;
        projection[9]  = 0.0;
        projection[10] = -2.0/f_n;
        projection[11] = -1.0;

        projection[12] = -(clip_right+clip_left)/r_l;
        projection[13] = -(clip_top+clip_bottom)/t_b;
        projection[14] = -(clip_far+clip_near)/f_n;
        projection[15] = 1.0;
    }

    // fov ]0,180[
    void setPerspective(double fov=M_PI*0.25,double clip_near=0.001,double clip_far=1000.0) {
        double f=1.0/::tan(fov*0.5);
        double a=static_cast<double>(m_view_width)/m_view_height;
        double d=clip_near-clip_far;

        projection[0]  = f/a;
        projection[1]  = 0.0;
        projection[2]  = 0.0;
        projection[3]  = 0.0;

        projection[4]  = 0.0;
        projection[5]  = f;
        projection[6]  = 0.0;
        projection[7]  = 0.0;

        projection[8]  = 0.0;
        projection[9]  = 0.0;
        projection[10] = (clip_near+clip_far)/d;
        projection[11] = -1.0;

        projection[12] = 0.0;
        projection[13] = 0.0;
        projection[14] = 2.0*clip_near*clip_far/d;
        projection[15] = 0.0;
    }

    struct ScreenSpace {
        GLint x;        ///< Upper left origin of the screen space.
        GLint y;        ///< \copydoc x
        GLsizei width;  ///< Size of the screen space.
        GLsizei height; ///< Size of the screen space.
    } screen;

    math::HMat4f modelview;
    math::Mat4d projection;
};

#pragma pack(pop)

} // namespace wrapgl

} // namespace gale

#endif // CAMERA_H
