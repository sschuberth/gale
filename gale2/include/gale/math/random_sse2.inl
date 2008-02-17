#include <emmintrin.h>

/**
 * Random number generator class implemented using SSE2 intrinsics based on the
 * Linear Congruential Generator idea, see <http://blog.gamedeff.com/?p=80>.
 */
struct RandomSSE2Impl
{
    /**
     * \name Methods required by the base class template
     */
    //@{

    /// Sets the generator seed to the given value.
    void setSeed(unsigned int const seed) {
        m_seed=_mm_set_epi32(seed+7,seed+9,seed+13,seed+17);
    }

    /// Generates a pseudo random number within the full range of 32 bits.
    unsigned int getRandom() {
        static __m128i const MM_MUL=_mm_set_epi32(16807,16807,16807,16807);

        static int i=3;
        i=(i+1)&3;

        if (i==0) {
            __m128i seed=m_seed;
            __m128i low=_mm_mul_epu32(seed,MM_MUL);
            __m128i high=_mm_mul_epu32(_mm_shuffle_epi32(seed,_MM_SHUFFLE(1,3,3,1)),MM_MUL);

            __m128 e_low=_mm_load_ps(reinterpret_cast<float const*>(&low));
            __m128 e_high=_mm_load_ps(reinterpret_cast<float const*>(&high));
            __m128 e_seed=_mm_shuffle_ps(e_low,e_high,_MM_SHUFFLE(2,0,2,0));

            m_seed=_mm_load_si128(reinterpret_cast<__m128i const*>(&e_seed));

            m_res=_mm_srai_epi32(m_seed,8);
        }

        return m_res.m128i_u32[i];
    }

    //@}

    /**
     * \name Internal state variables
     */
    //@{

    __m128i m_seed; ///< The seed number.
    __m128i m_res;  ///< The last four random numbers.

    //@}
};

/**
 * \name Type definitions for convenience
 */
//@{

typedef RandomBase<double,RandomSSE2Impl> RandomSSE2d;
typedef RandomBase<float,RandomSSE2Impl> RandomSSE2f;

//@}
