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
 * Pseudo Random Number Generators (PRNG)
 */

#include "../global/types.h"

#include "essentials.h"

#ifndef GALE_TINY_CODE
    #include <time.h>
#endif

namespace gale {

namespace math {

/**
 * Random number generator template base class for the data type specified by
 * the \c T template argument. It provides common wrapper methods around the
 * implementation passed as the \c I template argument.
 */
template<typename T,class I>
class RandomBase
{
  public:

    /// Definition for external access to the data type.
    typedef T Type;

    /**
     * \name Constructors
     */
    //@{

    /// Initializes the generator with a seed value that is derived from the
    /// current system time.
    RandomBase() {
#ifndef GALE_TINY_CODE
        init(static_cast<g_uint32>(time(NULL)));
#else
        init(0xdeadbeefUL);
#endif
    }

    /// Initializes the generator with the given seed value.
    RandomBase(g_uint32 const seed) {
        init(seed);
    }

    //@}

    /**
     * \name Methods to be provided by the template implementation classes
     */
    //@{

    /// Sets the generator seed to the given value.
    void init(g_uint32 const seed) {
        m_impl.init(seed);
    }

    /// Generates a pseudo random number within the full range of 32 bits.
    g_uint32 random() {
        return m_impl.random();
    }

    //@}

    /**
     * \name Utility methods to generate numbers within certain ranges
     */
    //@{

    /// Returns an integer random number in range [0,range].
    g_uint32 random(g_uint32 const range) {
        // Mask all bits starting from the MSB set in the range.
        g_uint32 mask=range;
        mask|=mask>>1;
        mask|=mask>>2;
        mask|=mask>>4;
        mask|=mask>>8;
        mask|=mask>>16;

        // Draw numbers until one in range [0,range] is found.
        g_uint32 n;
        do {
            n=random()&mask;
        } while (n>range);

        return n;
    }

    /// Returns a floating-point random number in range [0,1].
    T random01() {
        static T const s=1/T(UINT_MAX);
        return random()*s;
    }

    /// Returns a floating-point random number in range [0,range].
    T random0N(T const range) {
        return random01()*range;
    }

    /// Returns a floating-point random number in range [0,1[.
    T random0Excl1() {
        static T const s=1/(T(UINT_MAX)+1);
        return random()*s;
    }

    /// Returns a floating-point random number in range [0,range[.
    T random0ExclN(T const range) {
        return random0Excl1()*range;
    }

    /// Returns a floating-point random number in range ]0,1].
    T randomExcl01() {
        static T const s=1/(T(UINT_MAX)+1);
        return T(random()+0.5)*s;
    }

    /// Returns a floating-point random number in range ]0,range].
    T randomExcl0N(T const range) {
        return randomExcl01()*range;
    }

    //@}

  private:

    I m_impl; ///< Instance of the random generator implementation.
};

#include "random_ecuyer.inl"
#include "random_mother.inl"

} // namespace math

} // namespace gale
