#ifndef CAMERA_H
#define CAMERA_H

#include "../system/rendercontext.h"
#include "../math/hmatrix4.h"
#include "../math/matrix4.h"

namespace gale {

namespace wrapgl {

// Make sure data members are tightly packed.
#pragma pack(push,1)

class Camera
{
  public:

	//PerspectiveCamera(const Vector3f& position=Vector3f(0,0,5),const Vector3f& look_target=Vector3f::ZERO(),
	//	const Vector3f& up_direction=Vector3f::Y());
    Camera() {
        setViewport();
        modelview=math::HMat4f::IDENTITY();
        projection=math::Mat4d::IDENTITY();
    }

    void setViewport() {
        if (system::RenderContext::getCurrent()) {
            glGetIntegerv(GL_VIEWPORT,&m_view_x);
        } else {
            m_view_x=m_view_y=0;
            m_view_width=m_view_height=1;
        }
    }

    void setViewport(GLint view_x,GLint view_y,GLsizei view_width,GLsizei view_height) {
        m_view_x      = math::max(0,view_x);
        m_view_y      = math::max(0,view_y);
        m_view_width  = math::max(1,view_width);
        m_view_height = math::max(1,view_height);
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

  private:

    /// Upper left origin of the viewport.
    GLint m_view_x,m_view_y;

    /// Width and height of the viewport. These are not GLsizei to be able to
    /// get the viewport via glGetIntegerv().
    GLint m_view_width,m_view_height;

  public:

    math::HMat4f modelview;
    math::Mat4d projection;
};

#pragma pack(pop)

} // namespace wrapgl

} // namespace gale

#endif // CAMERA_H
