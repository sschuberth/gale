#ifndef CAMERA_H
#define CAMERA_H

/**
 * \file
 * Camera transformation and navigation implementation
 */

#include "../system/rendersurface.h"
#include "../math/hmatrix4.h"
#include "../math/matrix4.h"

namespace gale {

namespace wrapgl {

// Make sure data members are tightly packed.
#pragma pack(push,1)

/**
 * This class defines a camera with a modelview and projection matrix that
 * projects onto a given screen space portion of the render surface that it is
 * attached to.
 */
class Camera
{
  public:

    /// Stores the location and size of the render surface portion that the
    /// camera projects to.
    struct ScreenSpace {
        GLint x;        ///< Left origin of the screen space.
        GLint y;        ///< Upper origin of the screen space.
        GLsizei width;  ///< Width of the screen space.
        GLsizei height; ///< Height of the screen space.
    };

    /// Constructor that initializes a perspective camera attached to the given
    /// render \a surface. By default, its screen space is the current viewport
    /// and no transformation is set to the modelview matrix.
    Camera(system::RenderSurface const& surface):
      m_surface(surface),m_screen_changed(false) {
        // Initialize the camera screen space to the current OpenGL viewport.
        glGetIntegerv(GL_VIEWPORT,reinterpret_cast<GLint*>(&m_screen));

        // Set a perspective camera with no transformation by default.
        setModelview(math::HMat4f::IDENTITY());
        setProjection(math::Mat4d::Factory::PerspectiveProjection(m_screen.width,m_screen.height));
    }

    /// Returns the render surface the camera is attached to.
    system::RenderSurface const& getSurface() const {
        return m_surface;
    }

    /// Sets screen space origin to use.
    void setScreenSpaceOrigin(int x,int y) {
        m_screen.x=x;
        m_screen.y=y;
        m_screen_changed=true;
    }

    /// Sets screen space dimensions to use.
    void setScreenSpaceDimensions(int width,int height) {
        m_screen.width=width;
        m_screen.height=height;
        m_screen_changed=true;
    }

    /// Returns the currently set screen space.
    ScreenSpace const& getScreenSpace() const {
        return m_screen;
    }

    /// Sets the modelview transformation matrix.
    void setModelview(math::HMat4f const& modelview) {
        m_modelview=modelview;
        m_modelview_changed=true;
    }

    /// Returns the current modelview transformation matrix.
    math::HMat4f const& getModelview() const {
        return m_modelview;
    }

    /// Sets the projection matrix.
    void setProjection(math::Mat4d const& projection) {
        m_projection=projection;
        m_projection_changed=true;
    }

    /// Returns the current projection matrix.
    math::Mat4d const& getProjection() const {
        return m_projection;
    }

    /// Activates the camera by applying all its settings to the current render
    /// context.
    void apply();

  protected:

    static Camera* s_current; ///< Common pointer to the last applied camera.

    system::RenderSurface const& m_surface; ///< The surface that the camera is attached to.

    ScreenSpace m_screen;  ///< The camera's current screen space.
    bool m_screen_changed; ///< Marks whether the screen space needs to be applied.

    math::HMat4f m_modelview; ///< Modelview transformation matrix of the camera.
    bool m_modelview_changed; ///< Marks whether the modelview needs to be applied.

    math::Mat4d m_projection;  ///< Projection matrix of the camera.
    bool m_projection_changed; ///< Marks whether the projection needs to be applied.
};

#pragma pack(pop)

} // namespace wrapgl

} // namespace gale

#endif // CAMERA_H
