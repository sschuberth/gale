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
    /// Definition for external access to the data type.
    typedef T Type;

    /**
     * \name Predefined constants
     * In order to avoid the so called "static initialization order fiasco",
     * static methods instead of static variables are used here, see
     * <http://www.parashift.com/c++-faq-lite/ctors.html#faq-10.14>.
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
    BiasScale(T const bias=0,T const scale=1)
    :   bias(bias)
    ,   scale(scale)
    {}

    /// Initializes the transformation from a transformation that applies the
    /// scale first and then the bias.
    explicit BiasScale(ScaleBias<T> const& other);

    //@}

    /**
     * \name Miscellaneous methods
     */
    //@{

    /// Returns whether transformation \a a equals transformation \a b.
    friend bool operator==(BiasScale const& a,BiasScale const& b) {
        return meta::OpCmpEqual::evaluate(a.bias,b.bias)
            && meta::OpCmpEqual::evaluate(a.scale,b.scale);
    }

    /// Returns whether this transformation is the identity and thus performs a
    /// no-operation.
    bool isNOP() const {
        return *this==IDENTITY();
    }

    /// Evaluates the transformation for value \a x.
    T evaluate(T const x) const {
        return (x+bias)*scale;
    }

    /// Concatenates the \a other transformation to this transformation.
    void concat(BiasScale const& other) {
        bias+=other.bias/scale;
        scale*=other.scale;
    }

    /// Inverts this transformation so that its concatenation with the original
    /// transformation would result in no-operation.
    void invert();

    //@}

    /**
     * \name Convenience operators for named methods
     */
    //@{

    /// Returns the concatenation of transformations \a a and \a b.
    friend BiasScale operator&(BiasScale const& a,BiasScale const& b) {
        BiasScale tmp=a;
        tmp.concat(b);
        return tmp;
    }

    /// Returns the inverse of transformation \a other so that its concatenation
    /// with the original transformation would result in no-operation.
    friend BiasScale operator!(BiasScale const& other) {
        BiasScale tmp=other;
        tmp.invert();
        return tmp;
    }

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
    /// Definition for external access to the data type.
    typedef T Type;

    /**
     * \name Predefined constants
     * In order to avoid the so called "static initialization order fiasco",
     * static methods instead of static variables are used here, see
     * <http://www.parashift.com/c++-faq-lite/ctors.html#faq-10.14>.
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
    ScaleBias(T const scale=1,T const bias=0)
    :   scale(scale)
    ,   bias(bias)
    {}

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
    friend bool operator==(ScaleBias const& a,ScaleBias const& b) {
        return meta::OpCmpEqual::evaluate(a.scale,b.scale)
            && meta::OpCmpEqual::evaluate(a.bias,b.bias);
    }

    /// Returns whether this transformation is the identity and thus performs a
    /// no-operation.
    bool isNOP() const {
        return *this==IDENTITY();
    }

    /// Evaluates the transformation for value \a x.
    T evaluate(T const x) const {
        return x*scale+bias;
    }

    /// Concatenates the \a other transformation to this transformation.
    void concat(ScaleBias const& other) {
        scale*=other.scale;
        bias=bias*other.scale+other.bias;
    }

    /// Inverts this transformation so its concatenation with the original
    /// transformation would result in no-operation.
    void invert() {
        if (meta::OpCmpEqual::evaluate(scale,T(0))) {
            *this=IDENTITY();
        }
        else {
            bias=-bias/scale;
            scale=T(1)/scale;
        }
    }

    //@}

    /**
     * \name Convenience operators for named methods
     */
    //@{

    /// Returns the concatenation of transformations \a a and \a b.
    friend ScaleBias operator&(ScaleBias const& a,ScaleBias const& b) {
        ScaleBias tmp=a;
        tmp.concat(b);
        return tmp;
    }

    /// Returns the inverse of transformation \a other so that its concatenation
    /// with the original transformation would result in no-operation.
    friend ScaleBias operator!(ScaleBias const& other) {
        ScaleBias tmp=other;
        tmp.invert();
        return tmp;
    }

    //@}

    T scale; ///< The scale value that is multiplied with the operand value.
    T bias;  ///< The bias value that is added to the operand value.
};

template<typename T>
inline BiasScale<T>::BiasScale(ScaleBias<T> const& other)
{
    bias=other.bias/other.scale;
    scale=other.scale;
}

template<typename T>
inline void BiasScale<T>::invert()
{
    if (meta::OpCmpEqual::evaluate(scale,T(0))) {
        *this=IDENTITY();
    }
    else {
        scale=T(1)/scale;
        bias=-bias/scale;
    }
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
