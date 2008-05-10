/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
 *
 * Copyright (C) 2005-2008  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
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

#include "matrix4.h"
#include "quaternion.h"

#include "../global/types.h"

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

    /// Data type definition for external access to the template argument.
    typedef T Type;

    /**
     * \name Constructors
     */
    //@{

    /// Initializes the generator with a seed value that is derived from the
    /// current system time.
    RandomBase() {
        setSeed(static_cast<g_uint32>(time(NULL)));
    }

    /// Initializes the generator with the given seed value.
    RandomBase(g_uint32 seed) {
        setSeed(seed);
    }

    //@}

    /**
     * \name Methods to be provided by the template implementation classes
     */
    //@{

    /// Sets the generator seed to the given value.
    void setSeed(g_uint32 seed) {
        m_rand.setSeed(seed);
    }

    /// Generates a pseudo random number within the full range of 32 bits.
    g_uint32 getRandom() {
        return m_rand.getRandom();
    }

    //@}

    /**
     * \name Utility methods to generate numbers within certain ranges
     */
    //@{

    /// Returns an integer random number in range [0,range].
    g_uint32 getRandom(g_uint32 const range) {
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
            n=getRandom()&mask;
        } while (n>range);

        return n;
    }

    /// Returns a floating-point random number in range [0,1].
    T getRandom01() {
        static T const s=1/T(UINT_MAX);
        return getRandom()*s;
    }

    /// Returns a floating-point random number in range [0,range].
    T getRandom0N(T const range) {
        return getRandom01()*range;
    }

    /// Returns a floating-point random number in range [0,1[.
    T getRandom0Excl1() {
        static T const s=1/(T(UINT_MAX)+1);
        return getRandom()*s;
    }

    /// Returns a floating-point random number in range [0,range[.
    T getRandom0ExclN(T const range) {
        return getRandom0Excl1()*range;
    }

    /// Returns a floating-point random number in range ]0,1].
    T getRandomExcl01() {
        static T const s=1/(T(UINT_MAX)+1);
        return T(getRandom()+0.5)*s;
    }

    /// Returns a floating-point random number in range ]0,range].
    T getRandomExcl0N(T const range) {
        return getRandomExcl01()*range;
    }

    //@}

    /**
     * \name Utility methods to generate certain data types
     */
    //@{

    /// Returns a normalized random 3-component vector.
    Vector<3,T> getVector3() {
        Vector<3,T> v;

        do {
            v.setX(getRandom0N(2)-1);
            v.setY(getRandom0N(2)-1);
            v.setZ(getRandom0N(2)-1);
        } while (v.getLengthSquared()>1);

        return v.normalize();
    }

    /// Returns a normalized random 4-component vector.
    Vector<4,T> getVector4() {
        Vector<4,T> v;

        do {
            v.setX(getRandom0N(2)-1);
            v.setY(getRandom0N(2)-1);
            v.setZ(getRandom0N(2)-1);
            v.setW(getRandom0N(2)-1);
        } while (v.getLengthSquared()>1);

        return v.normalize();
    }

    /// Returns a random orthonormalized 4x4 homogeneous matrix.
    HMatrix4<T> getHMatrix4() {
        HMatrix4<T> m(
            getVector3(),
            getVector3(),
            getVector3()
        );
        return m.orthonormalize();
    }

    /// Returns a random invertible 4x4 matrix.
    Matrix4<T> getMatrix4() {
        Matrix4<T> m;
        do {
            m.c0=getVector4();
            m.c1=getVector4();
            m.c2=getVector4();
            m.c3=getVector4();
        } while (abs(m.getDeterminant())<=std::numeric_limits<T>::epsilon());
        return m;
    }

    /// Returns a random quaternion.
    Quaternion<T> getQuaternion() {
        return Quaternion<T>(getVector3(),getRandom0ExclN(T(2*M_PI)));
    }

    //@}

  private:

    C m_rand; ///< Instance of the random generator template implementation.
};

#include "random_ecuyer.inl"

} // namespace math

} // namespace gale

#endif // RANDOM_H
