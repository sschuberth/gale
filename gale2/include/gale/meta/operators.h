/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at https://github.com/sschuberth/gale/.
 *
 * Copyright (C) 2005-2011  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
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

#pragma once

/**
 * \file
 * Meta-template operator implementations
 */

#include "../global/defines.h"
#include "../math/essentials.h"

namespace gale {

namespace meta {

/// Assignment operator.
struct OpAssign
{
    /// Sets \a a to \a b (used e.g. to explicitly cast to type \a A).
    template<typename A,typename B>
    static G_INLINE void evaluate(A& a,B const& b) {
        a=A(b);
    }
};

/// Negating assignment operator.
struct OpAssignNeg
{
    /// Negates \a b and stores the result into \a a.
    template<typename A,typename B>
    static G_INLINE void evaluate(A& a,B const& b) {
        a=A(-b);
    }
};

/// Addition operator.
struct OpArithAdd
{
    /// Adds \a b to \a a.
    template<typename T>
    static G_INLINE void evaluate(T& a,T const& b) {
        a+=b;
    }
};

/// Subtraction operator.
struct OpArithSub
{
    /// Subtracts \a b from \a a.
    template<typename T>
    static G_INLINE void evaluate(T& a,T const& b) {
        a-=b;
    }
};

/// Multiplication operator.
struct OpArithMul
{
    /// Multiplies \a a by \a b.
    template<typename T>
    static G_INLINE void evaluate(T& a,T const& b) {
        a*=b;
    }
};

/// Division operator.
struct OpArithDiv
{
    /// Divides \a a by \a b.
    template<typename T>
    static G_INLINE void evaluate(T& a,T const& b) {
        G_ASSERT(abs(b)>math::Numerics<T>::ZERO_TOLERANCE())
        a/=b;
    }
};

/// Reciprocal operator.
struct OpArithReci
{
    /// Calculates the reciprocal of \a b and stores the result into \a a.
    template<typename T>
    static G_INLINE void evaluate(T& a,T const& b) {
        G_ASSERT(abs(b)>math::Numerics<T>::ZERO_TOLERANCE())
        a=1/b;
    }
};

/// Boolean "less" comparison operator.
struct OpCmpLess
{
    /// Returns whether \a a is less than \a b.
    template<typename T>
    static G_INLINE bool evaluate(T const& a,T const& b) {
        return a<b;
    }
};

/// Boolean "less or equal" comparison operator.
struct OpCmpLessEqual
{
    /// Returns whether \a a is less than or equal to \a b.
    template<typename T>
    static G_INLINE bool evaluate(T const& a,T const& b) {
        return a<=b;
    }
};

/// Boolean "greater" comparison operator.
struct OpCmpGreater
{
    /// Returns whether \a a is greater than \a b.
    template<typename T>
    static G_INLINE bool evaluate(T const& a,T const& b) {
        return a>b;
    }
};

/// Boolean "greater or equal" comparison operator.
struct OpCmpGreaterEqual
{
    /// Returns whether \a a is greater than or equal to \a b.
    template<typename T>
    static G_INLINE bool evaluate(T const& a,T const& b) {
        return a>=b;
    }
};

/// Boolean absolute "equal" comparison operator.
struct OpCmpEqual
{
    /// Returns whether \a a equals \a b with regard to an absolute \a tolerance
    /// which by default depends on the precision of data type \a T.
    template<typename T>
    static G_INLINE bool evaluate(
        T const& a
    ,   T const& b
    ,   T const& tolerance=math::Numerics<T>::ZERO_TOLERANCE()
    )
    {
        // Note that the tolerance may be 0 for integral types!
        return abs(b-a)<=tolerance;
    }
};

/// Boolean absolute and relative "equal" comparison operator, see
/// http://realtimecollisiondetection.net/blog/?p=89.
struct OpCmpEqualRel
{
    /// Returns whether \a a equals \a b with regard to an absolute tolerance
    /// \a tol_abs and a relative tolerance \a tol_rel which by default depend
    /// on the precision of data type \a T.
    template<typename T>
    static G_INLINE bool evaluate(
      T const& a,T const& b,
      T const& tol_abs=math::Numerics<T>::ZERO_TOLERANCE(),
      T const& tol_rel=math::Numerics<T>::ZERO_TOLERANCE())
    {
        // Note that the tolerance may be 0 for integral types!
        return abs(b-a)<=math::max(tol_abs,tol_rel*math::max(abs(a),abs(b)));
    }
};

/// Minimum determination operator.
struct OpCalcMin
{
    /// Returns the minimum of \a a and \a b which need to be of the same data
    /// type so casts (and thus temporary copies) can be avoided.
    template<typename T>
    static G_INLINE T const& evaluate(T const& a,T const& b) {
        return b<a?b:a;
    }
};

/// Maximum determination operator.
struct OpCalcMax
{
    /// Returns the maximum of \a a and \a b which need to be of the same data
    /// type so casts (and thus temporary copies) can be avoided.
    template<typename T>
    static G_INLINE T const& evaluate(T const& a,T const& b) {
        return b>a?b:a;
    }
};

/// Linear interpolation operator.
struct OpCalcLerp
{
    /// Returns the linear interpolation between \a a and \a b based on \a c.
    template<typename T,typename U>
    static G_INLINE T evaluate(T const& a,T const& b,U const& c) {
        G_ASSERT(0<=c && c<=1)
        return T(a+(b-a)*c);
    }
};

/// Product calculation operator.
struct OpCalcProd
{
    /// Returns the product of \a a times \a b.
    template<typename T>
    static G_INLINE T evaluate(T const& a,T const& b) {
        return a*b;
    }
};

} // namespace meta

} // namespace gale
