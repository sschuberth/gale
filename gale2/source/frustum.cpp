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

bool Frustum::isInside(Vec3f const& point)
{
    Vec3f v=m_camera.getModelview()*point;

    // Recalculate the frustum planes if the camera's projection matrix has
    // changed.
    if (m_camera.hasProjectionChanged()) {
        calculate();
    }

    // To be inside, the point has to lie in front of all frustum planes.
    for (int i=5;i>=0;--i) {
        if (m_frustum[i].distanceTo(v)<0) {
            return false;
        }
    }

    return true;
}

void Frustum::calculate()
{
    Mat4d const& projection=m_camera.getProjection();

    // All frustum planes face inwards.

    m_frustum[FACE_LEFT]=Plane(
        Vec3d(
            projection[3]+projection[0],
            projection[7]+projection[4],
            projection[11]+projection[8]
        ),
        projection[15]+projection[12]
    );

    m_frustum[FACE_RIGHT]=Plane(
        Vec3d(
            projection[3]-projection[0],
            projection[7]-projection[4],
            projection[11]-projection[8]
        ),
        projection[15]-projection[12]
    );

    m_frustum[FACE_BOTTOM]=Plane(
        Vec3d(
            projection[3]+projection[1],
            projection[7]+projection[5],
            projection[11]+projection[9]
        ),
        projection[15]+projection[13]
    );

    m_frustum[FACE_TOP]=Plane(
        Vec3d(
            projection[3]-projection[1],
            projection[7]-projection[5],
            projection[11]-projection[9]
        ),
        projection[15]-projection[13]
    );

    m_frustum[FACE_ZNEAR]=Plane(
        Vec3d(
            projection[3]+projection[2],
            projection[7]+projection[6],
            projection[11]+projection[10]
        ),
        projection[15]+projection[14]
    );

    m_frustum[FACE_ZFAR]=Plane(
        Vec3d(
            projection[3]-projection[2],
            projection[7]-projection[6],
            projection[11]-projection[10]
        ),
        projection[15]-projection[14]
    );
}

} // namespace model

} // namespace gale
