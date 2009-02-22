/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
 *
 * Copyright (C) 2005-2009  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef CAMERA_H
#define CAMERA_H

/**
 * \file
 * An implementation for camera transformation and navigation
 */

#include "../math/matrix4.h"
#include "../math/quaternion.h"

#include "../model/boundingbox.h"
#include "../model/plane.h"

#include "../wrapgl/rendersurface.h"

namespace gale {

namespace wrapgl {

// TODO: Add Linux implementation.
#ifdef G_OS_WINDOWS

// Warning C4355: 'this' used in base member initializer list.
#pragma warning(disable:4355)

/**
 * This class defines a camera with a modelview transformation and projection
 * matrix that projects onto a given screen space portion of the render surface
 * that it is attached to. The camera's coordinate system equals OpenGL's, i.e.
 * it is a right-handed coordinate system where the camera looks forward long
 * the negative z-axis, the positive y-axis marks the "up" direction and the
 * positive x-axis marks the "right" direction.
 */
class Camera
{
  public:

    /// A class to represent the camera's view frustum.
    class Frustum
    {
        friend class Camera;

      public:

        /// Constructs the frustum planes for the given \a camera.
        Frustum(Camera const& camera)
        :   m_camera(camera) {}

        /// Returns whether the given \a point is contained in the frustum.
        bool contains(math::Vec3f const& point);

        /// Returns whether the given \a box is (partly) contained in the frustum.
        bool contains(model::AABB const& box);

        /// Returns whether the given \a box is (partly) contained in the frustum.
        bool contains(model::AABB::Vertices const& box);

      private:

        /// Names for the plane array entries (as seen from the camera).
        enum Face {
            FACE_LEFT,   ///< Index of the left frustum plane.
            FACE_RIGHT,  ///< Index of the right frustum plane.
            FACE_BOTTOM, ///< Index of the bottom frustum plane.
            FACE_TOP,    ///< Index of the top frustum plane.
            FACE_ZNEAR,  ///< Index of the near (clipping) frustum plane.
            FACE_ZFAR,   ///< Index of the far (clipping) frustum plane.
            FACE_COUNT   ///< Helper entry to name the number of enumerations.
        };

        /// (Re-)calculates the frustum planes.
        void calculate();

        /// Disable assignments (to avoid warnings due to the reference member variable).
        Frustum& operator=(Frustum const&);

        Camera const& m_camera;             ///< Reference to the camera.
        model::Plane m_frustum[FACE_COUNT]; ///< Array of frustum planes.
    };

    /// Stores the location and size of the render surface portion that the
    /// camera projects to.
    struct ScreenSpace {
        GLint x;        ///< Left origin of the screen space.
        GLint y;        ///< Upper origin of the screen space.
        GLsizei width;  ///< Width of the screen space.
        GLsizei height; ///< Height of the screen space.
    };

    /// Returns a pointer to the camera that was last made current, or NULL
    /// if there is no current camera.
    static Camera* getCurrent() {
        return s_current;
    }

    /// Makes \a camera the current one. This is merely a convenience method to
    /// equalize APIs among classes.
    static void setCurrent(Camera& camera) {
        camera.makeCurrent();
    }

    /// Constructor that initializes a perspective camera attached to the given
    /// render \a surface. By default, its screen space is the current viewport
    /// and no transformation is set to the modelview matrix.
    Camera(RenderSurface const* surface=NULL)
    :   m_surface(surface)
    ,   m_frustum(*this)
    ,   m_screen_changed(false)
    {
        // Initialize the camera screen space to the current OpenGL viewport.
        glGetIntegerv(GL_VIEWPORT,reinterpret_cast<GLint*>(&m_screen));
        G_ASSERT_OPENGL

        // Set a perspective camera with no transformation by default.
        setModelview(math::HMat4f::IDENTITY());
        setProjection(math::Mat4d::Factory::PerspectiveProjection(m_screen.width,m_screen.height));
    }

    /// Returns a reference to the camera's view frustum.
    Frustum& frustum() {
        return m_frustum;
    }

    /**
     * \name Screen space related methods
     */
    //@{

    /// Returns the currently set screen space.
    ScreenSpace const& getScreenSpace() const {
        return m_screen;
    }

    /// Sets screen space origin to use.
    void setScreenSpaceOrigin(int const x,int const y) {
        m_screen.x=x;
        m_screen.y=y;
        m_screen_changed=true;
    }

    /// Sets screen space dimensions to use.
    void setScreenSpaceDimensions(int const width,int const height) {
        m_screen.width=width;
        m_screen.height=height;
        m_screen_changed=true;
    }

    /// Returns whether the screen space has changed since the camera was last
    /// made current.
    bool hasScreenSpaceChanged() const {
        return m_screen_changed;
    }

    //@}

    /**
     * \name Modelview transformation matrix related methods
     */
    //@{

    /// Returns the current modelview transformation.
    math::HMat4f const& getModelview() const {
        return m_modelview;
    }

    /// Sets the modelview transformation.
    void setModelview(math::HMat4f const& modelview) {
        m_modelview=modelview;
        m_modelview_changed=true;
    }

    /// Returns whether the modelview transformation has changed since the
    /// camera was last made current.
    bool hasModelviewChanged() const {
        return m_modelview_changed;
    }

    //@}

    /**
     * \name Projection matrix related methods
     */
    //@{

    /// Returns the current projection matrix.
    math::Mat4d const& getProjection() const {
        return m_projection;
    }

    /// Sets the projection matrix.
    void setProjection(math::Mat4d const& projection) {
        m_projection=projection;
        m_projection_changed=true;
    }

    /// Returns whether the projection matrix has changed since the camera was
    /// last made current.
    bool hasProjectionChanged() const {
        return m_projection_changed;
    }

    //@}

    /// Makes the camera the current one by applying all its settings to the
    /// current render context. If \a force is \c true, the camera settings are
    /// also applied if they did not change.
    void makeCurrent(bool const force=false);

    /**
     * \name Absolute transformations in the world coordinate system
     */
    //@{

    /// Sets the camera's location to the given \a position.
    math::Vec3f const& getPosition() {
        return m_modelview.getPositionVector();
    }

    /// Sets the camera's location to the given \a position.
    void setPosition(math::Vec3f const& position) {
        m_modelview.setPositionVector(position);
        m_modelview_changed=true;
    }

    /// Makes the camera look at the given \a point. If \a flip is \c true, the
    /// camera is allowed to flip in order to minimize the "right" angle between
    /// old and new orientation.
    void setLookTarget(math::Vec3f const& point,bool const flip=false) {
        using namespace math;

        HMat4f::Vec const& position=m_modelview.getPositionVector();

        HMat4f::Vec& backward=m_modelview.getBackwardVector();
        HMat4f::Vec& up=m_modelview.getUpVector();

        HMat4f::Vec right=m_modelview.getRightVector();

        backward=~(position-point);
        right=~(up^backward);

        // Flip the "right" vector if its angle to the original vector is
        // greater than 90°.
        if (flip && right%m_modelview.getRightVector()<0) {
            right=-right;
        }

        up=backward^right;

        m_modelview.setRightVector(right);
        m_modelview_changed=true;
    }

    //@}

    /**
     * \name Relative transformations in the camera coordinate system
     */
    //@{

    /// Moves the camera right (for a positive \a distance) or left along its
    /// x-axis.
    void strafe(float const distance) {
        math::HMat4f::Vec& position=m_modelview.getPositionVector();
        math::HMat4f::Vec const& right=m_modelview.getRightVector();
        position+=right*distance;

        m_modelview_changed=true;
    }

    /// Moves the camera up (for a positive \a distance) or down along its
    /// y-axis.
    void elevate(float const distance) {
        math::HMat4f::Vec& position=m_modelview.getPositionVector();
        math::HMat4f::Vec const& up=m_modelview.getUpVector();
        position+=up*distance;

        m_modelview_changed=true;
    }

    /// Moves the camera backward (for a positive \a distance) or forward along
    /// its z-axis.
    void approach(float const distance) {
        math::HMat4f::Vec& position=m_modelview.getPositionVector();
        math::HMat4f::Vec const& backward=m_modelview.getBackwardVector();
        position+=backward*distance;

        m_modelview_changed=true;
    }

    /// Rotates the camera around the given \a axis about the given \a angle.
    void rotate(math::Vec3f const& axis,double const angle) {
        math::Quatf q(~axis,angle);

        // Normalize the vectors to avoid rounding errors.
        m_modelview.setRightVector(~(q*m_modelview.getRightVector()));
        m_modelview.setUpVector(~(q*m_modelview.getUpVector()));
        m_modelview.setBackwardVector(~(q*m_modelview.getBackwardVector()));

        m_modelview_changed=true;
    }

    /// Rotates the camera about its x-axis. For a positive \a angle, the camera
    /// will look up.
    void pitch(double const angle) {
        math::HMat4f::Vec const& right=m_modelview.getRightVector();
        rotate(right,angle);
    }

    /// Rotates the camera about its y-axis. For a positive \a angle, the camera
    /// will look left.
    void yaw(double const angle) {
        math::HMat4f::Vec const& up=m_modelview.getUpVector();
        rotate(up,angle);
    }

    /// Rotates the camera about its z-axis. For a positive \a angle, the camera
    /// will tilt left.
    void roll(double const angle) {
        math::HMat4f::Vec const& backward=m_modelview.getBackwardVector();
        rotate(backward,angle);
    }

    //@}

  protected:

    static Camera* s_current;       ///< Common pointer to the current camera.

    RenderSurface const* m_surface; ///< The surface that the camera is attached to.

    Frustum m_frustum;              ///< The camera's view frustum.

    ScreenSpace m_screen;           ///< The camera's current screen space.
    bool m_screen_changed;          ///< Dirty flag for the screen space.

    math::HMat4f m_modelview;       ///< Modelview transformation of the camera.
    bool m_modelview_changed;       ///< Dirty flag for the modelview transformation.

    math::Mat4d m_projection;       ///< Projection matrix of the camera.
    bool m_projection_changed;      ///< Dirty flag for projection matrix.
};

// Warning C4355: 'this' used in base member initializer list.
#pragma warning(default:4355)

#endif // G_OS_WINDOWS

} // namespace wrapgl

} // namespace gale

#endif // CAMERA_H
