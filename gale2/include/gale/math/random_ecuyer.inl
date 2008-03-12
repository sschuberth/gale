/**
 * Random number generator class based on the maximally equidistributed combined
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

    /// Sets the generator seed to the given value.
    void setSeed(unsigned int const seed) {
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

        // We skip the "warm-up" here so save some code size and performance.
    }

    /// Generates a pseudo random number within the full range of 32 bits.
    unsigned int getRandom() {
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

    unsigned int seed0; ///< The first seed number.
    unsigned int seed1; ///< The second seed number.
    unsigned int seed2; ///< The third seed number.

    //@}
};

/**
 * \name Type definitions for convenience
 */
//@{

typedef RandomBase<double,RandomEcuyerImpl> RandomEcuyerd;
typedef RandomBase<float,RandomEcuyerImpl> RandomEcuyerf;

//@}
