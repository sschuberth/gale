/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
 *
 * Copyright (C) 2005-2008  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
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

#ifndef PLANE_H
#define PLANE_H

/**
 * \file
 * Linear algebra plane routines
 */

#include "vector.h"

namespace gale {

namespace math {

/**
 * Plane class implementation based on the equation required by glClipPlane():
 *
 *     1) A*x + B*y + C*z + D = 0
 *
 * Here, A, B, C are the components of the normal vector, and D is the negative
 * distance to the plane for any point x, y, z on the plane. Note that in the
 * Hessian Normal Form (HNF), D is changed in sign so it becomes the positive
 * distance to the plane:
 *
 *     2) A*x + B*y + C*z = D
 *
 * While internally equation 1) is used, all public methods assume the HNF, i.e.
 * equation 2) to be used, as it is more natural to deal with positive distances.
 *
 * Example usage:
 * \code
 * Plane p(Vec3d(0,1,-1),0.5);
 * glClipPlane(GL_CLIP_PLANE0,p);
 * \endcode
 */
class Plane
{
  public:

    /**
     * \name Constructors
     */
    //@{

    /// Constructs a plane from a \a normal and a \a distance to the origin.
    template<typename T>
    Plane(Vector<3,T> const& normal,T distance)
    :   m_normal(~normal),m_distance(-distance) {}

    /// Constructs a plane from three vectors \a a, \a b and \a c on that plane.
    template<typename T>
    Plane(Vector<3,T> const& a,Vector<3,T> const& b,Vector<3,T> const& c) {
        m_normal=~((b-a)^(c-a));
        m_distance=-a.dot(m_normal);
    }

    //@}

    /**
     * \name Element access methods
     */
    //@{

    /// Returns a pointer to the internal plane equation.
    double* data() {
        return m_normal;
    }

    /// Returns a constant pointer to the internal plane equation.
    double const* data() const {
        return m_normal;
    }

    /// Casts \c this plane to a double pointer, e.g. for use with glClipPlane().
    /// As an intended side effect, this also provides indexed data access.
    operator double*() {
        return data();
    }

    /// Casts \c this plane to a double const pointer, e.g. for use with glClipPlane().
    /// As an intended side effect, this also provides indexed data access.
    operator double const*() const {
        return data();
    }

    //@}

    /**
     * \name Component access methods
     */
    //@{

    /// Returns the plane's normal vector.
    Vec3d const& getNormal() const {
        return m_normal;
    }

    /// Sets the plane's \a normal vector.
    void setNormal(Vec3d const& normal) {
        m_normal=~normal;
    }

    /// Returns the plane's distance to the origin.
    double getDistance() const {
        return -m_distance;
    }

    /// Sets the plane's distance to the origin.
    void setDistance(double distance) {
        m_distance=-distance;
    }

    //@}

    /**
     * \name Miscellaneous methods
     */
    //@{

    /// Returns the distance of \a v to the plane along the normal vector.
    double distanceTo(Vec3d const& v) const {
        return (v%m_normal)+m_distance;
    }

    //@}

  protected:

    Vec3d m_normal;    ///< The plane's normal vector (A, B, C in the formula).
    double m_distance; ///< Negative distance to the origin (D in the formula).
};

} // namespace math

} // namespace gale

#endif // PLANE_H
