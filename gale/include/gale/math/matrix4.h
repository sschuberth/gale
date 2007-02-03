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
 * Matrix class implementation based on column vectors. The matrix is stored in
 * column-major order in memory as required by OpenGL.
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
    Matrix4() {
    }

    /// Initialize the column vectors with vectors \a c0 to \a c3.
    Matrix4(Vec const& c0,Vec const& c1,Vec const& c2,Vec const& c3):
      m_c0(c0),m_c1(c1),m_c2(c2),m_c3(c3)
    {
    }

    //@}

    /**
     * \name Element access methods
     */
    //@{

    /// Returns a pointer to the matrix data in memory.
    T* getData() {
        return normal;
    }

    /// Returns a \c constant pointer to the matrix data in memory
    T const* getData() const {
        return normal;
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
    /// Despite in maths, usually, the index runs from 0 to 3.
    T& operator()(unsigned int row,unsigned int column) {
        G_ASSERT(row<4 && column<4)
        return (*this)[(column<<2)+row];
    }

    /// Returns a constant reference to the data element located at \a row and
    /// \a column. Despite in maths, usually, the index runs from 0 to 3.
    T const& operator()(unsigned int row,unsigned int column) const {
        G_ASSERT(row<4 && column<4)
        return (*this)[(column<<2)+row];
    }

    //@}

    /**
     * \name Arithmetic matrix to matrix operators
     */
    //@{

    /// Increments \c this matrix by another matrix \a m.
    Matrix4 const& operator+=(Matrix4 const& m) {
        m_c0+=m.m_c0;
        m_c1+=m.m_c1;
        m_c2+=m.m_c2;
        m_c3+=m.m_c3;
        return *this;
    }

    /// Decrements \c this matrix by another matrix \a m.
    Matrix4 const& operator-=(Matrix4 const& m) {
        m_c0-=m.m_c0;
        m_c1-=m.m_c1;
        m_c2-=m.m_c2;
        m_c3-=m.m_c3;
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
        return Matrix4(-m.m_c0,-m.m_c1,-m.m_c2,-m.m_c3);
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
     * \name Arithmetic matrix to vector operators
     */
    //@{

    /// Multiplies vector \a v from the right to matrix \a m.
    friend Vec operator*(Matrix4 const& m,Vec const& v) {
        // 16 scalar multiplications, 12 scalar additions.
        return Vec(
            m[0]*v[0] + m[4]*v[1] + m[8]*v[2]  + m[12]*v[3],
            m[1]*v[0] + m[5]*v[1] + m[9]*v[2]  + m[13]*v[3],
            m[2]*v[0] + m[6]*v[1] + m[10]*v[2] + m[14]*v[3],
            m[3]*v[0] + m[7]*v[1] + m[11]*v[2] + m[15]*v[3]
        );
    }

    /// Multiplies vector \a v from the left to matrix \a m.
    friend Vec operator*(Vec const& v,Matrix4 const& m) {
        // 16 scalar multiplications, 12 scalar additions.
        return Vec(
            m[0]*v[0]  + m[1]*v[1]  + m[2]*v[2]  + m[3]*v[3],
            m[4]*v[0]  + m[5]*v[1]  + m[6]*v[2]  + m[7]*v[3],
            m[8]*v[0]  + m[9]*v[1]  + m[10]*v[2] + m[11]*v[3],
            m[12]*v[0] + m[13]*v[1] + m[14]*v[2] + m[15]*v[3]
        );
    }

    //@}

    /**
     * \name Arithmetic matrix to scalar operators
     */
    //@{

    /// Multiplies all elements of matrix \a m by the scalar \a s.
    friend Matrix4 operator*(Matrix4 const& m,T s) {
        return Matrix4(m.m_c0*s,m.m_c1*s,m.m_c2*s,m.m_c3*s);
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
        return m.m_c0 == n.m_c0
            && m.m_c1 == n.m_c1
            && m.m_c2 == n.m_c2
            && m.m_c3 == n.m_c3;
    }

    /// Returns whether the elements in \a m are not equal to their counterparts
    /// in \a n using an epsilon-environment depending on the precision of \a T.
    friend bool operator!=(Matrix4 const& m,Matrix4 const& n) {
        return !(m==n);
    }

    //@}

#ifndef GALE_TINY

    /**
     * \name Streaming input / output methods
     */
    //@{

    /// Reads a matrix from an input stream.
    friend std::istream& operator>>(std::istream& s,Matrix4& m) {
        return s >> m.m_c0 >> m.m_c1 >> m.m_c2 >> m.m_c3;
    }

    /// Writes a matrix to an output stream.
    friend std::ostream& operator<<(std::ostream& s,Matrix4 const& m) {
        return s << '[' << m.m_c0 << ',' << m.m_c1 << ',' << m.m_c2 << ',' << m.m_c3 << ']';
    }

    //@}

#endif // GALE_TINY

  private:

    Vec m_c0; ///< The first column vector.
    Vec m_c1; ///< The second column vector.
    Vec m_c2; ///< The third column vector.
    Vec m_c3; ///< The fourth column vector.
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
