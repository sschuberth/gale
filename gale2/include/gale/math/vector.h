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
 * OpenGL example usage:
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
    explicit Vector(unsigned int const mask) {
        meta::LoopFwd<N,meta::OpAssign>::iterateIndexMask(Base::data(),mask);
    }

  public:

    /// Create a vector whose components are either not initialized at all or
    /// initialized to 0 if \c GALE_INIT_DATA is defined.
    Vector() {}

    /// Allows to initialize 2-vectors directly.
    Vector(T const x,T const y)
    :   Base(x,y) {}

    /// Allows to initialize 3-vectors directly.
    Vector(T const x,T const y,T const z)
    :   Base(x,y,z) {}

    /// Allows to initialize 4-vectors directly.
    Vector(T const x,T const y,T const z,T const w)
    :   Base(x,y,z,w) {}

    /// Converts a vector of different type but with the same amount of
    /// components to this type.
    template<typename U>
    Vector(Vector<N,U> const& v) {
        meta::LoopFwd<N,meta::OpAssign>::iterate(Base::data(),v.data());
    }

    //@}

    /**
     * \name Component access methods
     */
    //@{

    /// Returns the x-component.
    T getX() {
        return Base::data()[0];
    }

    /// Returns a constant reference to the x-component.
    T const& getX() const {
        return Base::data()[0];
    }

    /// Assigns a new value to the x-component.
    void setX(T const& x) {
        Base::data()[0]=x;
    }

    /// Returns the y-component.
    T getY() {
        return Base::data()[1];
    }

    /// Returns a constant reference to the y-component.
    T const& getY() const {
        return Base::data()[1];
    }

    /// Assigns a new value to the y-component.
    void setY(T const& y) {
        Base::data()[1]=y;
    }

    /// Returns the z-component.
    T getZ() {
        G_ASSERT(N>=3)
        return Base::data()[2];
    }

    /// Returns a constant reference to the z-component.
    T const& getZ() const {
        G_ASSERT(N>=3)
        return Base::data()[2];
    }

    /// Assigns a new value to the z-component.
    void setZ(T const& z) {
        G_ASSERT(N>=3)
        Base::data()[2]=z;
    }

    /// Returns the w-component.
    T getW() {
        G_ASSERT(N>=4)
        return Base::data()[3];
    }

    /// Returns a constant reference to the w-component.
    T const& getW() const {
        G_ASSERT(N>=4)
        return Base::data()[3];
    }

    /// Assigns a new value to the w-component.
    void setW(T const& w) {
        G_ASSERT(N>=4)
        Base::data()[3]=w;
    }

    //@}

    /**
     * \name Magnitude related methods
     */
    //@{

    /// Returns the squared Cartesian length of this vector.
    T length2() const {
        return dot(*this);
    }

    /// Returns the Cartesian length of this vector.
    double length() const {
        return sqrt(static_cast<double>(length2()));
    }

    /// Normalizes this vector so its length equals 1 (if it is not very small).
    /// Returns the length before normalization.
    double normalize() {
        static double const t=Numerics<T>::ZERO_TOLERANCE()*Numerics<T>::ZERO_TOLERANCE();
        double l=length2();

        if (!meta::OpCmpEqual::evaluate(l,1.0) && l>t) {
            l=sqrt(l);
            (*this)/=T(l);
        }

        return l;
    }

    /// Returns whether all elements equal their counterpart in \a v with regard
    /// to the tolerance \a t.
    bool equals(Vector const& v,T const t=T(1e-6)) {
        return (v-*this).length2()<=t*t;
    }

    //@}

    /**
     * \name Angle related methods
     */
    //@{

    /// Returns the dot product between vectors \a v and \a w.
    T dot(Vector const& v) const {
        // N scalar muls/divs, N-1 scalar adds/subs.
        return meta::LoopFwd<N,meta::OpCalcProd>::iterateCombAdd(
            Base::data()
        ,   v.data()
        );
    }

    /// Returns the cosine of the angle between this vector and vector \a v.
    T angleCosine(Vector const& v) const {
        return (~(*this))%(~v);
    }

    /// Returns the smallest angle between this vector and vector \a v in radians.
    double angle(Vector const& v) const {
        return acos(static_cast<double>(angleCosine(v)));
    }

    /// Returns the oriented angle between this vector and vector \a v in radians.
    /// The angle is measured in positive rotation direction around the reference
    /// vector \a r.
    double orientedAngle(Vector const& v,Vector const& r) const {
        Vector tn=~(*this),vn=~v;

        // Calculate the normal between the two angle vectors and project the
        // reference vector onto it.
        Vector n=(*this)^v;
        T d=r%n;

        if (d==0 && tn+vn==Vector::ZERO()) {
            // If the two normalized angle vectors compensate each other,
            // they point to opposite directions, and the angle should
            // be exactly 180 degrees.
            return Constd::PI();
        }

        // Calculate the unoriented angle between the two angle vectors.
        double a=acos(static_cast<double>(tn%vn));

        if (d<0) {
            // If d is negative, r projects onto the negative direction of n,
            // i.e. it points to the other half space with respect to the
            // plane spanned by the two angle vectors, and the angle amounts
            // to what is missing to a full circle.
            a=2*Constd::PI()-a;
        }

        return a;
    }

    /// Returns the orthogonal projection of this vector onto the vector \a v.
    Vector orthoProjection(Vector const& v) const {
        return dot(v)/v.length2()*v;
    }

    /// Calculates the cross product between this vector and vector \a v.
    void crossWith(Vector const& v) {
        // 6 scalar muls/divs, 3 scalar adds/subs.
        G_ASSERT(N==3)
        T x = getY()*v.getZ() - getZ()*v.getY();
        T y = getZ()*v.getX() - getX()*v.getZ();
        T z = getX()*v.getY() - getY()*v.getX();
        set(x,y,z);
    }

    /// Returns an arbitrary vector which is orthogonal to this vector.
    Vector orthoVector() const {
        // Try the x-axis to create an orthogonal vector.
        Vector v=(*this)^Vector::X();

        // If the x-axis is (almost) collinear to this vector, take the y-axis.
        if (v.length2()<=Numerics<T>::ZERO_TOLERANCE()) {
            v=(*this)^Vector::Y();
        }

        return v;
    }

    /// Returns whether this vector is collinear to vector \a v.
    bool isCollinear(Vector const& v) const {
        return meta::OpCmpEqual::evaluate(
            (v^(*this)).length2()
        ,   T(0)
        );
    }

    //@}

    /**
     * \name Convenience operators for named methods
     */
    //@{

    /// Returns a normalized copy of vector \a v.
    friend Vector operator~(Vector const& v) {
        Vector tmp=v;
        tmp.normalize();
        return tmp;
    }

    /// Returns the dot product between vectors \a v and \a w.
    friend T operator%(Vector const& v,Vector const& w) {
        // N scalar muls/divs, N-1 scalar adds/subs.
        return v.dot(w);
    }

    /// Returns the cross product between vectors \a v and \a w.
    friend Vector operator^(Vector const& v,Vector const& w) {
        // 6 scalar muls/divs, 3 scalar adds/subs.
        G_ASSERT(N==3)
        return Vector(
            v.getY()*w.getZ() - v.getZ()*w.getY()
        ,   v.getZ()*w.getX() - v.getX()*w.getZ()
        ,   v.getX()*w.getY() - v.getY()*w.getX()
        );
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
