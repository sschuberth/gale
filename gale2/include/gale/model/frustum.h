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

#ifndef FRUSTUM_H
#define FRUSTUM_H

/**
 * \file
 * Linear algebra plane routines
 */

#include "../wrapgl/camera.h"

#include "plane.h"

namespace gale {

namespace model {

/**
 * A camera frustum class for CPU-based frustum culling.
 */
class Frustum
{
  public:

    /// Constructs the frustum planes for the given \a camera.
    Frustum(wrapgl::Camera const& camera)
    :   m_camera(camera) {}

    /// Returns whether the given \a point is contained in the frustum.
    bool contains(math::Vec3f const& point);

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

    wrapgl::Camera const& m_camera; ///< Reference to the camera.
    Plane m_frustum[FACE_COUNT];    ///< Array of frustum planes.
};

} // namespace model

} // namespace gale

#endif // FRUSTUM_H
