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

#ifndef QUATERNION_H
#define QUATERNION_H

/**
 * \file
 * Linear algebra quaternion routines
 */

#include "hmatrix4.h"

namespace gale {

namespace math {

// Make sure data members are tightly packed.
#pragma pack(push,1)

/**
 * Quaternion implementation based on a scalar for the real number part and a
 * vector for the imaginary number part. It has common linear algebra and math
 * operations specialized on computer graphics.
 *
 * OpenGL example usage:
 * \code
 * Quatf q(Vec3f::Z(),45.0f*Constf::DEG_TO_RAD());
 * HMat4f m=q.getMatrix();
 * glMultMatrixf(m);
 * \endcode
 */
template<typename T>
class Quaternion
{
  public:

    /// Definition for external access to the data type.
    typedef T Type;

    /// Definition of the type of vector used to store the imaginary number parts.
    typedef Vector<3,T> Vec;

    /// Definition of the type of matrix used to represent the rotation.
    typedef HMatrix4<T> HMat;

    /// Function pointer definition for interpolating quaternions.
    typedef Quaternion (*Interpolator)(Quaternion const&,Quaternion const&,double);

    /**
     * \name Predefined constants
     * In order to avoid the so called "static initialization order fiasco",
     * static methods instead of static variables are used here, see
     * <http://www.parashift.com/c++-faq-lite/ctors.html#faq-10.14>.
     */
    //@{

    /// Returns a quaternion which has all components set to 0 (which is the
    /// identity quaternion regarding addition).
    static Quaternion const& ZERO() {
        static Quaternion const q(0,Vec::ZERO());
        return q;
    }

    /// Returns a quaternion with the real part set to 1 and the imaginary part
    /// set to 0 (which is the identity quaternion regarding multiplication).
    static Quaternion const& IDENTITY() {
        static Quaternion const q(1,Vec::ZERO());
        return q;
    }

    //@}

    /**
     * \name Constructors
     */
    //@{

    /// Create a quaternion whose components are either not initialized at all
    /// or initialized to 0 if \c GALE_INIT_DATA is defined.
    Quaternion() {
#ifdef GALE_INIT_DATA
        real=0;
#endif
    }

    /// Initialized the quaternion to the given \a real number part and the
    /// imaginary number parts given as \a imag.
    Quaternion(T const real,Vec const& imag)
    :   real(real),imag(imag) {}

    /// Creates a quaternion that matches the rotation represented by the given
    /// normalized rotation \a axis vector and rotation \a angle in radians.
    Quaternion(Vec const& axis,double const angle) {
        setAxisAngle(axis,angle);
    }

    /// Creates a quaternion that matches the rotation represented by the given
    /// homogeneous matrix \a m, which needs to be orthonormalized.
    Quaternion(HMat const& m) {
        setMatrix(m);
    }

    //@}

    /**
     * \name Element access methods
     */
    //@{

    /// Returns a pointer to the internal data representation.
    T* data() {
        return reinterpret_cast<T*>(this);
    }

    /// Returns a constant pointer to the internal data representation.
    T const* data() const {
        return reinterpret_cast<T*>(this);
    }

    /// Casts \c this quaternion to a pointer of type \a T. As an intended side
    /// effect, this also provides indexed data access.
    operator T*() {
        return data();
    }

    /// Casts \c this quaternion to a pointer of type \a T \c const. As an
    /// intended side effect, this also provides indexed data access.
    operator T const*() const {
        return data();
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
        // 12 scalar muls/divs, 8 scalar adds/subs.
        return Quaternion(
            q.real*r.real - (q.imag%r.imag)
        ,   q.real*r.imag + q.imag*r.real + (q.imag^r.imag)
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
        // 18 scalar muls/divs, 11 scalar adds/subs.
        Vec i = q.real*v + (q.imag^v);
        return (q.imag%v)*q.imag + i*q.real - (i^q.imag);
    }

    //@}

    /**
     * \name Arithmetic quaternion / scalar operators
     */
    //@{

    /// Multiplies \c this quaternion by a scalar \a s.
    Quaternion const& operator*=(T const s) {
        real*=s;
        imag*=s;
        return *this;
    }

    /// Divides \c this quaternion by a scalar \a s.
    Quaternion const& operator/=(T const s) {
        G_ASSERT(abs(s)>Numerics<T>::ZERO_TOLERANCE())
        return (*this)*=1/s;
    }

    /// Performs scalar multiplication from the right.
    friend Quaternion operator*(Quaternion const& q,T const s) {
        return Quaternion(q)*=s;
    }

    /// Performs scalar multiplication from the left.
    friend Quaternion operator*(T const s,Quaternion const& q) {
        return q*s;
    }

    /// Performs scalar division from the right.
    friend Quaternion operator/(Quaternion const& q,T const s) {
        // The value of s is checked downstream in operator/=(T s).
        return Quaternion(q)/=s;
    }

    /// Performs scalar division from the left.
    friend Quaternion operator/(T const s,Quaternion const& q) {
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
        return meta::OpCmpEqual::evaluate(q.real,r.real) && q.imag==r.imag;
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
    T length2() const {
        return dot(*this);
    }

    /// Returns the Cartesian length of this quaternion.
    double length() const {
        return ::sqrt(static_cast<double>(length2()));
    }

    /// Normalizes this quaternion so its length equals 1 (if it is not very
    /// small). Returns the length before normalization.
    double normalize() {
        double l=length2();

        if (!meta::OpCmpEqual::evaluate(l,1.0) && l>Numerics<T>::ZERO_TOLERANCE()*Numerics<T>::ZERO_TOLERANCE()) {
            l=::sqrt(l);
            (*this)/=T(l);
        }

        return l;
    }

    //@}

    /**
     * \name Angle related methods
     */
    //@{

    /// Calculates the dot product between this quaternion and quaternion \a q.
    T dot(Quaternion const& q) const {
        return real*q.real+(imag%q.imag);
    }

    /// Returns the cosine of the angle between this quaternion and quaternion
    /// \a q, which do not need to be normalized.
    T angleCosine(Quaternion const& q) const {
        return (~(*this))%(~q);
    }

    /// Returns the angle between this quaternion and quaternion \a v, which do
    /// not need to be normalized, in radians.
    double angle(Quaternion const& q) const {
        return ::acos(static_cast<double>(angleCosine(q)));
    }

    //@}

    /**
     * \name Convenience operators for named methods
     */
    //@{

    /// Returns a normalized copy of quaternion \a q.
    friend Quaternion operator~(Quaternion const& q) {
        Quaternion tmp=q;
        tmp.normalize();
        return tmp;
    }

    /// Returns the dot product between quaternions \a q and \a r.
    friend T operator%(Quaternion const& q,Quaternion const& r) {
        return q.dot(r);
    }

    /// Returns the inverse of quaternion \a q.
    friend Quaternion operator!(Quaternion const& q) {
        Quaternion tmp=q;
        tmp.invert();
        return tmp;
    }

    //@}

    /**
     * \name Interpolation related methods, see
     * <http://number-none.com/product/Understanding%20Slerp,%20Then%20Not%20Using%20It/>
     */
    //@{

    /// Performs a normalized linear interpolation between the quaternions \a q
    /// and \a r based on a scalar \a s with the following properties:
    /// - The operation is commutative,
    /// - the rotation does \b not have constant speed,
    /// - the interpolation is torque-minimal.
    /// For performance reasons, \a s is not clamped to [0,1].
    friend Quaternion nlerp(Quaternion const& q,Quaternion const& r,double const s) {
        return ~Quaternion(
            T(q.real+s*(r.real-q.real))
        ,   lerp(q.imag,r.imag,s)
        );
    }

    /// Performs a spherical-linear interpolation between the quaternions \a q
    /// and \a r based on a scalar \a s with the following properties:
    /// - The operation is \b not commutative,
    /// - the rotation has constant speed (tangential acceleration is zero),
    /// - the interpolation is torque-minimal.
    /// For performance reasons, \a s is not clamped to [0,1].
    friend Quaternion slerp(Quaternion const& q,Quaternion const& r,double const s) {
        T cosine=q.angleCosine(r);

        if (meta::OpCmpEqual::evaluate(cosine,T(1))) {
            // If the quaternions are very close just interpolate linearly.
            return nlerp(q,r,s);
        }

        // Calculate the angle between q and r.
        double angle=::acos(cosine);

        // Calculate the angle between q and the interpolated result.
        double theta=angle*s;

        return (
            q * T(::sin(angle-theta))
          + r * T(::sin(      theta))
        )/T(::sin(angle));
    }

    /// Calculates the complex exponential of a quaternion \a q.
    friend Quaternion exp(Quaternion const& q) {
        double l=q.imag.length();
        if (l>Numerics<T>::ZERO_TOLERANCE()) {
            double s=::sin(l);
            return Quaternion(T(::cos(l)),q.imag*T(s/l));
        }
        return Quaternion(1,Vec::ZERO());
    }

    /// Calculates the complex logarithm of a quaternion \a q.
    friend Quaternion log(Quaternion const& q) {
        if (abs(q.real)<1) {
            double half=::acos(q.real);
            double s=::sin(half);
            if (abs(s)>Numerics<T>::ZERO_TOLERANCE()) {
                return Quaternion(0,q.imag*T(half/s));
            }
        }
        return Quaternion(0,Vec::ZERO());
    }

    /// Performs a cubic Bzier interpolation between the quaternions \a q and
    /// \a r based on a scalar \a s within the quadrilateral defined
    /// together with the "tangent" quaternions \a a and \a b (the curve touches
    /// the midpoint of the "line" from \a a to \a b when \a s = 0.5).
    /// Optionally, an \a interpolator may be specified. For performance
    /// reasons, \a s is not clamped to [0,1].
    friend Quaternion squad(
        Quaternion const& q
    ,   Quaternion const& r
    ,   double const s
    ,   Quaternion const& a
    ,   Quaternion const& b
    ,   Interpolator const interpolator=Quaternion::slerp
    )
    {
        return interpolator(interpolator(q,r,s),interpolator(a,b,s),2*s*(1-s));
    }

    //@}

    /**
     * \name Miscellaneous methods
     */
    //@{

    /// Sets this quaternion to match the rotation represented by the given
    /// normalized rotation \a axis vector and rotation \a angle in radians.
    void setAxisAngle(Vec const& axis,double const angle) {
        double half=0.5*angle;
        real=T(::cos(half));
        imag=T(::sin(half))*axis;
    }

    /// Returns a rotation \a axis vector and rotation \a angle in radians that
    /// match the rotation represented by this quaternion, which needs to be
    /// normalized.
    void getAxisAngle(Vec& axis,double& angle) {
        T dot=imag.length2();

        if (dot>Numerics<T>::ZERO_TOLERANCE()) {
            angle=::acos(real)*2.0;
            axis=imag/T(::sqrt(static_cast<double>(dot)));
        }
        else {
            angle=0;
            axis=Vec::X();
        }
    }

    /// Sets this quaternion to match the rotation represented by the given
    /// homogeneous matrix \a m, which needs to be orthonormalized.
    void setMatrix(HMat const& m) {
        T trace=m(0,0)+m(1,1)+m(2,2);

        if (trace>0) {
            T r=::sqrt(trace+1);
            T s=T(0.5)/r;

            real    = T(0.5)*r;
            imag[0] = (m(2,1)-m(1,2))*s;
            imag[1] = (m(0,2)-m(2,0))*s;
            imag[2] = (m(1,0)-m(0,1))*s;
        }
        else {
            unsigned int i=0;

            if (m(1,1)>m(0,0)) {
                i=1;
            }
            if (m(2,2)>m(i,i)) {
                i=2;
            }

            unsigned int j=(i+1)%3;
            unsigned int k=(j+1)%3;

            T r=::sqrt(m(i,i)-m(j,j)-m(k,k)+1);
            T s=T(0.5)/r;

            real    = (m(k,j)-m(j,k))*s;
            imag[i] = T(0.5)*r;
            imag[j] = (m(j,i)+m(i,j))*s;
            imag[k] = (m(k,i)+m(i,k))*s;
        }
    }

    /// Returns a homogeneous matrix \a m that matches the rotation represented
    /// by this quaternion, which needs to be normalized.
    void getMatrix(HMat& m) const {
        T x,y,z;
        T wx,wy,wz,xx,xy,xz,yy,yz,zz;

        x=2*imag.getX();
        y=2*imag.getY();
        z=2*imag.getZ();

        wx=x*real;
        wy=y*real;
        wz=z*real;

        xx=x*imag.getX();
        xy=y*imag.getX();
        xz=z*imag.getX();

        yy=y*imag.getY();
        yz=z*imag.getY();

        zz=z*imag.getZ();

        m.c0=Vec(T(1-(yy+zz)), T(  (xy+wz)), T(  (xz-wy)));
        m.c1=Vec(T(  (xy-wz)), T(1-(xx+zz)), T(  (yz+wx)));
        m.c2=Vec(T(  (xz+wy)), T(  (yz-wx)), T(1-(xx+yy)));
        m.c3=Vec::ZERO();
    }

    /// Returns the tangent for this quaternion given the predecessor \a a and
    /// the successor \a b, which all need to be normalized.
    Quaternion tangent(Quaternion const& a,Quaternion const& b) const {
        // For normalized quaternions, the conjugate equals the inverse.
        Quaternion inv=*this;
        inv.conjugate();

        Quaternion t=T(-0.25)*(log(inv*a)+log(inv*b));
        return (*this)*exp(t);
    }

    /// Conjugates this quaternion, i.e. negates its imaginary part.
    void conjugate() {
        imag=-imag;
    }

    /// Inverts this quaternion; the product of a quaternion and its inverse
    /// equals the identity quaternion.
    void invert() {
        conjugate();
        (*this)/=length2();
    }

    //@}

#ifndef GALE_TINY_CODE

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

#endif // GALE_TINY_CODE

  public:

    T real;   ///< Real number part of the quaternion.
    Vec imag; ///< Imaginary number parts of the quaternion.
};

#pragma pack(pop)

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
