/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
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

/**
 * A random number generator based on the maximally equidistributed combined
 * Tausworthe generator by L'Ecuyer. The period of the generator is 2^88 (about
 * 10^26) and it uses only three 32-bit integers to store its state, see
 * <http://root.cern.ch/root/html/TRandom2.html>.
 */
struct RandomEcuyerImpl
{
    /**
     * \name Methods required by the base class template
     */
    //@{

    /// Initializes the random generator with the given \a seed value.
    void init(g_uint32 const seed) {
        // Generate the internal seeds needed for the generator state using a
        // Linear Congruential Generator. The only condition, stated at the end
        // of the 1999 L'Ecuyer paper, is that the seeds must be greater than 1,
        // 7 and 15.

// Linear Congruential Generator
#define LCG(n) ((69069*n)&0xffffffffUL)

        seed0=LCG(seed);
        if (seed0<2) {
            seed0+=2;
        }

        seed1=LCG(seed0);
        if (seed1<8) {
            seed1+=8;
        }

        seed2=LCG(seed1);
        if (seed2<16) {
            seed2+=16;
        }

#undef LCG

#ifndef GALE_TINY_CODE
        // "Warm up" the random generator.
        for (g_int32 i=0;i<6;++i) {
            random();
        }
#endif
    }

    /// Generates a pseudo random number within the full range of 32 bits.
    g_uint32 random() {
// Use a mask of 0xffffffffUL to make in work on 64-bit machines.
#define TAUSWORTHE(s,a,b,c,d) (((s&c)<<d)&0xffffffffUL)^((((s<<a)&0xffffffffUL)^s)>>b)

        seed0=TAUSWORTHE(seed0, 13, 19, 4294967294UL, 12);
        seed1=TAUSWORTHE(seed1,  2, 25, 4294967288UL,  4);
        seed2=TAUSWORTHE(seed2,  3, 11, 4294967280UL, 17);

        // The original implementation does not allow 0 to be returned and
        // generates a new number in that case.
        return seed0^seed1^seed2;

#undef TAUSWORTHE
    }

    //@}

    /**
     * \name Internal state variables
     */
    //@{

    g_uint32 seed0; ///< The first seed number.
    g_uint32 seed1; ///< The second seed number.
    g_uint32 seed2; ///< The third seed number.

    //@}
};

/**
 * \name Type definitions for convenience
 */
//@{

typedef RandomBase<g_real64,RandomEcuyerImpl> RandomEcuyerd;
typedef RandomBase<g_real32,RandomEcuyerImpl> RandomEcuyerf;

//@}
