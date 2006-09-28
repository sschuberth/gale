/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at http://developer.berlios.de/projects/gale/
 *
 * Copyright (C) 2005-2006  Sebastian Schuberth <sschuberth@gmail.com>
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
 * Linear algebra vector class implementations.
 */

#include "tuple.h"

namespace gale {

namespace math {

/**
 * Vector class implementation based on a tuple, featuring common linear algebra
 * and specialized computer graphic math.
 *
 * Example usage:
 * \code
 * glBegin(GL_LINE_LOOP);
 *     glVertex3f(Vec3f::X());
 *     glVertex3f(Vec3f::Y());
 *     glVertex3f(Vec3f::Z());
 * glEnd();
 * \endcode
 */
template<unsigned int N,typename T>
class Vector:public TupleBase<N,T,Vector<N,T> > {
  public:
    /**
     * \name Predefined constants
     * In order to avoid the so called "static initialization order fiasco",
     * static methods instead of static variables are used here (see
     * http://www.parashift.com/c++-faq-lite/ctors.html#faq-10.14).
     */
    //@{
    /// Returns a vector which has all components set to 0.
    static Vector const& ZERO() {
        static Vector const v(N);
        return v;
    }

    /// Returns a vector which has the x-component set to 1, all others to 0.
    static Vector const& X() {
        static Vector const v(0);
        return v;
    }

    /// Returns a vector which has the y-component set to 1, all others to 0.
    static Vector const& Y() {
        static Vector const v(1);
        return v;
    }

    /// Returns a vector which has the z-component set to 1, all others to 0.
    static Vector const& Z() {
        static Vector const v(2);
        return v;
    }

    /// Returns a vector which has the w-component set to 1, all others to 0.
    static Vector const& W() {
        static Vector const v(3);
        return v;
    }
    //@}

    /**
     * \name Constructors
     */
    //@{
    /// For performance reasons, do not initialize any data by default.
    Vector() {
    }

    /// Sets all components to 0 except the one specified by \a index, which is
    /// set to 1. This constructor is required to initialize the constants.
    explicit Vector(unsigned int index) {
        meta::LoopFwd<N,meta::OpAssign>::iterateMatchIndex(getData(),index);
    }

    /// Allows to initialize 2-vectors directly.
    Vector(T const& x,T const& y):TupleBase<N,T,Vector>(x,y) {
    }

    /// Allows to initialize 3-vectors directly.
    Vector(T const& x,T const& y,T const& z):TupleBase<N,T,Vector>(x,y,z) {
    }

    /// Allows to initialize 4-vectors directly.
    Vector(T const& x,T const& y,T const& z,T const& w):TupleBase<N,T,Vector>(x,y,z,w) {
    }

    /// Conversion constructor to be able to use vectors of different types.
    template<typename U>
    Vector(const Vector<N,U>& v) {
        meta::LoopFwd<N,meta::OpAssign>::iterate(getData(),v.getData());
    }
    //@}

    /**
     * \name Component access methods
     */
    //@{
    /// Returns a reference to the x-component.
    T& getX() {
        G_ASSERT(N>=1)
        return m_data[0];
    }

    /// Returns a \c constant reference to the x-component.
    T const& getX() const {
        G_ASSERT(N>=1)
        return m_data[0];
    }

    /// Assigns a new value to the x-component.
    void setX(T const& x) {
        G_ASSERT(N>=1)
        m_data[0]=x;
    }

    /// Returns a reference to the y-component.
    T& getY() {
        G_ASSERT(N>=2)
        return m_data[1];
    }

    /// Returns a \c constant reference to the y-component.
    T const& getY() const {
        G_ASSERT(N>=2)
        return m_data[1];
    }

    /// Assigns a new value to the y-component.
    void setY(T const& y) {
        G_ASSERT(N>=2)
        m_data[1]=y;
    }

    /// Returns a reference to the z-component.
    T& getZ() {
        G_ASSERT(N>=3)
        return m_data[2];
    }

    /// Returns a \c constant reference to the z-component.
    T const& getZ() const {
        G_ASSERT(N>=3)
        return m_data[2];
    }

    /// Assigns a new value to the z-component.
    void setZ(T const& z) {
        G_ASSERT(N>=3)
        m_data[2]=z;
    }

    /// Returns a reference to the w-component.
    T& getW() {
        G_ASSERT(N>=4)
        return m_data[3];
    }

    /// Returns a \c constant reference to the w-component.
    T const& getW() const {
        G_ASSERT(N>=4)
        return m_data[3];
    }

    /// Assigns a new value to the w-component.
    void setW(T const& w) {
        G_ASSERT(N>=4)
        m_data[3]=w;
    }
    //@}

    /**
     * \name Magnitude related methods
     */
    //@{
    /// Returns the squared Cartesian length of this vector.
    T getLengthSquared() const {
        return dot(*this);
    }

    /// Returns the Cartesian length of this vector.
    double getLength() const {
        return std::sqrt(static_cast<double>(getLengthSquared()));
    }

    /// Normalizes this vector so its length equals 1 (if it is not very small).
    Vector const& normalize() {
        double length=getLength();
        if (length>std::numeric_limits<T>::epsilon())
            (*this)/=T(length);
        return *this;
    }
    //@}

    /**
     * \name Angle related methods
     */
    //@{
    /// Calculates the cross product between this vector and \a v.
    Vector cross(Vector const& v) const {
        G_ASSERT(N==3)
        return Vector(
            m_data[1]*v.m_data[2]-m_data[2]*v.m_data[1],
            m_data[2]*v.m_data[0]-m_data[0]*v.m_data[2],
            m_data[0]*v.m_data[1]-m_data[1]*v.m_data[0]
        );
    }

    /// Calculates the dot product between this vector and \a v.
    T dot(Vector const& v) const {
        return meta::LoopFwd<N,meta::OpCalcProd>::iterateCombAdd(getData(),v.getData());
    }

    /// Returns the cosine of the angle between this vector and \a v.
    T getAngleCosine(Vector const& v) const {
        return (~v).dot(~(*this));
    }

    /// Returns the angle between this vector and \a v in radians.
    double getAngle(Vector const& v) const {
        return std::acos(static_cast<double>(getAngleCosine(v)));
    }

    /// Returns a highly accurate calculation of the angle between this vector
    /// and \a v in radians.
    double getAccurateAngle(Vector const& v) const {
        Vector tn=~(*this),vn=~v;
        return std::atan2(tn.cross(vn).getLength(),static_cast<double>(tn.dot(vn)));
    }

    /// Returns a vector which is orthogonal to this vector.
    Vector getOrthoVector() const {
        // Try the x-axis to create an orthogonal vector.
        Vector v=cross(Vector::X());
        // If the x-axis is (almost) collinear to this vector, take the y-axis.
        if (v.getLengthSquared()<=std::numeric_limits<T>::epsilon())
            v=cross(Vector::Y());
        return v;
    }

    /// Returns whether this vector is collinear to \a v.
    bool isCollinear(Vector const& v) const {
        return meta::OpCmpEqualEps::evaluate(v.cross(*this).getLengthSquared(),0);
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
        return v.cross(w);
    }

    /// Calculates the dot product between the vectors \a v and \a w.
    friend T operator%(Vector const& v,Vector const& w) {
        return v.dot(w);
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
