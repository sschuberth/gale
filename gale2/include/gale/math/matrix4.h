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

#ifndef MATRIX4_H
#define MATRIX4_H

/**
 * \file
 * Linear algebra matrix routines
 */

#include "vector.h"

namespace gale {

namespace math {

// Make sure data members are tightly packed.
#pragma pack(push,1)

/**
 * Matrix class implementation based on column vectors. The matrix is stored as
 * 4x4 numbers organized in column-major order in memory as required by OpenGL,
 * i.e. the matrix entry at row r, 0 <= r <= 3, and column c, 0 <= c <= 3, is
 * located at offset i = c*4 + r.
 *
 * OpenGL example usage:
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

    /// Definition for external access to the data type.
    typedef T Type;

    /// Definition of the type of vector used as the columns for the matrix.
    typedef Vector<4,T> Vec;

    /// %Factory methods to easily create commonly used matrices.
    struct Factory
    {
        /// Creates an orthographic projection matrix with the points in the
        /// near clipping at \a clip_left, \a clip_bottom and at \a clip_right,
        /// \a clip_top being mapped to the lower left and upper right window
        /// corners respectively.
        static Matrix4 OrthographicProjection(
            double const clip_left
        ,   double const clip_right
        ,   double const clip_bottom
        ,   double const clip_top
        ,   double const clip_near=-1.0
        ,   double const clip_far=1.0
        )
        {
            // Right plus / minus left clipping values.
            double rpl=clip_right+clip_left;
            double rml=clip_right-clip_left;

            // Top plus / minus bottom clipping values.
            double tpb=clip_top+clip_bottom;
            double tmb=clip_top-clip_bottom;

            // Far plus / minus near clipping values.
            double fpn=clip_far+clip_near;
            double fmn=clip_far-clip_near;

            return Matrix4(
                Vec( 2.0/rml,      0.0,      0.0, 0.0)
            ,   Vec(     0.0,  2.0/tmb,      0.0, 0.0)
            ,   Vec(     0.0,      0.0, -2.0/fmn, 0.0)
            ,   Vec(-rpl/rml, -tpb/tmb, -fpn/fmn, 1.0)
            );
        }

        /// Creates a perspective projection matrix for a viewport of the given
        /// \a view_width and \a view_height, a vertical field of view of \a fov
        /// and clipping planes located at \a clip_near and \a clip_far.
        static Matrix4 PerspectiveProjection(
            int const view_width
        ,   int const view_height
        ,   double const fov=Constd::PI()*0.25
        ,   double const clip_near=0.001
        ,   double const clip_far=1000.0
        )
        {
            // For details, see the gluPerspective() documentation in the OpenGL SDK.
            double f=1.0/tan(fov*0.5);
            double a=static_cast<double>(view_width)/view_height;

            double s=clip_near+clip_far;
            double d=clip_near-clip_far;
            double p=clip_near*clip_far;

            return Matrix4(
                Vec(f/a, 0.0,     0.0,  0.0)
            ,   Vec(0.0,   f,     0.0,  0.0)
            ,   Vec(0.0, 0.0,     s/d, -1.0)
            ,   Vec(0.0, 0.0, 2.0*p/d,  0.0)
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

    /// Create a matrix whose components are either not initialized at all or
    /// initialized to 0 if \c GALE_INIT_DATA is defined.
    Matrix4() {}

    /// Initialize the column vectors with vectors \a c0 to \a c3.
    Matrix4(Vec const& c0,Vec const& c1,Vec const& c2,Vec const& c3)
    :   c0(c0),c1(c1),c2(c2),c3(c3) {}

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
        G_ASSERT(row<4 && column<4)
        return (*this)[(column<<2)+row];
    }

    /// Returns a constant reference to the data element located at \a row
    /// and \a column. Despite usual in maths, the index starts at 0 (not 1).
    T const& operator()(unsigned int const row,unsigned int const column) const {
        G_ASSERT(row<4 && column<4)
        return (*this)[(column<<2)+row];
    }

    //@}

    /**
     * \name Arithmetic matrix / matrix operators
     */
    //@{

    /// Element-wise increments \c this matrix by matrix \a m.
    Matrix4 const& operator+=(Matrix4 const& m) {
        c0+=m.c0;
        c1+=m.c1;
        c2+=m.c2;
        c3+=m.c3;
        return *this;
    }

    /// Element-wise decrements \c this matrix by matrix \a m.
    Matrix4 const& operator-=(Matrix4 const& m) {
        c0-=m.c0;
        c1-=m.c1;
        c2-=m.c2;
        c3-=m.c3;
        return *this;
    }

    /// Multiplies \c this matrix by matrix \a m from the right.
    Matrix4 const& operator*=(Matrix4 const& m) {
        return *this=(*this)*m;
    }

    /// Returns matrix \a m unchanged; provided for convenience.
    friend Matrix4 const& operator+(Matrix4 const& m) {
        return m;
    }

    /// Returns the element-wise negation of matrix \a m.
    friend Matrix4 operator-(Matrix4 const& m) {
        return Matrix4(-m.c0,-m.c1,-m.c2,-m.c3);
    }

    /// Returns the element-wise sum of matrices \a m and \a n.
    friend Matrix4 operator+(Matrix4 const& m,Matrix4 const& n) {
        return Matrix4(m)+=n;
    }

    /// Returns the element-wise difference of matrices \a m and \a n.
    friend Matrix4 operator-(Matrix4 const& m,Matrix4 const& n) {
        return Matrix4(m)-=n;
    }

    /// Returns matrix \a m multiplied by matrix \a n from the right.
    friend Matrix4 operator*(Matrix4 const& m,Matrix4 const& n) {
        Vec c0,c1,c2,c3;

        // 64 scalar muls/divs, 48 scalar adds/subs.
        for (int row=3;row>=0;--row) {
            int col1=row+4,col2=row+8,col3=row+12;
            c0[row] = m[row]*n[ 0] + m[col1]*n[ 1] + m[col2]*n[ 2] + m[col3]*n[ 3];
            c1[row] = m[row]*n[ 4] + m[col1]*n[ 5] + m[col2]*n[ 6] + m[col3]*n[ 7];
            c2[row] = m[row]*n[ 8] + m[col1]*n[ 9] + m[col2]*n[10] + m[col3]*n[11];
            c3[row] = m[row]*n[12] + m[col1]*n[13] + m[col2]*n[14] + m[col3]*n[15];
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
        // 16 scalar muls/divs, 12 scalar adds/subs.
        return Vec(
            c0[0]*v[0] + c1[0]*v[1] + c2[0]*v[2] + c3[0]*v[3]
        ,   c0[1]*v[0] + c1[1]*v[1] + c2[1]*v[2] + c3[1]*v[3]
        ,   c0[2]*v[0] + c1[2]*v[1] + c2[2]*v[2] + c3[2]*v[3]
        ,   c0[3]*v[0] + c1[3]*v[1] + c2[3]*v[2] + c3[3]*v[3]
        );
    }

    /// Multiplies this matrix from the right to row vector \a v (resulting in a
    /// row vector).
    Vec multFromRightTo(Vec const& v) const {
        // 16 scalar muls/divs, 12 scalar adds/subs.
        return Vec(
            v[0]*c0[0] + v[1]*c0[1] + v[2]*c0[2] + v[3]*c0[3]
        ,   v[0]*c1[0] + v[1]*c1[1] + v[2]*c1[2] + v[3]*c1[3]
        ,   v[0]*c2[0] + v[1]*c2[1] + v[2]*c2[2] + v[3]*c2[3]
        ,   v[0]*c3[0] + v[1]*c3[1] + v[2]*c3[2] + v[3]*c3[3]
        );
    }

    /// Multiplies this matrix from the left to column vector \a v (resulting in
    /// a column vector) using homogeneous coordinates.
    Vector<3,T> multFromLeftTo(Vector<3,T> const& v) const {
        // 16 scalar muls/divs, 12 scalar adds/subs.
        T v4 = c0[3]*v[0] + c1[3]*v[1] + c2[3]*v[2] + c3[3];
        T  s = v4 ? T(1)/v4 : T(1);
        return Vec(
            s*(c0[0]*v[0] + c1[0]*v[1] + c2[0]*v[2] + c3[0])
        ,   s*(c0[1]*v[0] + c1[1]*v[1] + c2[1]*v[2] + c3[1])
        ,   s*(c0[2]*v[0] + c1[2]*v[1] + c2[2]*v[2] + c3[2])
        );
    }

    /// Multiplies this matrix from the right to row vector \a v (resulting in a
    /// row vector) using homogeneous coordinates.
    Vector<3,T> multFromRightTo(Vector<3,T> const& v) const {
        // 16 scalar muls/divs, 12 scalar adds/subs.
        T v4 = v[0]*c3[0] + v[1]*c3[1] + v[2]*c3[2] + c3[3];
        T  s = v4 ? T(1)/v4 : T(1);
        return Vec(
            s*(v[0]*c0[0] + v[1]*c0[1] + v[2]*c0[2] + c0[3])
        ,   s*(v[0]*c1[0] + v[1]*c1[1] + v[2]*c1[2] + c1[3])
        ,   s*(v[0]*c2[0] + v[1]*c2[1] + v[2]*c2[2] + c2[3])
        );
    }

    //@}

    /**
     * \name Arithmetic matrix / scalar operators
     */
    //@{

    /// Multiplies each element of \c this matrix by a scalar \a s.
    Matrix4 const& operator*=(T const s) {
        c0*=s;
        c1*=s;
        c2*=s;
        c3*=s;
        return *this;
    }

    /// Divides each element of \c this matrix by a scalar \a s.
    Matrix4 const& operator/=(T const s) {
        c0/=s;
        c1/=s;
        c2/=s;
        c3/=s;
        return *this;
    }

    /// Multiplies each element of matrix \a m by a scalar \a s from the right.
    friend Matrix4 operator*(Matrix4 const& m,T const s) {
        return Matrix4(m.c0*s,m.c1*s,m.c2*s,m.c3*s);
    }

    /// Multiplies each element of matrix \a m by a scalar \a s from the left.
    friend Matrix4 operator*(T const s,Matrix4 const& m) {
        return m*s;
    }

    /// Divides each element of matrix \a m by a scalar \a s.
    friend Matrix4 operator/(Matrix4 const& m,T const s) {
        return m*(1/s);
    }

    /// Divides a scalar \a s by each element of matrix \a m.
    friend Matrix4 operator/(T const s,Matrix4 const& m) {
        return Matrix4(s/m.c0,s/m.c1,s/m.c2,s/m.c3);
    }

    //@}

    /**
     * \name Comparison operators
     */
    //@{

    /// Returns whether all elements in \a m equal their counterpart in \a n
    /// with regard to a tolerance depending on the precision of data type \a T.
    friend bool operator==(Matrix4 const& m,Matrix4 const& n) {
        return m.c0==n.c0
            && m.c1==n.c1
            && m.c2==n.c2
            && m.c3==n.c3;
    }

    /// Returns whether the elements in \a m are not equal to their counterparts
    /// in \a n with regard to a tolerance depending on the precision of data
    /// type \a T.
    friend bool operator!=(Matrix4 const& m,Matrix4 const& n) {
        return !(m==n);
    }

    //@}

    /**
     * \name Miscellaneous methods
     */
    //@{

    /// Returns the determinant of this matrix.
    T determinant() const {
        T a[6],b[6];
        interim(a,b);
        return determinant(a,b);
    }

    /// Returns the adjoint of this matrix.
    Matrix4 adjoint() const {
        T a[6],b[6];
        interim(a,b);
        return adjoint(a,b);
    }

    /// Inverts this matrix. Optionally returns a \a result indicating whether
    /// the matrix' determinant is non-zero and thus the inverse exists.
    void invert(bool* const result=NULL) {
        T a[6],b[6];
        interim(a,b);

        T det=determinant(a,b);
        bool valid=(abs(det)>Numerics<T>::ZERO_TOLERANCE());

        if (result) {
            *result=valid;
        }

        if (valid) {
            *this=(1/det)*adjoint(a,b);
        }
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

    /// Returns the inverse of matrix \a m.
    friend Matrix4 operator!(Matrix4 const& m) {
        Matrix4 tmp=m;
        tmp.invert();
        return tmp;
    }

    //@}

#ifndef GALE_TINY_CODE

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

#endif // GALE_TINY_CODE

  private:

    /// Calculates interim results for determinant() and adjoint().
    void interim(T (&a)[6],T (&b)[6]) const {
        static unsigned int const indices[]={
            0,  0,  0,  4,  4,  8
        ,   5,  9, 13,  9, 13, 13
        };

        for (int i=0;i<6;++i) {
            unsigned int o0=indices[i];
            unsigned int o1=indices[i+6];

            a[i]=(*this)[o0]*(*this)[o1] - (*this)[o1-1]*(*this)[o0+1];

            o0+=2;
            o1+=2;

            b[i]=(*this)[o0]*(*this)[o1] - (*this)[o1-1]*(*this)[o0+1];
        }
    }

    /// Returns the determinant based on the interim values in \a a and \a b.
    T determinant(T const a[6],T const b[6]) const {
        return a[0]*b[5] - a[1]*b[4] + a[2]*b[3] + a[3]*b[2] - a[4]*b[1] + a[5]*b[0];
    }

    /// Returns the adjoint based on the interim values in \a a and \a b.
    Matrix4 adjoint(T const a[6],T const b[6]) const {
        Vec v[4];

        unsigned int o0=0;
        unsigned int o1=1;
        T const* f=b;

        for (int i=0;i<2;++i) {
            v[o0]=Vec(
                + c1[o1]*f[5] - c2[o1]*f[4] + c3[o1]*f[3]
            ,   - c0[o1]*f[5] + c2[o1]*f[2] - c3[o1]*f[1]
            ,   + c0[o1]*f[4] - c1[o1]*f[2] + c3[o1]*f[0]
            ,   - c0[o1]*f[3] + c1[o1]*f[1] - c2[o1]*f[0]
            );
            v[o1]=Vec(
                - c1[o0]*f[5] + c2[o0]*f[4] - c3[o0]*f[3]
            ,   + c0[o0]*f[5] - c2[o0]*f[2] + c3[o0]*f[1]
            ,   - c0[o0]*f[4] + c1[o0]*f[2] - c3[o0]*f[0]
            ,   + c0[o0]*f[3] - c1[o0]*f[1] + c2[o0]*f[0]
            );

            o0+=2;
            o1+=2;
            f=a;
        }

        return Matrix4(v[0],v[1],v[2],v[3]);
    }

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
