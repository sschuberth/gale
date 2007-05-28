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

    /// Stores the location and size of the surface portion that the camera
    /// renders to.
    struct ScreenSpace {
        GLint x;        ///< Left origin of the screen space.
        GLint y;        ///< Upper origin of the screen space.
        GLsizei width;  ///< Width of the screen space.
        GLsizei height; ///< Height of the screen space.
    };

    Camera(system::RenderSurface const& surface):
      m_surface(surface),m_screen_dirty(false) {
        // Initialize the camera screen space to the current OpenGL viewport.
        glGetIntegerv(GL_VIEWPORT,reinterpret_cast<GLint*>(&m_screen));

        // Set a perspective camera with no transformation by default.
        setModelview(math::HMat4f::IDENTITY());
        setProjection(math::Mat4d::Factory::PerspectiveProjection(m_screen.width,m_screen.height));
    }

    system::RenderSurface const& getSurface() const {
        return m_surface;
    }

    void setScreenSpaceOrigin(int x,int y) {
        m_screen.x=x;
        m_screen.y=y;
        m_screen_dirty=true;
    }

    void setScreenSpaceDimensions(int width,int height) {
        m_screen.width=width;
        m_screen.height=height;
        m_screen_dirty=true;
    }

    ScreenSpace const& getScreenSpace() const {
        return m_screen;
    }

    void setModelview(math::HMat4f const& modelview) {
        m_modelview=modelview;
        m_modelview_dirty=true;
    }

    math::HMat4f const& getModelview() const {
        return m_modelview;
    }

    void setProjection(math::Mat4d const& projection) {
        m_projection=projection;
        m_projection_dirty=true;
    }

    math::Mat4d const& getProjection() const {
        return m_projection;
    }

    void apply();

  protected:

    static Camera* s_current;

    system::RenderSurface const& m_surface; ///< The surface that the camera is attached to.

    ScreenSpace m_screen; ///< The camera's current screen space.
    bool m_screen_dirty;

    math::HMat4f m_modelview; ///< Transformation matrix of the camera.
    bool m_modelview_dirty;

    math::Mat4d m_projection; ///< Projection matrix of the camera.
    bool m_projection_dirty;
};

#pragma pack(pop)

} // namespace wrapgl

} // namespace gale

#endif // CAMERA_H
