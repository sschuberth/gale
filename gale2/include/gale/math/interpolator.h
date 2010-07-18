/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
 *
 * Copyright (C) 2005-2010  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
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

#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

/**
 * \file
 * Interpolation methods
 */

#include "../global/dynamicarray.h"

// For specialized friend lerp()-methods.
#include "quaternion.h"

namespace gale {

namespace math {

/// Linearly interpolates between \a a and \a b based on \a s. For performance
/// reasons, \a s is not clamped to [0,1].
template<class T>
static T lerp(T const& a,T const& b,float const s) {
    return a+(b-a)*s;
}

/**
 * A collection of interpolation (and approximation) methods.
 */
class Interpolator
{
  public:

    /// Piecewise linear interpolation of the values in \a v at position \a s.
    /// The position in range [0,1] is mapped to the array size. If \a closed is
    /// \c true, the values are treated as being periodic, resulting in a closed
    /// curve. Also see <http://local.wasp.uwa.edu.au/~pbourke/miscellaneous/interpolation/>.
    template<class T>
    static T Linear(global::DynamicArray<T> const& v,float s,bool const closed=false) {
        int i0,i1;
        s=getInterval(v.getSize()-1,s,closed,i0,i1);

        return lerp(v[i0],v[i1],s);
    }

    /// Piecewise cosine interpolation of the values in \a v at position \a s.
    /// The position in range [0,1] is mapped to the array size. If \a closed is
    /// \c true, the values are treated as being periodic, resulting in a closed
    /// curve. Also see <http://local.wasp.uwa.edu.au/~pbourke/miscellaneous/interpolation/>.
    template<class T>
    static T Cosine(global::DynamicArray<T> const& v,float s,bool const closed=false) {
        int i0,i1;
        s=getInterval(v.getSize()-1,s,closed,i0,i1);

        return lerp(v[i0],v[i1],(1.0f-cos(Constf::PI()*s))*0.5f);
    }

    /// Piecewise cubic Bezier interpolation of the values in \a v and their
    /// corresponding tangents in \a t at position \a s. The position in range
    /// [0,1] is mapped to the array size. If \a closed is \c true, the values
    /// are treated as being periodic, resulting in a closed curve. Also see
    /// <http://local.wasp.uwa.edu.au/~pbourke/geometry/bezier/cubicbezier.html>.
    template<class T>
    static T Bezier(global::DynamicArray<T> const& v,global::DynamicArray<T> const& t,float s,bool const closed=false) {
        static signed char const w[]={
            -1, 3,-3, 1
        ,    3,-6, 3, 0
        ,   -3, 3, 0, 0
        ,    1, 0, 0, 0
        };

        int i0,i1;
        s=getInterval(v.getSize()-1,s,closed,i0,i1);

        return evalPolynomial(v[i0],v[i0]+t[i0],v[i1]-t[i1],v[i1],w,1.0f,s);
    }

    /// Piecewise cubic B-Spline approximation of the values in \a v at
    /// position \a s. The position in range [0,1] is mapped to the array size.
    /// If \a closed is \c true, the values are treated as being periodic,
    /// resulting in a closed curve. Also see <http://blackpawn.com/texts/splines/>.
    template<class T>
    static T BSpline(global::DynamicArray<T> const& v,float s,bool const closed=false) {
        static signed char const w[]={
            -1, 3,-3, 1
        ,    3,-6, 3, 0
        ,   -3, 0, 3, 0
        ,    1, 4, 1, 0
        };

        int n=v.getSize()-1;

        int i1,i2;
        s=getInterval(n,s,closed,i1,i2);

        int i0=(i1==0)?(closed?n:0):i1-1;
        int i3=(i2==n)?(closed?0:n):i2+1;

        return evalPolynomial(v[i0],v[i1],v[i2],v[i3],w,1.0f/6.0f,s);
    }

    /// Piecewise cubic Catmull-Rom interpolation of the values in \a v at
    /// position \a s. The position in range [0,1] is mapped to the array size.
    /// If \a closed is \c true, the values are treated as being periodic,
    /// resulting in a closed curve. Also see <http://blackpawn.com/texts/splines/>.
    template<class T>
    static T CatmullRom(global::DynamicArray<T> const& v,float s,bool const closed=false) {
        static signed char const w[]={
            -1, 3,-3, 1
        ,    2,-5, 4,-1
        ,   -1, 0, 1, 0
        ,    0, 2, 0, 0
        };

        int n=v.getSize()-1;

        int i1,i2;
        s=getInterval(n,s,closed,i1,i2);

        int i0=(i1==0)?(closed?n:0):i1-1;
        int i3=(i2==n)?(closed?0:n):i2+1;

        return evalPolynomial(v[i0],v[i1],v[i2],v[i3],w,0.5f,s);
    }

  private:

    /// Returns the indices \a i0 and \a i1 for position \a s into an array with
    /// maximum index \a n (the position in range [0,1] is mapped to the array
    /// size). If \a closed is \c true, the array is treated as being periodic,
    /// resulting in the indices to wrap. The return value equals the position
    /// scaled to range [0,1] relative to the interval.
    static float getInterval(int const n,float const s,bool const closed,int& i0,int& i1) {
        G_ASSERT(n>=0)

        // Calculate the array indices from the position.
        float i=(n+static_cast<int>(closed))*s;
        int it=roundToZero(i);

        i0=(it>n)?0:it;
        i1=(i0==n)?(closed?0:n):i0+1;

        return i-it;
    }

    /// Evaluates the cubic polynomial constructed from the values \a v0, \a v1
    /// \a v2, \a v3, the weights in \a w and the factor \a f, at position \a s.
    template<class T>
    static T evalPolynomial(T const& v0,T const& v1,T const& v2,T const& v3,signed char const (&w)[16],float const f,float const s) {
        T a=f*(v0*w[ 0] + v1*w[ 1] + v2*w[ 2] + v3*w[ 3]);
        T b=f*(v0*w[ 4] + v1*w[ 5] + v2*w[ 6] + v3*w[ 7]);
        T c=f*(v0*w[ 8] + v1*w[ 9] + v2*w[10] + v3*w[11]);
        T d=f*(v0*w[12] + v1*w[13] + v2*w[14] + v3*w[15]);

        return ((a*s + b)*s + c)*s + d;
    }
};

} // namespace math

} // namespace gale

#endif // INTERPOLATOR_H
