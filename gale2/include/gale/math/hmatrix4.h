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

#ifndef HMATRIX4_H
#define HMATRIX4_H

/**
 * \file
 * Linear algebra homogeneous matrix routines
 */

#include "vector.h"

namespace gale {

namespace math {

// Make sure data members are tightly packed.
#pragma pack(push,1)

/**
 * Homogeneous matrix class implementation based on column vectors. The matrix
 * is stored as 4x4 numbers organized in column-major order in memory as
 * required by OpenGL, i.e. the matrix entry at row r (0 <= r <= 3) and column
 * c (0 <= c <= 3) is located at offset i = c*4 + r.
 *
 * OpenGL example usage:
 * \code
 * Quatf q(Vec3f::Z(),convDegToRad(45.0));
 * HMat4f m=q.getMatrix();
 * glMultMatrixf(m);
 * \endcode
 */
template<typename T>
class HMatrix4
{
  public:

    /// Definition for external access to the data type.
    typedef T Type;

    /// Definition of the type of vector used as the columns for the matrix.
    typedef Vector<3,T> Vec;

    /// %Factory methods to easily create commonly used homogeneous matrices.
    struct Factory
    {
        /// Creates a rotation matrix about the x-axis for the given \a angle in
        /// radians.
        static HMatrix4 RotationX(double const angle) {
            double s=::sin(angle),c=::cos(angle);

            return HMatrix4(
                Vec::X()
            ,   Vec(0, T( c), T( s))
            ,   Vec(0, T(-s), T( c))
            ,   Vec::ZERO()
            );
        }

        /// Creates a rotation matrix about the y-axis for the given \a angle in
        /// radians.
        static HMatrix4 RotationY(double const angle) {
            double s=::sin(angle),c=::cos(angle);

            return HMatrix4(
                Vec(T( c), 0, T(-s))
            ,   Vec::Y()
            ,   Vec(T( s), 0, T( c))
            ,   Vec::ZERO()
            );
        }

        /// Creates a rotation matrix about the z-axis for the given \a angle in
        /// radians.
        static HMatrix4 RotationZ(double const angle) {
            double s=::sin(angle),c=::cos(angle);

            return HMatrix4(
                Vec(T( c), T( s), 0)
            ,   Vec(T(-s), T( c), 0)
            ,   Vec::Z()
            ,   Vec::ZERO()
            );
        }

        /// Creates a rotation matrix about the given normalized \a axis for the
        /// given \a angle in radians.
        static HMatrix4 Rotation(Vec const& axis,double const angle) {
            double s=::sin(angle),c=1-::cos(angle);
            double xs=s*axis.getX(),ys=s*axis.getY(),zs=s*axis.getZ();
            double xc=c*axis.getX(),yc=c*axis.getY(),zc=c*axis.getZ();

            return HMatrix4(
                Vec(T(1-c + xc*axis.getX()), T( zs + xc*axis.getY()), T(-ys + xc*axis.getZ()))
            ,   Vec(T(-zs + yc*axis.getX()), T(1-c + yc*axis.getY()), T( xs + yc*axis.getZ()))
            ,   Vec(T( ys + zc*axis.getX()), T(-xs + zc*axis.getY()), T(1-c + zc*axis.getZ()))
            ,   Vec::ZERO()
            );
        }

        /// Creates a translation matrix along the given \a direction,
        /// optionally scaled by \a factor.
        static HMatrix4 Translation(Vec const& direction,T const factor=1) {
            return HMatrix4(
                Vec::X()
            ,   Vec::Y()
            ,   Vec::Z()
            ,   direction*factor
            );
        }

        /// Creates a scaling matrix for the given factors along the \a x, \a y
        /// and \a z axes.
        static HMatrix4 Scaling(T const x,T const y,T const z) {
            return HMatrix4(
                Vec(x,0,0)
            ,   Vec(0,y,0)
            ,   Vec(0,0,z)
            ,   Vec::ZERO()
            );
        }

        /// Creates a matrix to project a point in space onto the plane
        /// specified by its normalized \a normal.
        static HMatrix4 Projection(Vec const& normal) {
            T a=normal.getX(),b=normal.getY(),c=normal.getZ();
            T aa=a*a,bb=b*b,cc=c*c;

            return HMatrix4(
                Vec(bb+cc,  -a*b,  -a*c)
            ,   Vec( -b*a, aa+cc,  -b*c)
            ,   Vec( -c*a,  -c*b, aa+bb)
            ,   Vec::ZERO()
            );
        }
    };

    /**
     * \name Predefined constants
     * In order to avoid the so called "static initialization order fiasco",
     * static methods instead of static variables are used here, see
     * <http://www.parashift.com/c++-faq-lite/ctors.html#faq-10.14>.
     */
    //@{

    /// Returns a matrix which has all components set to 0.
    static HMatrix4 const& ZERO() {
        static HMatrix4 const m(Vec::ZERO(),Vec::ZERO(),Vec::ZERO(),Vec::ZERO());
        return m;
    }

    /// Returns the identity matrix (regarding multiplication).
    static HMatrix4 const& IDENTITY() {
        static HMatrix4 const m(Vec::X(),Vec::Y(),Vec::Z(),Vec::ZERO());
        return m;
    }

    //@}

    /**
     * \name Constructors
     */
    //@{

    /// Create a matrix whose components are either not initialized at all or
    /// initialized to 0 if \c GALE_INIT_DATA is defined.
    HMatrix4()
    :   m_c0w(0),m_c1w(0),m_c2w(0),m_c3w(1) {}

    /// Initialize the column vectors with vectors \a c0, \a c1, \a c2 and \a c3.
    HMatrix4(Vec const& c0,Vec const& c1,Vec const& c2,Vec const& c3)
    :   c0(c0),m_c0w(0),c1(c1),m_c1w(0),c2(c2),m_c2w(0),c3(c3),m_c3w(1) {}

    //@}

    /**
     * \name Element access methods
     */
    //@{

    /// Returns a pointer to the matrix data in memory.
    T* data() {
        return c0.data();
    }

    /// Returns a constant pointer to the matrix data in memory
    T const* data() const {
        return c0.data();
    }

    /// Casts \c this matrix to a pointer of type \a T. As an intended side
    /// effect, this also provides indexed data access.
    operator T*() {
        return data();
    }

    /// Casts \c this matrix to a pointer of type \a T \c const. As an intended
    /// side effect, this also provides indexed data access.
    operator T const*() const {
        return data();
    }

    /// Returns a reference to the data element located at \a row and \a column.
    /// Despite usual in maths, the index starts at 0 (not 1).
    T& operator()(unsigned int const row,unsigned int const column) {
        G_ASSERT(row<3 && column<4)
        return (*this)[(column<<2)+row];
    }

    /// Returns a constant reference to the data element located at \a row
    /// and \a column. Despite usual in maths, the index starts at 0 (not 1).
    T const& operator()(unsigned int const row,unsigned int const column) const {
        G_ASSERT(row<3 && column<4)
        return (*this)[(column<<2)+row];
    }

    /// Returns a reference to the first column vector which is interpreted to
    /// point right in the right-handed coordinate system orientation.
    Vec& getRightVector() {
        return c0;
    }

    /// Returns a constant reference to the first column vector which is
    /// interpreted to point right in the right-handed coordinate system
    /// orientation.
    Vec const& getRightVector() const {
        return c0;
    }

    /// Assigns a new value to the first column vector which is interpreted to
    /// point right in the right-handed coordinate system orientation.
    void setRightVector(Vec const& right) {
        c0=right;
    }

    /// Returns a reference to the second column vector which is interpreted to
    /// point up in the right-handed coordinate system orientation.
    Vec& getUpVector() {
        return c1;
    }

    /// Returns a constant reference to the second column vector which is
    /// interpreted to point up in the right-handed coordinate system
    /// orientation.
    Vec const& getUpVector() const {
        return c1;
    }

    /// Assigns a new value to the second column vector which is interpreted to
    /// point up in the right-handed coordinate system orientation.
    void setUpVector(Vec const& up) {
        c1=up;
    }

    /// Returns a reference to the third column vector which is interpreted to
    /// point backward in the right-handed coordinate system orientation.
    Vec& getBackwardVector() {
        return c2;
    }

    /// Returns a constant reference to the third column vector which is
    /// interpreted to point backward in the right-handed coordinate system
    /// orientation.
    Vec const& getBackwardVector() const {
        return c2;
    }

    /// Assigns a new value to the third column vector which is interpreted to
    /// point backward in the right-handed coordinate system orientation.
    void setBackwardVector(Vec const& backward) {
        c2=backward;
    }

    /// Returns a reference to the fourth column vector which is interpreted to
    /// be the coordinate system position.
    Vec& getPositionVector() {
        return c3;
    }

    /// Returns a constant reference to the fourth column vector which is
    /// interpreted to be the coordinate system position.
    Vec const& getPositionVector() const {
        return c3;
    }

    /// Assigns a new value to the fourth column vector which is interpreted to
    /// be the coordinate system position.
    void setPositionVector(Vec const& position) {
        c3=position;
    }

    //@}

    /**
     * \name Arithmetic matrix / matrix operators
     */
    //@{

    /// Element-wise increments \c this matrix by matrix \a m.
    HMatrix4 const& operator+=(HMatrix4 const& m) {
        c0+=m.c0;
        c1+=m.c1;
        c2+=m.c2;
        c3+=m.c3;
        return *this;
    }

    /// Element-wise decrements \c this matrix by matrix \a m.
    HMatrix4 const& operator-=(HMatrix4 const& m) {
        c0-=m.c0;
        c1-=m.c1;
        c2-=m.c2;
        c3-=m.c3;
        return *this;
    }

    /// Multiplies \c this matrix by matrix \a m from the right.
    HMatrix4 const& operator*=(HMatrix4 const& m) {
        return *this=(*this)*m;
    }

    /// Multiplies \c this matrix by the inverse of matrix \a m from the right.
    HMatrix4 const& operator/=(HMatrix4 const& m) {
        return *this=(*this)/m;
    }

    /// Returns matrix \a m unchanged; provided for convenience.
    friend HMatrix4 const& operator+(HMatrix4 const& m) {
        return m;
    }

    /// Returns the element-wise negation of matrix \a m.
    friend HMatrix4 operator-(HMatrix4 const& m) {
        return HMatrix4(-m.c0,-m.c1,-m.c2,-m.c3);
    }

    /// Returns the element-wise sum of matrices \a m and \a n.
    friend HMatrix4 operator+(HMatrix4 const& m,HMatrix4 const& n) {
        return HMatrix4(m)+=n;
    }

    /// Returns the element-wise difference of matrices \a m and \a n.
    friend HMatrix4 operator-(HMatrix4 const& m,HMatrix4 const& n) {
        return HMatrix4(m)-=n;
    }

    /// Returns matrix \a m multiplied by matrix \a n from the right.
    friend HMatrix4 operator*(HMatrix4 const& m,HMatrix4 const& n) {
        Vec c0,c1,c2,c3;

        // 36 scalar multiplications, 27 scalar additions (includes translation).
        for (int row=2;row>=0;--row) {
            int col1=row+4,col2=row+8,col3=row+12;
            c0[row] = m[row]*n[ 0] + m[col1]*n[ 1] + m[col2]*n[ 2];
            c1[row] = m[row]*n[ 4] + m[col1]*n[ 5] + m[col2]*n[ 6];
            c2[row] = m[row]*n[ 8] + m[col1]*n[ 9] + m[col2]*n[10];
            c3[row] = m[row]*n[12] + m[col1]*n[13] + m[col2]*n[14] + m[col3];
        }

        return HMatrix4(c0,c1,c2,c3);
    }

    /// Returns matrix \a m multiplied by the inverse of matrix \a n from the right.
    friend HMatrix4 operator/(HMatrix4 const& m,HMatrix4 const& n) {
        return m*(!n);
    }

    //@}

    /**
     * \name Arithmetic matrix / vector operators
     */
    //@{

    /// Multiplies this matrix from the left to column vector \a v (resulting in
    /// a column vector).
    Vec multFromLeftTo(Vec const& v) const {
        // 9 scalar multiplications, 9 scalar additions (includes translation).
        return Vec(
            c0[0]*v[0] + c1[0]*v[1] + c2[0]*v[2] + c3[0]
        ,   c0[1]*v[0] + c1[1]*v[1] + c2[1]*v[2] + c3[1]
        ,   c0[2]*v[0] + c1[2]*v[1] + c2[2]*v[2] + c3[2]
        );
    }

    /// Multiplies this matrix from the right to row vector \a v (resulting in a
    /// row vector).
    Vec multFromRightTo(Vec const& v) const {
        // 15 scalar multiplications, 9 scalar additions (includes translation).
        T v4=v[0]*c3[0] + v[1]*c3[1] + v[2]*c3[2] + m_c3w;
        T s=v4 ? T(1)/v4 : T(1);
        return Vec(
            s*(v[0]*c0[0] + v[1]*c0[1] + v[2]*c0[2])
        ,   s*(v[0]*c1[0] + v[1]*c1[1] + v[2]*c1[2])
        ,   s*(v[0]*c2[0] + v[1]*c2[1] + v[2]*c2[2])
        );
    }

    //@}

    /**
     * \name Arithmetic matrix / scalar operators
     */
    //@{

    /// Multiplies each element of \c this matrix by a scalar \a s.
    HMatrix4 const& operator*=(T const s) {
        c0*=s;
        c1*=s;
        c2*=s;
        c3*=s;
        return *this;
    }

    /// Divides each element of \c this matrix by a scalar \a s.
    HMatrix4 const& operator/=(T const s) {
        c0/=s;
        c1/=s;
        c2/=s;
        c3/=s;
        return *this;
    }

    /// Multiplies each element of matrix \a m by a scalar \a s from the right.
    friend HMatrix4 operator*(HMatrix4 const& m,T const s) {
        return HMatrix4(m.c0*s,m.c1*s,m.c2*s,m.c3*s);
    }

    /// Multiplies each element of matrix \a m by a scalar \a s from the left.
    friend HMatrix4 operator*(T const s,HMatrix4 const& m) {
        return m*s;
    }

    /// Divides each element of matrix \a m by a scalar \a s.
    friend HMatrix4 operator/(HMatrix4 const& m,T const s) {
        return m*(1/s);
    }

    /// Divides a scalar \a s by each element of matrix \a m.
    friend HMatrix4 operator/(T const s,HMatrix4 const& m) {
        return HMatrix4(s/m.c0,s/m.c1,s/m.c2,s/m.c3);
    }

    //@}

    /**
     * \name Comparison operators
     */
    //@{

    /// Returns whether all elements in \a m equal their counterpart in \a n
    /// with regard to a tolerance depending on the precision of data type \a T.
    friend bool operator==(HMatrix4 const& m,HMatrix4 const& n) {
        return m.c0==n.c0
            && m.c1==n.c1
            && m.c2==n.c2
            && m.c3==n.c3;
    }

    /// Returns whether the elements in \a m are not equal to their counterparts
    /// in \a n with regard to a tolerance depending on the precision of data
    /// type \a T.
    friend bool operator!=(HMatrix4 const& m,HMatrix4 const& n) {
        return !(m==n);
    }

    //@}

    /**
     * \name Miscellaneous methods
     */
    //@{

    /// Returns the determinant of this matrix.
    T determinant() const {
        T a0 = c0[0]*c1[1] - c1[0]*c0[1];
        T a1 = c0[0]*c2[1] - c2[0]*c0[1];
        T a3 = c1[0]*c2[1] - c2[0]*c1[1];

        T const& b2 = c0[2];
        T const& b4 = c1[2];
        T const& b5 = c2[2];

        return a0*b5 - a1*b4 + a3*b2;
    }

    /// Orthonormalizes this matrix using a modified Gram-Schmidt algorithm so
    /// the column vectors are orthogonal to each other and normalized.
    HMatrix4& orthonormalize() {
        c0.normalize();
        c1=~(c1-(c1%c0)*c0);
        c2=c0^c1;
        return *this;
    }

    /// Inverts this matrix. Optionally returns a \a result indicating whether
    /// the matrix is orthonormal and thus the inverse is viable.
    HMatrix4& invert(bool* const result=NULL) {
        if (result) {
            *result=abs(c0%c1)<=Numerics<T>::ZERO_TOLERANCE()
                 && abs(c0%c2)<=Numerics<T>::ZERO_TOLERANCE()
                 && abs(c1%c2)<=Numerics<T>::ZERO_TOLERANCE()
                 && meta::OpCmpEqual::evaluate(c0.length2(),T(1))
                 && meta::OpCmpEqual::evaluate(c1.length2(),T(1))
                 && meta::OpCmpEqual::evaluate(c2.length2(),T(1));
        }

        if (!result || *result) {
            c3=Vec(-c3%c0,-c3%c1,-c3%c2);

            // Transpose the column vectors.
            T tmp;
            tmp=(*this)(1,0); (*this)(1,0)=(*this)(0,1); (*this)(0,1)=tmp;
            tmp=(*this)(2,0); (*this)(2,0)=(*this)(0,2); (*this)(0,2)=tmp;
            tmp=(*this)(2,1); (*this)(2,1)=(*this)(1,2); (*this)(1,2)=tmp;
        }

        return *this;
    }

    //@}

    /**
     * \name Convenience operators for named methods
     */
    //@{

    /// Multiplies matrix \a m from the left to column vector \a v (resulting in
    /// a column vector).
    friend Vec operator*(HMatrix4 const& m,Vec const& v) {
        return m.multFromLeftTo(v);
    }

    /// Returns an orthonormalized copy of matrix \a m.
    friend HMatrix4 operator~(HMatrix4 const& m) {
        return HMatrix4(m).orthonormalize();
    }

    /// Returns an inverted copy of matrix \a m.
    friend HMatrix4 operator!(HMatrix4 const& m) {
        return HMatrix4(m).invert();
    }

    //@}

#ifndef GALE_TINY_CODE

    /**
     * \name Streaming input / output methods
     */
    //@{

    /// Reads a matrix from an input stream.
    friend std::istream& operator>>(std::istream& s,HMatrix4& m) {
        return s >> m.c0 >> m.c1 >> m.c2 >> m.c3;
    }

    /// Writes a matrix to an output stream.
    friend std::ostream& operator<<(std::ostream& s,HMatrix4 const& m) {
        return s << '[' << m.c0 << ',' << m.c1 << ',' << m.c2 << ',' << m.c3 << ']';
    }

    //@}

#endif // GALE_TINY_CODE

  public:

    Vec c0;  ///< Three components of the first column vector.

  private:

    T m_c0w; ///< The fourth component of the first column vector (always 0).

  public:

    Vec c1;  ///< Three components of the second column vector.

  private:

    T m_c1w; ///< The fourth component of the second column vector (always 0).

  public:

    Vec c2;  ///< Three components of the third column vector.

  private:

    T m_c2w; ///< The fourth component of the third column vector (always 0).

  public:

    Vec c3;  ///< Three components of the fourth column vector.

  private:

    T m_c3w; ///< The fourth component of the fourth column vector (always 1).
};

#pragma pack(pop)

/**
 * \name Type definitions for use as OpenGL matrices
 */
//@{

typedef HMatrix4<double> HMat4d;
typedef HMatrix4<float> HMat4f;

//@}

} // namespace math

} // namespace gale

#endif // HMATRIX4_H
