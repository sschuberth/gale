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

#ifndef VECTOR_H
#define VECTOR_H

/**
 * \file
 * Linear algebra vector routines
 */

#include "tuple.h"

namespace gale {

namespace math {

/**
 * Vector class implementation based on a tuple, featuring common linear algebra
 * and math operations specialized on computer graphics.
 *
 * Example usage:
 * \code
 * glBegin(GL_LINE_LOOP);
 *     glVertex3fv(Vec3f::X());
 *     glVertex3fv(Vec3f::Y());
 *     glVertex3fv(Vec3f::Z());
 * glEnd();
 * \endcode
 */
template<unsigned int N,typename T>
class Vector:public TupleBase<N,T,Vector<N,T> >
{
    /// This type definition simplifies base class access to identifiers that
    /// are not visible until instantiation time because they do not dependent
    /// on template arguments.
    typedef TupleBase<N,T,Vector<N,T> > Base;

  public:

    /**
     * \name Predefined constants
     * In order to avoid the so called "static initialization order fiasco",
     * static methods instead of static variables are used here, see
     * <http://www.parashift.com/c++-faq-lite/ctors.html#faq-10.14>.
     */
    //@{

    /// Returns a vector which has all components set to 0.
    static Vector const& ZERO() {
        static Vector const v(0x0);
        return v;
    }

    /// Returns a vector which has the x-component set to 1, all others to 0.
    static Vector const& X() {
        static Vector const v(0x1);
        return v;
    }

    /// Returns a vector which has the y-component set to 1, all others to 0.
    static Vector const& Y() {
        static Vector const v(0x2);
        return v;
    }

    /// Returns a vector which has the z-component set to 1, all others to 0.
    static Vector const& Z() {
        static Vector const v(0x4);
        return v;
    }

    /// Returns a vector which has the w-component set to 1, all others to 0.
    static Vector const& W() {
        static Vector const v(0x8);
        return v;
    }

    //@}

    /**
     * \name Constructors
     */
    //@{

  protected:

    /// Sets each component in the vector to either 0 or 1 depending on the bits
    /// set in \a mask. Bit 0 maps to the first component, bit 1 to second one
    /// and so on. This constructor is required to initialize the static class
    /// constants.
    explicit Vector(unsigned int mask) {
        meta::LoopFwd<N,meta::OpAssign>::iterateIndexMask(Base::getData(),mask);
    }

  public:

    /// Create a vector whose components are either not initialized at all or
    /// initialized to 0 if \c GALE_INIT is defined.
    Vector() {}

    /// Allows to initialize 2-vectors directly.
    Vector(T const& x,T const& y):Base(x,y) {}

    /// Allows to initialize 3-vectors directly.
    Vector(T const& x,T const& y,T const& z):Base(x,y,z) {}

    /// Allows to initialize 4-vectors directly.
    Vector(T const& x,T const& y,T const& z,T const& w):Base(x,y,z,w) {}

    /// Converts a vector of different type but with the same amount of
    /// components to this type.
    template<typename U>
    Vector(Vector<N,U> const& v) {
        meta::LoopFwd<N,meta::OpAssign>::iterate(Base::getData(),v.getData());
    }

    //@}

    /**
     * \name Component access methods
     */
    //@{

    /// Returns a reference to the x-component.
    T& getX() {
        assert(N>=1);
        return Base::getData()[0];
    }

    /// Returns a \c constant reference to the x-component.
    T const& getX() const {
        assert(N>=1);
        return Base::getData()[0];
    }

    /// Assigns a new value to the x-component.
    void setX(T const& x) {
        assert(N>=1);
        Base::getData()[0]=x;
    }

    /// Returns a reference to the y-component.
    T& getY() {
        assert(N>=2);
        return Base::getData()[1];
    }

    /// Returns a \c constant reference to the y-component.
    T const& getY() const {
        assert(N>=2);
        return Base::getData()[1];
    }

    /// Assigns a new value to the y-component.
    void setY(T const& y) {
        assert(N>=2);
        Base::getData()[1]=y;
    }

    /// Returns a reference to the z-component.
    T& getZ() {
        assert(N>=3);
        return Base::getData()[2];
    }

    /// Returns a \c constant reference to the z-component.
    T const& getZ() const {
        assert(N>=3);
        return Base::getData()[2];
    }

    /// Assigns a new value to the z-component.
    void setZ(T const& z) {
        assert(N>=3);
        Base::getData()[2]=z;
    }

    /// Returns a reference to the w-component.
    T& getW() {
        assert(N>=4);
        return Base::getData()[3];
    }

    /// Returns a \c constant reference to the w-component.
    T const& getW() const {
        assert(N>=4);
        return Base::getData()[3];
    }

    /// Assigns a new value to the w-component.
    void setW(T const& w) {
        assert(N>=4);
        Base::getData()[3]=w;
    }

    //@}

    /**
     * \name Magnitude related methods
     */
    //@{

    /// Returns the squared Cartesian length of this vector.
    T getLengthSquared() const {
        return getDotProduct(*this);
    }

    /// Returns the Cartesian length of this vector.
    double getLength() const {
        return ::sqrt(static_cast<double>(getLengthSquared()));
    }

    /// Normalizes this vector so its length equals 1 (if it is not very small).
    Vector& normalize() {
        double length=getLength();
        if (length>std::numeric_limits<T>::epsilon()) {
            (*this)/=T(length);
        }
        return *this;
    }

    /// Returns whether all elements equal their counterpart in \a v with regard
    /// to the tolerance \a t.
    bool equals(Vector const& v,T t=T(1e-6)) {
        return (v-*this).getLengthSquared()<=t*t;
    }

    //@}

    /**
     * \name Angle related methods
     */
    //@{

    /// Calculates the cross product between this vector and vector \a v.
    Vector getCrossProduct(Vector const& v) const {
        assert(N==3);
        return Vector(
            Base::getData()[1]*v.getData()[2] - Base::getData()[2]*v.getData()[1],
            Base::getData()[2]*v.getData()[0] - Base::getData()[0]*v.getData()[2],
            Base::getData()[0]*v.getData()[1] - Base::getData()[1]*v.getData()[0]
        );
    }

    /// Calculates the dot product between this vector and vector \a v.
    T getDotProduct(Vector const& v) const {
        return meta::LoopFwd<N,meta::OpCalcProd>::iterateCombAdd(
            Base::getData(),
            v.getData()
        );
    }

    /// Returns the cosine of the angle between this vector and vector \a v.
    T getAngleCosine(Vector const& v) const {
        return (~(*this)).getDotProduct(~v);
    }

    /// Returns the angle between this vector and vector \a v in radians.
    double getAngle(Vector const& v) const {
        return ::acos(static_cast<double>(getAngleCosine(v)));
    }

    /// Returns a highly accurate calculation of the angle between this vector
    /// and vector \a v in radians.
    double getAccurateAngle(Vector const& v) const {
        Vector tn=~(*this),vn=~v;
        double dot=tn.getDotProduct(vn);
        return ::atan2(tn.getCrossProduct(vn).getLength(),dot);
    }

    /// Returns an arbitrary vector which is orthogonal to this vector.
    Vector getOrthoVector() const {
        // Try the x-axis to create an orthogonal vector.
        Vector v=getCrossProduct(Vector::X());

        // If the x-axis is (almost) collinear to this vector, take the y-axis.
        if (v.getLengthSquared()<=std::numeric_limits<T>::epsilon()) {
            v=getCrossProduct(Vector::Y());
        }

        return v;
    }

    /// Returns the projection of this vector onto the given vector \a v.
    Vector getProjectionOnto(Vector const& v) const {
        Vector const tmp=~v;
        return ((*this)%tmp)*tmp;
    }

    /// Returns whether this vector is collinear to vector \a v.
    bool isCollinear(Vector const& v) const {
        return meta::OpCmpEqualEps::evaluate(
            v.getCrossProduct(*this).getLengthSquared(),
            T(0)
        );
    }

    //@}

    /**
     * \name Convenience operators for named methods
     */
    //@{

    /// Returns a normalized copy of vector \a v.
    friend Vector operator~(Vector const& v) {
        return Vector(v).normalize();
    }

    /// Calculates the cross product between the vectors \a v and \a w.
    friend Vector operator^(Vector const& v,Vector const& w) {
        return v.getCrossProduct(w);
    }

    /// Calculates the dot product between the vectors \a v and \a w.
    friend T operator%(Vector const& v,Vector const& w) {
        return v.getDotProduct(w);
    }

    //@}
};

/**
 * \name Type definitions for use as OpenGL vertices
 */
//@{

typedef Vector<2,double> Vec2d;
typedef Vector<2,float> Vec2f;
typedef Vector<2,int> Vec2i;
typedef Vector<2,short> Vec2s;

typedef Vector<3,double> Vec3d;
typedef Vector<3,float> Vec3f;
typedef Vector<3,int> Vec3i;
typedef Vector<3,short> Vec3s;

typedef Vector<4,double> Vec4d;
typedef Vector<4,float> Vec4f;
typedef Vector<4,int> Vec4i;
typedef Vector<4,short> Vec4s;

//@}

} // namespace math

} // namespace gale

#endif // VECTOR_H
