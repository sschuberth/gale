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

    /// Linear interpolation of the values in \a v at position \a s. The
    /// position in range [0,1] is mapped to the array size. \a periodic toggles
    /// between natural and periodic boundary conditions. Also see
    /// <http://local.wasp.uwa.edu.au/~pbourke/miscellaneous/interpolation/>.
    template<class T>
    static T Linear(global::DynamicArray<T> const& v,float const s,bool periodic=false) {
        G_ASSERT(v.getSize()>0)

        int n=v.getSize()-1;

        // Calculate the interval borders from the pseudo index.
        float i=n*s;
        int i1=roundToZero(i);
        int i2=(i1==n)?(periodic?0:n):i1+1;

        // Interpolate with a new "s" which is scaled to the interval.
        return lerp(v[i1],v[i2],s*n-i1);
    }

    /// Cosine interpolation of the values in \a v at position \a s. The
    /// position in range [0,1] is mapped to the array size. \a periodic toggles
    /// between natural and periodic boundary conditions. Also see
    /// <http://local.wasp.uwa.edu.au/~pbourke/miscellaneous/interpolation/>.
    template<class T>
    static T Cosine(global::DynamicArray<T> const& v,float const s,bool periodic=false) {
        G_ASSERT(v.getSize()>0)

        int n=v.getSize()-1;

        // Calculate the interval borders from the pseudo index.
        float i=n*s;
        int i1=roundToZero(i);
        int i2=(i1==n)?(periodic?0:n):i1+1;

        // Interpolate with a new "s" which is scaled to the interval.
        float t=(1.0f-cos((s*n-i1)*Constf::PI()))*0.5f;
        return lerp(v[i1],v[i2],t);
    }

    /// B-Spline approximation of the values in \a v at position \a s. The
    /// position in range [0,1] is mapped to the array size. \a periodic toggles
    /// between natural and periodic boundary conditions. Also see
    /// <http://blackpawn.com/texts/splines/>.
    template<class T>
    static T BSpline(global::DynamicArray<T> const& v,float const s,bool periodic=false) {
        static signed char const w[]={
            -1, 3,-3, 1
        ,    3,-6, 3, 0
        ,   -3, 0, 3, 0
        ,    1, 4, 1, 0
        };

        return Cubic(v,s,periodic,w,1.0f/6.0f);
    }

    /// Catmull-Rom interpolation of the values in \a v at position \a s. The
    /// position in range [0,1] is mapped to the array size. \a periodic toggles
    /// between natural and periodic boundary conditions. Also see
    /// <http://blackpawn.com/texts/splines/>.
    template<class T>
    static T CatmullRom(global::DynamicArray<T> const& v,float const s,bool periodic=false) {
        static signed char const w[]={
            -1, 3,-3, 1
        ,    2,-5, 4,-1
        ,   -1, 0, 1, 0
        ,    0, 2, 0, 0
        };

        return Cubic(v,s,periodic,w,0.5f);
    }

  private:

    /// Cubic interpolation of the values in \a v at position \a s. The position
    /// in range [0,1] is mapped to the array size. \a periodic toggles between
    /// natural and periodic boundary conditions. The coefficients are calculated
    /// using the weights in \a w and the factor \a f.
    template<class T>
    static T Cubic(global::DynamicArray<T> const& v,float const s,bool periodic,signed char const (&w)[16],float f) {
        G_ASSERT(v.getSize()>0)

        int n=v.getSize()-1;

        // Calculate the interval borders from the pseudo index.
        float i=n*s;
        int i1=roundToZero(i);
        int i2=(i1==n)?(periodic?0:n):i1+1;

        int i0=(i1==0)?(periodic?n:0):i1-1;
        int i3=(i2==n)?(periodic?0:n):i2+1;

        T a=f*(v[i0]*w[ 0] + v[i1]*w[ 1] + v[i2]*w[ 2] + v[i3]*w[ 3]);
        T b=f*(v[i0]*w[ 4] + v[i1]*w[ 5] + v[i2]*w[ 6] + v[i3]*w[ 7]);
        T c=f*(v[i0]*w[ 8] + v[i1]*w[ 9] + v[i2]*w[10] + v[i3]*w[11]);
        T d=f*(v[i0]*w[12] + v[i1]*w[13] + v[i2]*w[14] + v[i3]*w[15]);

        // Interpolate with a new "s" which is scaled to the interval.
        float t=(s*n-i1);
        return ((a*t + b)*t + c)*t + d;
    }
};

} // namespace math

} // namespace gale

#endif // INTERPOLATOR_H
