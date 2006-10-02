/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at http://developer.berlios.de/projects/gale/
 *
 * Copyright (C) 2005-2006  Sebastian Schuberth <sschuberth@gmail.com>
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

#ifndef OPERATORS_H
#define OPERATORS_H

/**
 * \file
 * Meta-template operator implementations.
 */

#include <cmath>
#include <limits>

#include "../global/defines.h"

namespace gale {

namespace meta {

/// Assignment operator.
struct OpAssign {
    /// Sets \a a to \a b (used e.g. to explicitly cast to type \a A).
    template<typename A,typename B>
    static G_INLINE void evaluate(A& a,B const& b) {
        a=A(b);
    }
};

/// Negating assignment operator.
struct OpAssignNeg {
    /// Negates \a b and stores the result into \a a.
    template<typename A,typename B>
    static G_INLINE void evaluate(A& a,B const& b) {
        a=A(-b);
    }
};

/// Increment operator.
struct OpArithInc {
    /// Adds \a b to \a a.
    template<typename A,typename B>
    static G_INLINE void evaluate(A& a,B const& b) {
        a+=A(b);
    }
};

/// Decrement operator.
struct OpArithDec {
    /// Subtracts \a b from \a a.
    template<typename A,typename B>
    static G_INLINE void evaluate(A& a,B const& b) {
        a-=A(b);
    }
};

/// Multiplication operator.
struct OpArithMul {
    /// Multiplies \a a by \a b.
    template<typename A,typename B>
    static G_INLINE void evaluate(A& a,B const& b) {
        a*=A(b);
    }
};

/// Reciprocal operator.
struct OpArithReci {
    /// Calculates the reciprocal of \a b and stores the result into \a a.
    template<typename A,typename B>
    static G_INLINE void evaluate(A& a,B const& b) {
        G_ASSERT(std::abs(b)>std::numeric_limits<double>::epsilon())
        a=A(1.0/b);
    }
};

/// Boolean "less" comparison operator.
struct OpCmpLess {
    /// Returns whether \a a is less than \a b.
    template<typename A,typename B>
    static G_INLINE bool evaluate(A const& a,B const& b) {
        return a<b;
    }
};

/// Boolean "less or equal" comparison operator.
struct OpCmpLessEqual {
    /// Returns whether \a a is less than or equal to \a b.
    template<typename A,typename B>
    static G_INLINE bool evaluate(A const& a,B const& b) {
        return a<=b;
    }
};

/// Boolean "greater" comparison operator.
struct OpCmpGreater {
    /// Returns whether \a a is greater than \a b.
    template<typename A,typename B>
    static G_INLINE bool evaluate(A const& a,B const& b) {
        return a>b;
    }
};

/// Boolean "greater or equal" comparison operator.
struct OpCmpGreaterEqual {
    /// Returns whether \a a is greater than or equal to \a b.
    template<typename A,typename B>
    static G_INLINE bool evaluate(A const& a,B const& b) {
        return a>=b;
    }
};

/// Boolean "equal" comparison operator.
struct OpCmpEqualEps {
    /// Returns whether \a a equals \a b using an epsilon-environment depending
    /// on the data type's precision.
    template<typename T>
    static G_INLINE bool evaluate(
      T const& a,T const& b,
      T const& epsilon=std::numeric_limits<T>::epsilon())
    {
        return std::abs(b-a)<=epsilon;
    }
};

/// Minimum determination operator.
struct OpCalcMin {
    /// Returns the minimum of \a a and \a b which need to be of the same data
    /// type so casts (and thus temporary copies) can be avoided.
    template<typename T>
    static G_INLINE T const& evaluate(T const& a,T const& b) {
        return b<a?b:a;
    }
};

/// Maximum determination operator.
struct OpCalcMax {
    /// Returns the maximum of \a a and \a b which need to be of the same data
    /// type so casts (and thus temporary copies) can be avoided.
    template<typename T>
    static G_INLINE T const& evaluate(T const& a,T const& b) {
        return b>a?b:a;
    }
};

/// Linear interpolation operator.
struct OpCalcLerp {
    /// Returns the linear interpolation between \a a and \a b based on \a c,
    /// casted to type \a A.
    template<typename A,typename B,typename C>
    static G_INLINE A evaluate(A const& a,B const& b,C const& c) {
        return A(a+(b-a)*c);
    }
};

/// Product calculation operator.
struct OpCalcProd {
    /// Returns the product of \a a times \a b.
    template<typename A,typename B>
    static G_INLINE A evaluate(A const& a,B const& b) {
        return A(a*b);
    }
};

} // namespace meta

} // namespace gale

#endif // OPERATORS_H
