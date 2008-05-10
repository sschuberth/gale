/**
 * Multiply-with-carry random number generator as invented by George Marsaglia
 * and implemented by Agner Fog, see <http://www.agner.org/random/mother/>.
 */
struct RandomMotherImpl
{
    /**
     * \name Methods required by the base class template
     */
    //@{

    /// Sets the generator seed to the given value.
    void setSeed(g_uint32 seed) {
        g_int32 i;

        // Make random numbers and put them into the buffer.
        for (i=0;i<G_ARRAY_LENGTH(x);++i) {
            seed=seed*29943829-1;
            x[i]=seed;
        }

#ifndef GALE_TINY
        // "Warm up" the random generator.
        for (i=0;i<19;++i) {
            getRandom();
        }
#endif
    }

    /// Generates a pseudo random number within the full range of 32 bits.
    g_uint32 getRandom() {
        g_uint64 sum = g_uint64(2111111111ul)*g_uint64(x[3])
                     + g_uint64(        1492)*g_uint64(x[2])
                     + g_uint64(        1776)*g_uint64(x[1])
                     + g_uint64(        5115)*g_uint64(x[0])
                     +                        g_uint64(x[4]);

        x[3]=x[2];
        x[2]=x[1];
        x[1]=x[0];

        x[4]=g_uint32(sum>>32); // Carry bits.
        x[0]=g_uint32(sum);     // Low 32 bits of the sum.

        return x[0];
    }

    //@}

    /**
     * \name Internal state variables
     */
    //@{

    g_uint32 x[5]; ///< History buffer.

    //@}
};

/**
 * \name Type definitions for convenience
 */
//@{

typedef RandomBase<g_real64,RandomMotherImpl> RandomMotherd;
typedef RandomBase<g_real32,RandomMotherImpl> RandomMotherf;

//@}
