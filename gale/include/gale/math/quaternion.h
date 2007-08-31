/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at http://developer.berlios.de/projects/gale/
 *
 * Copyright (C) 2005-2007  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
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

#ifndef QUATERNION_H
#define QUATERNION_H

/**
 * \file
 * Linear algebra quaternion routines
 */

#include "hmatrix4.h"

namespace gale {

namespace math {

/**
 * Quaternion implementation based on a scalar for the real number part and a
 * vector for the imaginary number part. It has common linear algebra and math
 * operations specialized on computer graphics.
 *
 * Example usage:
 * \code
 * Quatf q(Vec3f::Z(),convDegToRad(45.0));
 * HMat4f m=q.getMatrix();
 * glMultMatrixf(m);
 * \endcode
 */
template<typename T>
class Quaternion
{
  public:

    /// Data type definition for external access to the template argument.
    typedef T Type;

    /// Definition of the type of vector used to store the imaginary number parts.
    typedef Vector<3,T> Vec;

    /// Definition of the type of matrix used to represent the rotation.
    typedef HMatrix4<T> HMat;

    /**
     * \name Predefined constants
     * In order to avoid the so called "static initialization order fiasco",
     * static methods instead of static variables are used here (see
     * http://www.parashift.com/c++-faq-lite/ctors.html#faq-10.14).
     */
    //@{

    /// Returns a quaternion which has all components set to 0 (which is the
    /// identity quaternion regarding addition).
    static Quaternion const& ZERO() {
        static Quaternion const q(0,Vec::ZERO());
        return q;
    }

    /// Returns the identity quaternion (regarding multiplication).
    static Quaternion const& IDENTITY() {
        static Quaternion const q(1,Vec::ZERO());
        return q;
    }

    //@}

    /**
     * \name Constructors
     */
    //@{

    /// For performance reasons, do not initialize any data by default.
    Quaternion() {}

    /// Initialized the quaternion to the given \a real number part and the
    /// imaginary number parts given as \a imag.
    Quaternion(T real,Vec const& imag):
      real(real),imag(imag) {}

    /// Creates a normalized quaternion representing the orientation given by a
    /// normalized \a axis vector and an \a angle in radians.
    Quaternion(Vec const& axis,double angle) {
        double half=angle*0.5;
        real=T(::cos(half));
        imag=T(::sin(half))*axis;
    }

    //@}

    /**
     * \name Element access methods
     */
    //@{

    /// Returns a pointer to the internal data representation.
    T* getData() {
        return reinterpret_cast<T*>(this);
    }

    /// Returns a \c constant pointer to the internal data representation.
    T const* getData() const {
        return reinterpret_cast<T*>(this);
    }

    /// Casts \c this quaternion to a pointer of type \a T. As an intended side
    /// effect, this also provides indexed data access.
    operator T*() {
        return getData();
    }

    /// Casts \c this quaternion to a pointer of type \a T \c const. As an
    /// intended side effect, this also provides indexed data access.
    operator T const*() const {
        return getData();
    }

    //@}

    /**
     * \name Arithmetic quaternion / quaternion operators
     */
    //@{

    /// Increments \c this quaternion by another quaternion \a q.
    Quaternion const& operator+=(Quaternion const& q) {
        real+=q.real;
        imag+=q.imag;
        return *this;
    }

    /// Decrements \c this quaternion by another quaternion \a q.
    Quaternion const& operator-=(Quaternion const& q) {
        real-=q.real;
        imag-=q.imag;
        return *this;
    }

    /// Multiplies \c this quaternion with quaternion \a q.
    Quaternion const& operator*=(Quaternion const& q) {
        return *this=(*this)*q;
    }

    /// Multiplies \c this quaternion with the inverse of quaternion \a q.
    Quaternion const& operator/=(Quaternion const& q) {
        return *this=(*this)/q;
    }

    /// Returns quaternion \a q unchanged; provided for convenience.
    friend Quaternion const& operator+(Quaternion const& q) {
        return q;
    }

    /// Returns the negation of quaternion \a q.
    friend Quaternion operator-(Quaternion const& q) {
        return Quaternion(-q.real,-q.imag);
    }

    /// Returns the sum of quaternions \a q and \a r.
    friend Quaternion operator+(Quaternion const& q,Quaternion const& r) {
        return Quaternion(q)+=r;
    }

    /// Returns the difference of quaternions \a q and \a r.
    friend Quaternion operator-(Quaternion const& q,Quaternion const& r) {
        return Quaternion(q)-=r;
    }

    /// Returns the product of quaternions \a q and \a r.
    friend Quaternion operator*(Quaternion const& q,Quaternion const& r) {
        // 16 scalar multiplications, 12 scalar additions.
        return Quaternion(
            q.real*r.real-(q.imag%r.imag),
            q.real*r.imag+q.imag*r.real+(q.imag^r.imag)
        );
    }

    /// Returns the product of quaternion \a q and the inverse of quaternion \a r.
    friend Quaternion operator/(Quaternion const& q,Quaternion const& r) {
        return q*(!r);
    }

    //@}

    /**
     * \name Arithmetic quaternion / vector operators
     */
    //@{

    /// Multiplies quaternion \a q with vector \a v. If the quaternion is
    /// normalized, this results in a rotation of \a v about \a q.
    friend Vec operator*(Quaternion const& q,Vec const& v) {
        // 18 scalar multiplications, 54 scalar additions.
        return (q*Quaternion(0,v)*q.getConjugate()).imag;
    }

    //@}

    /**
     * \name Arithmetic quaternion / scalar operators
     */
    //@{

    /// Multiplies \c this quaternion by a scalar \a s.
    Quaternion const& operator*=(T s) {
        real*=s;
        imag*=s;
        return *this;
    }

    /// Divides \c this quaternion by a scalar \a s.
    Quaternion const& operator/=(T s) {
        G_ASSERT(abs(s)>std::numeric_limits<T>::epsilon())
        return (*this)*=1/s;
    }

    /// Performs scalar multiplication from the right.
    friend Quaternion operator*(Quaternion const& q,T s) {
        return Quaternion(q)*=s;
    }

    /// Performs scalar multiplication from the left.
    friend Quaternion operator*(T s,Quaternion const& q) {
        return q*s;
    }

    /// Performs scalar division from the right.
    friend Quaternion operator/(Quaternion const& q,T s) {
        // The value of s is checked downstream in operator/=(T s).
        return Quaternion(q)/=s;
    }

    /// Performs scalar division from the left.
    friend Quaternion operator/(T s,Quaternion const& q) {
        return Quaternion(s/q.real,s/q.imag);
    }

    //@}

    /**
     * \name Element-wise comparison operators
     */
    //@{

    /// Returns whether all elements in \a q equal their counterpart in \a r
    /// with regard to a tolerance depending on the precision of data type \a T.
    friend bool operator==(Quaternion const& q,Quaternion const& r) {
        return meta::OpCmpEqualEps::evaluate(q.real,r.real) && q.imag==r.imag;
    }

    /// Returns whether the elements in \a q are not equal to their counterparts
    /// in \a r with regard to a tolerance depending on the precision of data
    /// type \a T.
    friend bool operator!=(Quaternion const& q,Quaternion const& r) {
        return !(q==r);
    }

    //@}

    /**
     * \name Magnitude related methods
     */
    //@{

    /// Returns the squared Cartesian length of this quaternion.
    T getLengthSquared() const {
        return getDotProduct(*this);
    }

    /// Returns the Cartesian length of this quaternion.
    double getLength() const {
        return ::sqrt(static_cast<double>(getLengthSquared()));
    }

    /// Normalizes this quaternion so its length equals 1 (if it is not very
    /// small).
    Quaternion& normalize() {
        double length=getLength();
        if (length>std::numeric_limits<T>::epsilon()) {
            (*this)/=T(length);
        }
        return *this;
    }

    //@}

    /**
     * \name Angle related methods
     */
    //@{

    /// Calculates the dot product between this quaternion and quaternion \a q.
    T getDotProduct(Quaternion const& q) const {
        return real*q.real+(imag%q.imag);
    }

    /// Returns the cosine of the angle between this quaternion and quaternion
    /// \a q.
    T getAngleCosine(Quaternion const& q) const {
        return (~(*this)).getDotProduct(~q);
    }

    /// Returns the angle between this quaternion and quaternion \a v in radians.
    double getAngle(Quaternion const& q) const {
        return ::acos(static_cast<double>(getAngleCosine(q)));
    }

    //@}

    /**
     * \name Convenience operators for named methods
     */
    //@{

    /// Returns a normalized copy of quaternion \a q.
    friend Quaternion operator~(Quaternion const& q) {
        return Quaternion(q).normalize();
    }

    /// Calculates the dot product between the quaternions \a q and \a r.
    friend T operator%(Quaternion const& q,Quaternion const& r) {
        return q.getDotProduct(r);
    }

    /// Returns an inverted copy of quaternion \a q.
    friend Quaternion operator!(Quaternion const& q) {
        return q.getConjugate()/q.getLengthSquared();
    }

    //@}

    /**
     * \name Interpolation methods
     * (see http://number-none.com/product/Understanding%20Slerp,%20Then%20Not%20Using%20It/)
     */
    //@{

    /// Performs a normalized linear interpolation between the quaternions \a q
    /// and \a r based on a scalar \a s. The interpolation is commutative,
    /// the rotation will not have constant velocity, but will be
    /// torque-minimal. For performance reasons, \a s is not clamped to [0,1].
    friend Quaternion nlerp(Quaternion const& q,Quaternion const& r,double s) {
        return ~Quaternion(
            T(q.real+s*(r.real-q.real)),
            lerp(q.imag,r.imag,s)
        );
    }

    /// Performs a spherical-linear interpolation between the quaternions \a q
    /// and \a r based on a scalar \a s. The interpolation is not commutative,
    /// the rotation will have constant velocity, and will be torque-minimal.
    /// For performance reasons, \a s is not clamped to [0,1].
    friend Quaternion slerp(Quaternion const& q,Quaternion const& r,double s) {
        T cos=q.getAngleCosine(r);

        if (meta::OpCmpEqualEps::evaluate(cos,T(1))) {
            // If the quaternions are very close just interpolate linearly.
            return nlerp(q,r,s);
        }

        double angle=::acos(cos);
        return (
            q * T(::sin(angle * (1-s)))
          + r * T(::sin(angle *    s ))
        )/T(::sin(angle));
    }

    /// Performs a spherical-cubic interpolation between the quaternions \a q
    /// and \a r based on a scalar \a s within the quadrilateral defined with
    /// quaternions \a a and \a b (the curve touches the midpoint of the "line"
    /// from \a a to \a b when \a s = 0.5). For performance reasons, \a s is not
    /// clamped to [0,1].
    friend Quaternion squad(Quaternion const& q,Quaternion const& r,double s,Quaternion const& a,Quaternion const& b) {
        return slerp(slerp(q,r,s),slerp(a,b,s),2*s*(1-s));
    }

    //@}

    /**
     * \name Miscellaneous methods
     */
    //@{

    /// Calculates the complex exponential of a quaternion \a q.
    friend Quaternion exp(Quaternion const& q) {
        double half=q.imag.getLength();
        double s=::sin(half);
        if (abs(s)>std::numeric_limits<T>::epsilon()) {
            return Quaternion(T(::cos(half)),q.imag*T(s/half));
        }
        return Quaternion(T(::cos(half)),q.imag);
    }

    /// Calculates the complex logarithm of a quaternion \a q.
    friend Quaternion log(Quaternion const& q) {
        if (abs(q.real)<1) {
            double half=::acos(q.real);
            double s=::sin(half);
            if (abs(s)>std::numeric_limits<T>::epsilon()) {
                return Quaternion(0,q.imag*T(half/s));
            }
        }
        return Quaternion(0,q.imag);
    }

    /// Returns the tangent for this quaternion given the predecessor \a a and
    /// the successor \a b, which all need to be normalized.
    Quaternion getTangent(Quaternion const& a,Quaternion const& b) const {
        // For normalized quaternions, the conjugate equals the inverse.
        Quaternion inv=getConjugate();
        return (*this)*exp(T(-0.25)*(log(inv*a)+log(inv*b)));
    }

    // Returns a homogeneous matrix that matches the rotation of this quaternion.
    HMat getMatrix() const {
        T wx,wy,wz,xx,xy,xz,yy,yz,zz;

        wx=real*imag.getX();
        wy=real*imag.getY();
        wz=real*imag.getZ();

        xx=imag.getX()*imag.getX();
        xy=imag.getX()*imag.getY();
        xz=imag.getX()*imag.getZ();

        yy=imag.getY()*imag.getY();
        yz=imag.getY()*imag.getZ();

        zz=imag.getZ()*imag.getZ();

        // This implicitly normalizes the quaternion.
        double s=2.0/getLength();

        return HMat(
            Vec(T(1-s*(yy+zz)), T(  s*(xy+wz)), T(  s*(xz-wy))),
            Vec(T(  s*(xy-wz)), T(1-s*(xx+zz)), T(  s*(yz+wx))),
            Vec(T(  s*(xz+wy)), T(  s*(yz-wx)), T(1-s*(xx+yy))),
            Vec::ZERO()
        );
    }

    /// Returns the conjugate of this quaternion.
    Quaternion getConjugate() const {
        return Quaternion(real,-imag);
    }

    /// Inverts this quaternion; the product of a quaternion and its inverse
    /// equals the identity quaternion.
    Quaternion& invert() {
        return (*this)=!(*this);
    }

    //@}

#ifndef GALE_TINY

    /**
     * \name Streaming input / output methods
     */
    //@{

    /// Reads tuple values from an input stream.
    friend std::istream& operator>>(std::istream& s,Quaternion& q) {
        return s >> q.real >> q.imag;
    }

    /// Writes tuple values to an output stream.
    friend std::ostream& operator<<(std::ostream& s,Quaternion const& q) {
        return s << '(' << q.real << ',' << q.imag << ')';
    }

    //@}

#endif // GALE_TINY

  public:

    T real;   ///< Real number part of the quaternion.
    Vec imag; ///< Imaginary number parts of the quaternion.
};

/**
 * \name Convenience type definitions
 */
//@{

typedef Quaternion<double> Quatd;
typedef Quaternion<float> Quatf;

//@}

} // namespace math

} // namespace gale

#endif // QUATERNION_H
