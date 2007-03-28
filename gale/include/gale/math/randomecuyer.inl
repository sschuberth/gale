/**
 * Random number generator class based on the maximally equidistributed combined
 * Tausworthe generator by L'Ecuyer. The period of the generator is 2^88 (about
 * 10^26) and it uses only three 32-bit integers to store its state (see
 * http://root.cern.ch/root/html/TRandom2.html).
 */
struct RandomEcuyerImpl
{
    /**
     * \name Methods required by the base class template
     */
    //@{

    /// Sets the generator seed to the given value.
    void setSeed(unsigned int seed) {
        // Generate the internal seeds needed for the generator state using a
        // Linear Congruential Generator. The only condition, stated at the end
        // of the 1999 L'Ecuyer paper, is that the seeds must be greater than 1,
        // 7 and 15.

// Linear Congruential Generator
#define LCG(n) ((69069*n)&0xffffffffUL)

        m_seed0=LCG(seed);
        if (m_seed0<2) {
            m_seed0+=2;
        }

        m_seed1=LCG(m_seed0);
        if (m_seed1<8) {
            m_seed1+=8;
        }

        m_seed2=LCG(m_seed1);
        if (m_seed2<16) {
            m_seed2+=16;
        }

#undef LCG
       }

    /// Generates a pseudo random number within the full range of 32 bits.
    unsigned int getRandom() {
// Use a mask of 0xffffffffUL to make in work on 64-bit machines.
#define TAUSWORTHE(s,a,b,c,d) (((s&c)<<d)&0xffffffffUL)^((((s<<a)&0xffffffffUL)^s)>>b)

        m_seed0=TAUSWORTHE(m_seed0, 13, 19, 4294967294UL, 12);
        m_seed1=TAUSWORTHE(m_seed1,  2, 25, 4294967288UL,  4);
        m_seed2=TAUSWORTHE(m_seed2,  3, 11, 4294967280UL, 17);

        return m_seed0^m_seed1^m_seed2;

#undef TAUSWORTHE
    }

    //@}

    /**
     * \name Internal state variables
     */
    //@{

    unsigned int m_seed0; ///< First seed number.
    unsigned int m_seed1; ///< Second seed number.
    unsigned int m_seed2; ///< Third seed number.

    //@}
};

/**
 * \name Type definitions for convenience
 */
//@{

typedef RandomBase<double,RandomEcuyerImpl> RandomEcuyerd;
typedef RandomBase<float,RandomEcuyerImpl> RandomEcuyerf;

//@}
