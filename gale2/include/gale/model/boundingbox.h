#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

/**
 * \file
 * Bounding box routines
 */

#include "../math/vector.h"

namespace gale {

namespace model {

/**
 * A class to represent an axis-aligned bounding box.
 */
class AABB
{
  public:

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
        return min.getX()<=point.getX() && point.getX()<=max.getX()
            && min.getY()<=point.getY() && point.getY()<=max.getY()
            && min.getZ()<=point.getZ() && point.getZ()<=max.getZ();
    }

    /// Returns the center of the box.
    math::Vec3f center() const {
        return math::Vec3f(min.getX()+max.getX(),min.getY()+max.getY(),min.getZ()+max.getZ())*0.5f;
    }

    math::Vec3f min; ///< Minimum coordinates of the box.
    math::Vec3f max; ///< Maximum coordinates of the box.
};

} // namespace model

} // namespace gale

#endif // BOUNDINGBOX_H
