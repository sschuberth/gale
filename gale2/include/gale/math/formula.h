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

namespace gale {

namespace math {

/**
 * Functor base class for implementing formulas.
 */
struct Formula
{
    /// Define a virtual destructor, as we have a virtual method.
    virtual ~Formula() {}

    /// The call operator to be overloaded with the formula. Defaults to
    /// evaluating the identity.
    virtual float operator()(float x) const {
        return x;
    }
};

/**
 * Functor for a constant formula.
 */
struct Constantformula:public Formula
{
    /// Initializing constructor for the formula's constant parameters.
    Constantformula(float constant)
    :   constant(constant) {}

    /// Evaluator for the formula.
    float operator()(float x) const {
        UNREFERENCED_PARAMETER(x);
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
struct Superformula:public Formula
{
    /// Initializing constructor for the formula's constant parameters.
    Superformula(float m,float n1,float n2,float n3,float a=1.0,float b=1.0)
    :   m(m),n1(n1),n2(n2),n3(n3),a(a),b(b) {}

    /// Evaluator for the formula.
    float operator()(float x) const {
        float ta=::cos(m*x/4.0f)/a;
        ta=::pow(::abs(ta),n2);
        float tb=::sin(m*x/4.0f)/b;
        tb=::pow(::abs(tb),n3);
        return ::pow(ta+tb,-1.0f/n1);
    }

    //@{
    /// Parameters which are constant during evaluation.
    float m,n1,n2,n3,a,b;
    //@}
};

} // namespace math

} // namespace gale

#endif // FORMULA_H
