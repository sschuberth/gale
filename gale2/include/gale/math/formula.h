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

#ifndef FORMULA_H
#define FORMULA_H

/**
 * \file
 * Mathematical formulas
 */

#include "vector.h"

namespace gale {

namespace math {

/**
 * Functor base class for implementing formulas that map R1 to R1.
 */
struct Formula
{
    /// Define a virtual destructor, as we have a virtual method.
    virtual ~Formula() {}

    /// The call operator to be overloaded with the formula. Defaults to
    /// evaluating to \a x, i.e. the identity.
    virtual float operator()(float const x) const {
        return x;
    }
};

/**
 * Functor base class for implementing formulas that map R2 to R3.
 */
struct FormulaR2R3
{
    /// Define a virtual destructor, as we have a virtual method.
    virtual ~FormulaR2R3() {}

    /// The call operator to be overloaded with the formula. Defaults to
    /// extending \a v with 0 as the third component.
    virtual Vec3f operator()(Vec2f const& v) const {
        return Vec3f(v.getX(),v.getY(),0);
    }
};

/**
 * Functor for a constant formula.
 */
struct ConstantFormula:public Formula
{
    /// Initializing constructor for the formula's constant parameters.
    ConstantFormula(float const constant)
    :   constant(constant)
    {}

    /// Evaluator for the formula.
    float operator()(float const x) const {
        G_UNREF_PARAM(x)
        return constant;
    }

    //@{
    /// Parameters which are constant during evaluation.
    float constant;
    //@}
};

/**
 * Functor for the generalized Superformula as described by J. Gielis, see
 * <http://mathworld.wolfram.com/Superellipse.html>.
 */
struct SuperFormula:public Formula
{
    /// Initializing constructor for the formula's constant parameters.
    SuperFormula(float const m,float const n1,float const n2,float const n3,float const a=1.0,float const b=1.0)
    :   m(m)
    ,   n1(n1),n2(n2),n3(n3)
    ,   a(a),b(b)
    {}

    /// Evaluator for the formula.
    float operator()(float const x) const {
        float ta=cos(m*x/4.0f)/a;
        ta=pow(abs(ta),n2);
        float tb=sin(m*x/4.0f)/b;
        tb=pow(abs(tb),n3);
        return pow(ta+tb,-1.0f/n1);
    }

    //@{
    /// Parameters which are constant during evaluation.
    float m,n1,n2,n3,a,b;
    //@}
};

} // namespace math

} // namespace gale

#endif // FORMULA_H
