/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
 *
 * Copyright (C) 2005-2011  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
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

#pragma once

/**
 * \file
 * Bounding box routines
 */

#include "../math/vector.h"

namespace gale {

namespace model {

/**
 * A data structure to represent an axis-aligned bounding box.
 */
struct AABB
{
    /// Array of the boxes' corner vertices.
    typedef math::Vec3f Vertices[8];

    /// Creates an empty box.
    AABB()
    :   min(math::Vec3f::ZERO())
    ,   max(math::Vec3f::ZERO())
    {}

    /// Returns the width of the box.
    float getWidth() const {
        return max.getX()-min.getX();
    }

    /// Sets the \a width of box; does not alter the center.
    void setWidth(float width) {
        width/=2;
        min.setX(-width);
        max.setX(+width);
    }

    /// Returns the height of the box.
    float getHeight() const {
        return max.getY()-min.getY();
    }

    /// Sets the \a height of box; does not alter the center.
    void setHeight(float height) {
        height/=2;
        min.setY(-height);
        max.setY(+height);
    }

    /// Returns the depth of the box.
    float getDepth() const {
        return max.getZ()-min.getZ();
    }

    /// Sets the \a depth of box; does not alter the center.
    void setDepth(float depth) {
        depth/=2;
        min.setZ(-depth);
        max.setZ(+depth);
    }

    /// Checks whether the box is empty, i.e. has no volume.
    bool isEmpty() const {
        return meta::OpCmpEqual::evaluate(min.getX(),max.getX())
            || meta::OpCmpEqual::evaluate(min.getY(),max.getY())
            || meta::OpCmpEqual::evaluate(min.getZ(),max.getZ());
    }

    /// Returns whether the given \a point is contained in the box.
    bool contains(math::Vec3f const& point) const {
        return min<=point && point<=max;
    }

    /// Returns the center of the box.
    math::Vec3f center() const {
        return (min+max)*0.5f;
    }

    /// Returns the extent of the box.
    math::Vec3f extent() const {
        return (max-min)*0.5f;
    }

    /// Sets the vertices in \a v to the corner vertices of the box.
    void vertices(Vertices& v) const {
        v[0]=min;
        v[1]=math::Vec3f(min.getX(),min.getY(),max.getZ());
        v[2]=math::Vec3f(min.getX(),max.getY(),min.getZ());
        v[3]=math::Vec3f(min.getX(),max.getY(),max.getZ());
        v[4]=math::Vec3f(max.getX(),min.getY(),min.getZ());
        v[5]=math::Vec3f(max.getX(),min.getY(),max.getZ());
        v[6]=math::Vec3f(max.getX(),max.getY(),min.getZ());
        v[7]=max;
    }

    math::Vec3f min; ///< Minimum coordinates of the box.
    math::Vec3f max; ///< Maximum coordinates of the box.
};

} // namespace model

} // namespace gale
