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

#ifndef HMATRIX4_H
#define HMATRIX4_H

/**
 * \file
 * Linear algebra homogeneous matrix routines
 */

#include "vector.h"

#ifndef GALE_TINY
    #include <iostream>
#endif

namespace gale {

namespace math {

/**
 * Homogeneous matrix class implementation based on column vectors. The matrix
 * is stored column-major in memory as required by OpenGL, i.e. the position
 * vector components are located at offsets 12, 13 and 14.
 *
 * Example usage:
 * \code
 * HMat4f m;
 * // ...
 * glMultMatrixf(m);
 * \endcode
 */
template<typename T>
class HMatrix4
{
  public:

    /// Definition of the type of vector used as the columns for the matrix.
    typedef Vector<3,T> Vec;

    /**
     * \name Predefined constants
     * In order to avoid the so called "static initialization order fiasco",
     * static methods instead of static variables are used here (see
     * http://www.parashift.com/c++-faq-lite/ctors.html#faq-10.14).
     */
    //@{

    /// Returns a vector which has all components set to 0.
    static HMatrix4 const& IDENTITY() {
        static HMatrix4 const m(Vec::X(),Vec::Y(),Vec::Z(),Vec::ZERO());
        return m;
    }

    //@}

    /**
     * \name Constructors
     */
    //@{

    /// For performance reasons, do not initialize the whole matrix by default.
    HMatrix4():
      m_c0w(0),m_c1w(0),m_c2w(0),m_c3w(1)
    {
    }

    /// Initialize the column vectors with "normal" vector \a n, "open" vector
    /// \a o, "approach" vector \a a and "position" vector \a p.
    HMatrix4(Vec const& n,Vec const& o,Vec const& a,Vec const& p):
      normal(n),m_c0w(0),up(o),m_c1w(0),look(a),m_c2w(0),position(p),m_c3w(1)
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
     * \name Element-wise arithmetic operators with matrices
     */
    //@{

    /// Increments \c this matrix by another matrix \a m.
    HMatrix4 const& operator+=(HMatrix4 const& m) {
        normal   += m.normal;
        up       += m.up;
        look     += m.look;
        position += m.position;
        return *this;
    }

    /// Decrements \c this matrix by another matrix \a m.
    HMatrix4 const& operator-=(HMatrix4 const& m) {
        normal   -= m.normal;
        up       -= m.up;
        look     -= m.look;
        position -= m.position;
        return *this;
    }

    /// Multiplies \c this matrix by another matrix \a m.
    HMatrix4 const& operator*=(HMatrix4 const& m) {
        return *this=(*this)*m;
    }

    /// Divides \c this matrix by another matrix \a m.
    HMatrix4 const& operator/=(HMatrix4 const& m) {
        return *this=(*this)/m;
    }

    /// Returns matrix \a m unchanged; provided for convenience.
    friend HMatrix4 const& operator+(HMatrix4 const& m) {
        return m;
    }

    /// Returns the negation of matrix \a m.
    friend HMatrix4 operator-(HMatrix4 const& m) {
        return HMatrix4(-m.normal,-m.up,-m.look,-m.position);
    }

    /// Returns the sum of matrices \a m and \a n.
    friend HMatrix4 operator+(HMatrix4 const& m,HMatrix4 const& n) {
        return HMatrix4(m)+=n;
    }

    /// Returns the difference of matrices \a m and \a n.
    friend HMatrix4 operator-(HMatrix4 const& m,HMatrix4 const& n) {
        return HMatrix4(m)-=n;
    }

    /// Returns the product of matrices \a m and \a n.
    friend HMatrix4 operator*(HMatrix4 const& m,HMatrix4 const& n) {
        Vec c0,c1,c2,c3;

        // 36 scalar multiplications, 27 scalar additions (includes translation).
        for (int row=2;row>=0;--row) {
            int row4=row+4,row8=row+8;
            c0[row] = m[row]*n[0]  + m[row4]*n[1]  + m[row8]*n[2];
            c1[row] = m[row]*n[4]  + m[row4]*n[5]  + m[row8]*n[6];
            c2[row] = m[row]*n[8]  + m[row4]*n[9]  + m[row8]*n[10];
            c3[row] = m[row]*n[12] + m[row4]*n[13] + m[row8]*n[14] + m[row+12];
        }

        return HMatrix4(c0,c1,c2,c3);
    }

    /// Returns the product of matrix \a m and the inverse of matrix \a n.
    friend HMatrix4 operator/(HMatrix4 const& m,HMatrix4 const& n) {
        return m*(!n);
    }

    //@}

    /**
     * \name Element-wise arithmetic operators with vectors
     */
    //@{

    /// Multiplies vector \a v from the right to matrix \a m.
    friend Vec operator*(HMatrix4 const& m,Vec const& v) {
        // 9 scalar multiplications, 9 scalar additions (includes translation).
        return Vec(
            m[0]*v[0] + m[4]*v[1] + m[8]*v[2]  + m[12],
            m[1]*v[0] + m[5]*v[1] + m[9]*v[2]  + m[13],
            m[2]*v[0] + m[6]*v[1] + m[10]*v[2] + m[14]
        );
    }

    /// Multiplies vector \a v from the left to matrix \a m.
    friend Vec operator*(Vec const& v,HMatrix4 const& m) {
        // 9 scalar multiplications, 9 scalar additions (includes translation).
        return Vec(
            m[0]*v[0] + m[1]*v[1] + m[2]*v[2]  + m[12],
            m[4]*v[0] + m[5]*v[1] + m[6]*v[2]  + m[13],
            m[8]*v[0] + m[9]*v[1] + m[10]*v[2] + m[14]
        );
    }

    //@}

    /**
     * \name Element-wise arithmetic operators with scalars
     */
    //@{

    /// Multiplies all elements of matrix \a m by the scalar \a s.
    friend HMatrix4 operator*(HMatrix4 const& m,T s) {
        return HMatrix4(m.normal*s,m.up*s,m.look*s,m.position*s);
    }

    /// Multiplies all elements of matrix \a m by the scalar \a s.
    friend HMatrix4 operator*(T s,HMatrix4 const& m) {
        return m*s;
    }

    //@}

    /**
     * \name Miscellaneous methods
     */
    //@{

    /// Orthonormalizes this matrix so the normal, up and look vectors are
    // normalized and orthogonal to each other.
    HMatrix4& orthonormalize() {
        normal.normalize();
        up=~(look^normal);
        look=~(normal^up);
        return *this;
    }

    /// Inverts this matrix; the product of a matrix and its inverse equals the
    /// identity matrix.
    HMatrix4& invert() {
        position=Vec(-position%normal,-position%up,-position%look);

        // Transpose the normal, up and look vectors.
        T tmp;
        tmp=(*this)(1,0); (*this)(1,0)=(*this)(0,1); (*this)(0,1)=tmp;
        tmp=(*this)(2,0); (*this)(2,0)=(*this)(0,2); (*this)(0,2)=tmp;
        tmp=(*this)(2,1); (*this)(2,1)=(*this)(1,2); (*this)(1,2)=tmp;

        return *this;
    }

    //@}

#ifndef GALE_TINY

    /**
     * \name Streaming input / output methods
     */
    //@{

    /// Reads a matrix from an input stream.
    friend std::istream& operator>>(std::istream& s,HMatrix4& m) {
        return s >> m.normal >> m.up >> m.look >> m.position;
    }

    /// Writes a matrix to an output stream.
    friend std::ostream& operator<<(std::ostream& s,HMatrix4 const& m) {
        return s << '[' << m.normal << ',' << m.up << ',' << m.look << ',' << m.position << ']';
    }

    //@}

#endif

  public:

    Vec normal;   ///< Three components of the first column vector.

  private:

    T m_c0w;      ///< The fourth component of the first column vector (always 0).

  public:

    Vec up;       ///< Three components of the second column vector.

  private:

    T m_c1w;      ///< The fourth component of the second column vector (always 0).

  public:

    Vec look;     ///< Three components of the third column vector.

  private:

    T m_c2w;      ///< The fourth component of the third column vector (always 0).

  public:

    Vec position; ///< Three components of the fourth column vector.

  private:

    T m_c3w;      ///< The fourth component of the fourth column vector (always 1).
};

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
