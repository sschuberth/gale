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

#ifndef BIASSCALE_H
#define BIASSCALE_H

/**
 * \file
 * Helper classes to transform values
 */

#include "../meta/operators.h"

namespace gale {

namespace math {

template<typename T>
struct ScaleBias;

/**
 * Transformation class that applies first a bias and then a scale to a value.
 */
template<typename T>
struct BiasScale
{
    /**
     * \name Predefined constants
     * In order to avoid the so called "static initialization order fiasco",
     * static methods instead of static variables are used here (see
     * http://www.parashift.com/c++-faq-lite/ctors.html#faq-10.14).
     */
    //@{

    /// Returns the identity transformation that does not alter the value.
    static BiasScale const& IDENTITY() {
        static BiasScale const bs(0,1);
        return bs;
    }

    //@}

    /**
     * \name Constructors
     */
    //@{

    /// Initializes the transformation to the given \a bias and \a scale which
    /// are the identity by default.
    BiasScale(T bias=0,T scale=1):
      bias(bias),scale(scale)
    {
    }

    /// Initializes the transformation from a transformation that applies the
    /// scale first and then the bias.
    explicit BiasScale(ScaleBias<T> const& other);

    //@}

    /**
     * \name Miscellaneous methods
     */
    //@{

    /// Returns whether transformation \a a equals transformation \a b.
    friend bool operator==(BiasScale const& a,BiasScale const& b) const {
        return OpCmpEqualEps::evaluate(a.bias,b.bias)
            && OpCmpEqualEps::evaluate(a.scale,b.scale);
    }

    /// Returns whether this transformation is the identity and thus performs a
    /// no-operation.
    bool isNOP() const {
        return *this==IDENTITY();
    }

    /// Applies the transformation to a value \a x.
    T apply(T x) const {
        return (x+bias)*scale;
    }

    /// Concatenates the \a other transformation to this transformation.
    BiasScale getConcat(BiasScale const& other) const {
        return BiasScale(bias+other.bias/scale,scale*other.scale);
    }

    /// Returns the inverse to this transformation so that their concatenation
    /// would be the identity transformation.
    ScaleBias<T> getInverse() const;

    //@}

    T bias;  ///< The bias value that is added to the operand value.
    T scale; ///< The scale value that is multiplied with the operand value.
};

/**
 * Transformation class that applies first a scale and then a bias to a value.
 */
template<typename T>
struct ScaleBias
{
    /**
     * \name Predefined constants
     * In order to avoid the so called "static initialization order fiasco",
     * static methods instead of static variables are used here (see
     * http://www.parashift.com/c++-faq-lite/ctors.html#faq-10.14).
     */
    //@{

    /// Returns the identity transformation that does not alter the value.
    static ScaleBias const& IDENTITY() {
        static ScaleBias const sb(1,0);
        return sb;
    }

    //@}

    /**
     * \name Constructors
     */
    //@{

    /// Initializes the transformation to the given \a scale and \a bias which
    /// are the identity by default.
    ScaleBias(T scale=1,T bias=0):
      scale(scale),bias(bias)
    {
    }

    /// Initializes the transformation from a transformation that applies the
    /// bias first and then the scale.
    explicit ScaleBias(BiasScale<T> const& other) {
        bias=other.bias*other.scale;
        scale=other.scale;
    }

    //@}

    /**
     * \name Miscellaneous methods
     */
    //@{

    /// Returns whether transformation \a a equals transformation \a b.
    friend bool operator==(ScaleBias const& a,ScaleBias const& b) const {
        return OpCmpEqualEps::evaluate(a.scale,b.scale)
            && OpCmpEqualEps::evaluate(a.bias,b.bias);
    }

    /// Returns whether this transformation is the identity and thus performs a
    /// no-operation.
    bool isNOP() const {
        return *this==IDENTITY();
    }

    /// Applies the transformation to a value \a x.
    T apply(T x) const {
        return x*scale+bias;
    }

    /// Concatenates the \a other transformation to this transformation.
    ScaleBias getConcat(ScaleBias const& other) const {
        return ScaleBias(scale*other.scale,bias*other.scale+other.bias);
    }

    /// Returns the inverse to this transformation so that their concatenation
    /// would be the identity transformation.
    BiasScale<T> getInverse() const {
        if (scale==0) {
            return BiasScale<T>(-bias,1);
        }
        return BiasScale<T>(-bias,T(1)/scale);
    }

    //@}

    T scale; ///< The scale value that is multiplied with the operand value.
    T bias;  ///< The bias value that is added to the operand value.
};

template<typename T>
inline BiasScale<T>::BiasScale(ScaleBias<T> const& other) {
    bias=other.bias/other.scale;
    scale=other.scale;
}

template<typename T>
inline ScaleBias<T> BiasScale<T>::getInverse() const {
    if (scale==0) {
        return ScaleBias<T>(0,0);
    }
    return ScaleBias<T>(T(1)/scale,-bias);
}

/**
 * \name Convenience type definitions
 */
//@{

typedef BiasScale<double> BiasScaled;
typedef BiasScale<float> BiasScalef;

typedef ScaleBias<double> ScaleBiasd;
typedef ScaleBias<float> ScaleBiasf;

//@}

} // namespace math

} // namespace gale

#endif // BIASSCALE_H
