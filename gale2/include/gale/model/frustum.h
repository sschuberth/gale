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

    Frustum(wrapgl::Camera const& camera)
    :   m_camera(camera) {}

    bool contains(math::Vec3f const& point);

  private:

    enum Face {
        FACE_LEFT,
        FACE_RIGHT,
        FACE_BOTTOM,
        FACE_TOP,
        FACE_ZNEAR,
        FACE_ZFAR,
        FACE_COUNT
    };

    void calculate();

    wrapgl::Camera const& m_camera;
    Plane m_frustum[FACE_COUNT];
};

} // namespace model

} // namespace gale

#endif // FRUSTUM_H
