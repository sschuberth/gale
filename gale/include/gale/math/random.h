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

#ifndef RANDOM_H
#define RANDOM_H

#include <time.h>

#include "quaternion.h"

/**
 * \file
 * Pseudo Random Number Generators (PRNG)
 */

namespace gale {

namespace math {

/**
 * Random number generator template base class that provides common methods.
 */
template<typename T,class C>
class RandomBase
{
  public:

    /**
     * \name Constructors
     */
    //@{

    /// Initializes the generator with a seed value that is derived from the
    /// current system time.
    RandomBase() {
        setSeed(static_cast<unsigned int>(time(NULL)));
    }

    /// Initializes the generator with the given seed value.
    RandomBase(unsigned int seed) {
        setSeed(seed);
    }

    //@}

    /**
     * \name Methods to be provided by the template implementation classes
     */
    //@{

    /// Sets the generator seed to the given value.
    void setSeed(unsigned int seed) {
        m_rand.setSeed(seed);
    }

    /// Generates a pseudo random number within the full range of 32 bits.
    unsigned int getRandom() {
        return m_rand.getRandom();
    }

    //@}

    /**
     * \name Utility methods to generate numbers within certain ranges
     */
    //@{

    /// Returns an integer random number in range [0,range].
    unsigned int getRandom(unsigned int range) {
        // Mask all bits starting from the MSB set in the range.
        unsigned int mask=range;
        mask|=mask>>1;
        mask|=mask>>2;
        mask|=mask>>4;
        mask|=mask>>8;
        mask|=mask>>16;

        // Draw numbers until one in range [0,range] is found.
        unsigned int n;
        do {
            n=getRandom()&mask;
        } while (n>range);
        return n;
    }

    /// Returns a floating-point random number in range [0,1].
    T getRandom01() {
        static T const s=static_cast<T>(1)/4294967295;
        return getRandom()*s;
    }

    /// Returns a floating-point random number in range [0,range].
    T getRandom0N(T range) {
        return getRandom01()*range;
    }

    /// Returns a floating-point random number in range [0,1[.
    T getRandom0Excl1() {
        static T const s=static_cast<T>(1)/4294967296;
        return getRandom()*s;
    }

    /// Returns a floating-point random number in range [0,range[.
    T getRandom0ExclN(T range) {
        return getRandom0Excl1()*range;
    }

    /// Returns a floating-point random number in range ]0,1].
    T getRandomExcl01() {
        static T const s=static_cast<T>(1)/4294967296;
        return static_cast<T>(getRandom()+0.5)*s;
    }

    /// Returns a floating-point random number in range ]0,range].
    T getRandomExcl0N(T range) {
        return getRandomExcl01()*range;
    }

    /// Returns a random normalized vector.
    Vector<3,T> getVector() {
        Vector<3,T> v;
        do {
            v.setX(getRandom0N(2)-1);
            v.setY(getRandom0N(2)-1);
            v.setZ(getRandom0N(2)-1);
        } while (v.getLengthSquared()>1);
        return v.normalize();
    }

    /// Returns a random normalized quaternion.
    Quaternion<T> getQuaternion() {
        return Quaternion<T>(getRandom0ExclN(static_cast<T>(2*M_PI)),getVector());
    }

    //@}

  private:

    C m_rand; ///< Instance of the random generator template implementation.
};

#include "randomecuyer.inl"

} // namespace math

} // namespace gale

#endif // RANDOM_H