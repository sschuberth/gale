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

#ifndef RANDOM_H
#define RANDOM_H

/**
 * \file
 * Pseudo Random Number Generators (PRNG)
 */

#include "../global/types.h"

#include "matrix4.h"
#include "quaternion.h"

#ifndef GALE_TINY_CODE
    #include <time.h>
#endif

namespace gale {

namespace math {

/**
 * Random number generator template base class for the data type specified by
 * the \c T template argument. It provides common wrapper methods around the
 * implementation passed as the \c C template argument.
 */
template<typename T,class C>
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
    RandomBase(g_uint32 seed) {
        init(seed);
    }

    //@}

    /**
     * \name Methods to be provided by the template implementation classes
     */
    //@{

    /// Sets the generator seed to the given value.
    void init(g_uint32 seed) {
        m_rand.init(seed);
    }

    /// Generates a pseudo random number within the full range of 32 bits.
    g_uint32 random() {
        return m_rand.random();
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

    /**
     * \name Utility methods to generate certain data types
     */
    //@{

    /// Returns a normalized random 3-component vector.
    Vector<3,T> randomVec3() {
        Vector<3,T> v;

        do {
            v.setX(random0N(2)-1);
            v.setY(random0N(2)-1);
            v.setZ(random0N(2)-1);
        } while (v.length2()>1);

        return v.normalize();
    }

    /// Returns a normalized random 4-component vector.
    Vector<4,T> randomVec4() {
        Vector<4,T> v;

        do {
            v.setX(random0N(2)-1);
            v.setY(random0N(2)-1);
            v.setZ(random0N(2)-1);
            v.setW(random0N(2)-1);
        } while (v.length2()>1);

        return v.normalize();
    }

    /// Returns a random orthonormalized 4x4 homogeneous matrix.
    HMatrix4<T> randomHMat4() {
        HMatrix4<T> m(
            randomVec3(),
            randomVec3(),
            randomVec3()
        );
        return m.orthonormalize();
    }

    /// Returns a random invertible 4x4 matrix.
    Matrix4<T> randomMat4() {
        Matrix4<T> m;
        do {
            m.c0=randomVec4();
            m.c1=randomVec4();
            m.c2=randomVec4();
            m.c3=randomVec4();
        } while (abs(m.determinant())<=Numerics<T>::ZERO_TOLERANCE());
        return m;
    }

    /// Returns a random quaternion.
    Quaternion<T> randomQuat() {
        return Quaternion<T>(randomVec3(),random0ExclN(2*Constants<T>::PI()));
    }

    //@}

  private:

    C m_rand; ///< Instance of the random generator implementation.
};

#include "random_ecuyer.inl"
#include "random_mother.inl"

} // namespace math

} // namespace gale

#endif // RANDOM_H
