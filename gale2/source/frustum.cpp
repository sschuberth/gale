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

#include "gale/model/frustum.h"

using namespace gale::math;

namespace gale {

namespace model {

Frustum::Frustum(wrapgl::Camera const& camera)
:   m_camera(camera)
{
    // Initialize the frustum planes.
    calculate();
}

bool Frustum::contains(Vec3f const& point)
{
    Vec3f v=!m_camera.getModelview()*point;

    // Recalculate the frustum planes if the camera's projection matrix has
    // changed.
    if (m_camera.hasProjectionChanged()) {
        calculate();
    }

    // To be inside, the point has to lie in front of all frustum planes.
    for (int i=0;i<G_ARRAY_LENGTH(m_frustum)-1;++i) {
        if (m_frustum[i].distanceTo(v)<0) {
            return false;
        }
    }

    return true;
}

bool Frustum::contains(AABB const& box)
{
    // Get the boxes corner vertices.
    AABB::Vertices v;
    box.vertices(v);

    return contains(v);
}

bool Frustum::contains(AABB::Vertices const& box)
{
    for (int i=0;i<G_ARRAY_LENGTH(box);++i) {
        // The box is (partly) contained if at least one of its corner vertices
        // is contained.
        if (contains(box[i])) {
            return true;
        }
    }

    return false;
}

void Frustum::calculate()
{
    Mat4d const& projection=m_camera.getProjection();

    Vec3d r0(projection[ 0], projection[ 4], projection[ 8]);
    Vec3d r1(projection[ 1], projection[ 5], projection[ 9]);
    Vec3d r2(projection[ 2], projection[ 6], projection[10]);
    Vec3d r3(projection[ 3], projection[ 7], projection[11]);

    // All frustum planes face inwards.

    m_frustum[ FACE_LEFT   ] = Plane(Vec3d(r3 + r0), projection[15] + projection[12]);
    m_frustum[ FACE_RIGHT  ] = Plane(Vec3d(r3 - r0), projection[15] - projection[12]);
    m_frustum[ FACE_BOTTOM ] = Plane(Vec3d(r3 + r1), projection[15] + projection[13]);
    m_frustum[ FACE_TOP    ] = Plane(Vec3d(r3 - r1), projection[15] - projection[13]);
    m_frustum[ FACE_ZNEAR  ] = Plane(Vec3d(r3 + r2), projection[15] + projection[14]);
    m_frustum[ FACE_ZFAR   ] = Plane(Vec3d(r3 - r2), projection[15] - projection[14]);
}

} // namespace model

} // namespace gale
