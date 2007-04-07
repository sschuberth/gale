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

#ifndef MATRIX4_H
#define MATRIX4_H

/**
 * \file
 * Linear algebra matrix routines
 */

#include "vector.h"

#ifndef GALE_TINY
    #include <iostream>
#endif

namespace gale {

namespace math {

// Make sure data members are tightly packed.
#pragma pack(push,1)

/**
 * Matrix class implementation based on column vectors. The matrix is stored as
 * 4x4 numbers organized in column-major order in memory as required by OpenGL,
 * i.e. the matrix entry at row r (0 <= r <= 3) and column c (0 <= c <= 3) is
 * located at offset i = c*4 + r.
 *
 * Example usage:
 * \code
 * Mat4d m;
 * // ...
 * glMatrixMode(GL_PROJECTION);
 * glLoadMatrixd(m);
 * \endcode
 */
template<typename T>
class Matrix4
{
  public:

    /// Definition of the type of vector used as the columns for the matrix.
    typedef Vector<4,T> Vec;

    /**
     * \name Predefined constants
     * In order to avoid the so called "static initialization order fiasco",
     * static methods instead of static variables are used here (see
     * http://www.parashift.com/c++-faq-lite/ctors.html#faq-10.14).
     */
    //@{

    /// Returns a matrix which has all components set to 0.
    static Matrix4 const& ZERO() {
        static Matrix4 const m(Vec::ZERO(),Vec::ZERO(),Vec::ZERO(),Vec::ZERO());
        return m;
    }

    /// Returns the identity matrix (regarding multiplication).
    static Matrix4 const& IDENTITY() {
        static Matrix4 const m(Vec::X(),Vec::Y(),Vec::Z(),Vec::W());
        return m;
    }

    //@}

    /**
     * \name Constructors
     */
    //@{

    /// For performance reasons, do not initialize any data by default.
    Matrix4() {}

    /// Initialize the column vectors with vectors \a c0 to \a c3.
    Matrix4(Vec const& c0,Vec const& c1,Vec const& c2,Vec const& c3):
      c0(c0),c1(c1),c2(c2),c3(c3) {}

    //@}

    /**
     * \name Element access methods
     */
    //@{

    /// Returns a pointer to the matrix data in memory.
    T* getData() {
        return c0;
    }

    /// Returns a \c constant pointer to the matrix data in memory
    T const* getData() const {
        return c0;
    }

    /// Casts \c this matrix to a pointer of type \a T. As an intended side
    /// effect, this also provides indexed data access.
    operator T*() {
        return getData();
    }

    /// Casts \c this matrix to a \c constant pointer of type \a T. As an
    /// intended side effect, this also provides indexed data access.
    operator T const*() const {
        return getData();
    }

    /// Returns a reference to the data element located at \a row and \a column.
    /// Despite usual in maths, the index starts at 0 (not 1).
    T& operator()(unsigned int row,unsigned int column) {
        G_ASSERT(row<4 && column<4)
        return (*this)[(column<<2)+row];
    }

    /// Returns a constant reference to the data element located at \a row and
    /// \a column. Despite usual in maths, the index starts at 0 (not 1).
    T const& operator()(unsigned int row,unsigned int column) const {
        G_ASSERT(row<4 && column<4)
        return (*this)[(column<<2)+row];
    }

    //@}

    /**
     * \name Arithmetic matrix / matrix operators
     */
    //@{

    /// Increments \c this matrix by another matrix \a m.
    Matrix4 const& operator+=(Matrix4 const& m) {
        c0+=m.c0;
        c1+=m.c1;
        c2+=m.c2;
        c3+=m.c3;
        return *this;
    }

    /// Decrements \c this matrix by another matrix \a m.
    Matrix4 const& operator-=(Matrix4 const& m) {
        c0-=m.c0;
        c1-=m.c1;
        c2-=m.c2;
        c3-=m.c3;
        return *this;
    }

    /// Multiplies \c this matrix by another matrix \a m.
    Matrix4 const& operator*=(Matrix4 const& m) {
        return *this=(*this)*m;
    }

    /// Returns matrix \a m unchanged; provided for convenience.
    friend Matrix4 const& operator+(Matrix4 const& m) {
        return m;
    }

    /// Returns the negation of matrix \a m.
    friend Matrix4 operator-(Matrix4 const& m) {
        return Matrix4(-m.c0,-m.c1,-m.c2,-m.c3);
    }

    /// Returns the sum of matrices \a m and \a n.
    friend Matrix4 operator+(Matrix4 const& m,Matrix4 const& n) {
        return Matrix4(m)+=n;
    }

    /// Returns the difference of matrices \a m and \a n.
    friend Matrix4 operator-(Matrix4 const& m,Matrix4 const& n) {
        return Matrix4(m)-=n;
    }

    /// Returns the product of matrices \a m and \a n.
    friend Matrix4 operator*(Matrix4 const& m,Matrix4 const& n) {
        Vec c0,c1,c2,c3;

        // 64 scalar multiplications, 48 scalar additions.
        for (int row=3;row>=0;--row) {
            int row4=row+4,row8=row+8,row12=row+12;
            c0[row] = m[row]*n[0]  + m[row4]*n[1]  + m[row8]*n[2]  + m[row12]*n[3];
            c1[row] = m[row]*n[4]  + m[row4]*n[5]  + m[row8]*n[6]  + m[row12]*n[7];
            c2[row] = m[row]*n[8]  + m[row4]*n[9]  + m[row8]*n[10] + m[row12]*n[11];
            c3[row] = m[row]*n[12] + m[row4]*n[13] + m[row8]*n[14] + m[row12]*n[15];
        }

        return Matrix4(c0,c1,c2,c3);
    }

    //@}

    /**
     * \name Arithmetic matrix / vector operators
     */
    //@{

    /// Multiplies this matrix from the left to column vector \a v (resulting in
    /// a column vector).
    Vec multFromLeftTo(Vec const& v) const {
        // 16 scalar multiplications, 12 scalar additions.
        return Vec(
            c0[0]*v[0] + c1[0]*v[1] + c2[0]*v[2] + c3[0]*v[3],
            c0[1]*v[0] + c1[1]*v[1] + c2[1]*v[2] + c3[1]*v[3],
            c0[2]*v[0] + c1[2]*v[1] + c2[2]*v[2] + c3[2]*v[3],
            c0[3]*v[0] + c1[3]*v[1] + c2[3]*v[2] + c3[3]*v[3]
        );
    }

    /// Multiplies this matrix from the right to row vector \a v (resulting in a
    /// row vector).
    Vec multFromRightTo(Vec const& v) const {
        // 16 scalar multiplications, 12 scalar additions.
        return Vec(
            v[0]*c0[0] + v[1]*c0[1] + v[2]*c0[2] + v[3]*c0[3],
            v[0]*c1[0] + v[1]*c1[1] + v[2]*c1[2] + v[3]*c1[3],
            v[0]*c2[0] + v[1]*c2[1] + v[2]*c2[2] + v[3]*c2[3],
            v[0]*c3[0] + v[1]*c3[1] + v[2]*c3[2] + v[3]*c3[3]
        );
    }

    /// Multiplies this matrix from the left to column vector \a v (resulting in
    /// a column vector) using homogeneous coordinates.
    Vector<3,T> multFromLeftTo(Vector<3,T> const& v) const {
        // 15 scalar multiplications, 12 scalar additions.
        T v4=c0[3]*v[0] + c1[3]*v[1] + c2[3]*v[2] + c3[3];
        T s=v4 ? 1.0f/v4 : 1.0f;
        return Vec(
            s*(c0[0]*v[0] + c1[0]*v[1] + c2[0]*v[2] + c3[0]),
            s*(c0[1]*v[0] + c1[1]*v[1] + c2[1]*v[2] + c3[1]),
            s*(c0[2]*v[0] + c1[2]*v[1] + c2[2]*v[2] + c3[2])
        );
    }

    /// Multiplies this matrix from the right to row vector \a v (resulting in a
    /// row vector) using homogeneous coordinates.
    Vector<3,T> multFromRightTo(Vector<3,T> const& v) const {
        // 15 scalar multiplications, 12 scalar additions.
        T v4=v[0]*c3[0] + v[1]*c3[1] + v[2]*c3[2] + c3[3];
        T s=v4 ? 1.0f/v4 : 1.0f;
        return Vec(
            s*(v[0]*c0[0] + v[1]*c0[1] + v[2]*c0[2] + c0[3]),
            s*(v[0]*c1[0] + v[1]*c1[1] + v[2]*c1[2] + c1[3]),
            s*(v[0]*c2[0] + v[1]*c2[1] + v[2]*c2[2] + c2[3])
        );
    }

    //@}

    /**
     * \name Arithmetic matrix / scalar operators
     */
    //@{

    /// Multiplies all elements of matrix \a m by the scalar \a s.
    friend Matrix4 operator*(Matrix4 const& m,T s) {
        return Matrix4(m.c0*s,m.c1*s,m.c2*s,m.c3*s);
    }

    /// Multiplies all elements of matrix \a m by the scalar \a s.
    friend Matrix4 operator*(T s,Matrix4 const& m) {
        return m*s;
    }

    /// Divides all elements of matrix \a m by the scalar \a s.
    friend Matrix4 operator/(Matrix4 const& m,T s) {
        return m*(1/s);
    }

    //@}

    /**
     * \name Comparison operators
     */
    //@{

    /// Returns whether all elements in \a m equal their counterpart in \a n
    /// using an epsilon-environment depending on the precision of \a T.
    friend bool operator==(Matrix4 const& m,Matrix4 const& n) {
        return m.c0 == n.c0
            && m.c1 == n.c1
            && m.c2 == n.c2
            && m.c3 == n.c3;
    }

    /// Returns whether the elements in \a m are not equal to their counterparts
    /// in \a n using an epsilon-environment depending on the precision of \a T.
    friend bool operator!=(Matrix4 const& m,Matrix4 const& n) {
        return !(m==n);
    }

    //@}

    /**
     * \name Convenience operators for named methods
     */
    //@{

    /// Multiplies matrix \a m from the left to column vector \a v (resulting in
    /// a column vector).
    friend Vec operator*(Matrix4 const& m,Vec const& v) {
        return m.multFromLeftTo(v);
    }

    /// Multiplies matrix \a m from the left to column vector \a v (resulting in
    /// a column vector) using homogeneous coordinates.
    friend Vector<3,T> operator*(Matrix4 const& m,Vector<3,T> const& v) {
        return m.multFromLeftTo(v);
    }

    //@}

#ifndef GALE_TINY

    /**
     * \name Streaming input / output methods
     */
    //@{

    /// Reads a matrix from an input stream.
    friend std::istream& operator>>(std::istream& s,Matrix4& m) {
        return s >> m.c0 >> m.c1 >> m.c2 >> m.c3;
    }

    /// Writes a matrix to an output stream.
    friend std::ostream& operator<<(std::ostream& s,Matrix4 const& m) {
        return s << '[' << m.c0 << ',' << m.c1 << ',' << m.c2 << ',' << m.c3 << ']';
    }

    //@}

#endif // GALE_TINY

  public:

    Vec c0; ///< The first column vector.
    Vec c1; ///< The second column vector.
    Vec c2; ///< The third column vector.
    Vec c3; ///< The fourth column vector.
};

#pragma pack(pop)

/**
 * \name Type definitions for use as OpenGL matrices
 */
//@{

typedef Matrix4<double> Mat4d;
typedef Matrix4<float> Mat4f;

//@}

} // namespace math

} // namespace gale

#endif // MATRIX4_H
