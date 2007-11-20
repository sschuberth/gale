#include <xmmintrin.h>

/**
 * Because there is more than one template parameter, this has to be a complete
 * template specialization. It uses SSE instructions for x86 architectures.
 */
template<class C>
class TupleBase<4,float,C>
{
  public:

    /**
     * \name Constructors
     */
    //@{

    /// For performance reasons, do not initialize any data by default.
    TupleBase() {}

    /// Allows to initialize 4-tuples directly.
    TupleBase(float const& e0,float const& e1,float const& e2,float const& e3) {
        m_simd=_mm_setr_ps(e0,e1,e2,e3);
    }

    //@}

    /**
     * \name Element access methods
     */
    //@{

    /// Returns a pointer to the internal data array.
    float* getData() {
        return m_simd.m128_f32;
    }

    /// Returns a \c constant pointer to the internal data array.
    float const* getData() const {
        return m_simd.m128_f32;
    }

    /// Casts \c this tuple to a pointer of type \a float. As an intended side
    /// effect, this also provides indexed data access.
    operator float*() {
        return getData();
    }

    /// Casts \c this tuple to a \c constant pointer of type \a float. As an
    /// intended side effect, this also provides indexed data access.
    operator float const*() const {
        return getData();
    }

    //@}

    /**
     * \name Element-wise arithmetic operators with tuples
     */
    //@{

    /// Increments \c this tuple by another tuple \a t.
    C const& operator+=(C const& t) {
        m_simd=_mm_add_ps(m_simd,t.m_simd);
        return *static_cast<C*>(this);
    }

    /// Decrements \c this tuple by another tuple \a t.
    C const& operator-=(C const& t) {
        m_simd=_mm_sub_ps(m_simd,t.m_simd);
        return *static_cast<C*>(this);
    }

    /// Multiplies \c this tuple by another tuple \a t.
    C const& operator*=(C const& t) {
        m_simd=_mm_mul_ps(m_simd,t.m_simd);
        return *static_cast<C*>(this);
    }

    /// Divides \c this tuple by another tuple \a t.
    C const& operator/=(C const& t) {
        // The value of t is checked downstream in OpArithReci.
        return (*this)*=1/t;
    }

    /// Returns tuple \a t unchanged; provided for convenience.
    friend C const& operator+(C const& t) {
        return t;
    }

    /// Returns the negation of tuple \a t.
    friend C operator-(C const& t) {
        C tmp;
        tmp.m_simd=_mm_sub_ps(_mm_set_ps1(0),t.m_simd);
        return tmp;
    }

    /// Returns the sum of tuples \a t and \a u.
    friend C operator+(C const& t,C const& u) {
        return C(t)+=u;
    }

    /// Returns the difference of tuples \a t and \a u.
    friend C operator-(C const& t,C const& u) {
        return C(t)-=u;
    }

    /// Returns the product of tuples \a t and \a u.
    friend C operator*(C const& t,C const& u) {
        return C(t)*=u;
    }

    /// Returns the quotient of tuples \a t and \a u.
    friend C operator/(C const& t,C const& u) {
        // The value of u is checked downstream in OpArithReci.
        return C(t)/=u;
    }

    //@}

    /**
     * \name Element-wise arithmetic operators with scalars
     */
    //@{

    /// Multiplies \c this tuple by a scalar \a s.
    C const& operator*=(float s) {
        m_simd=_mm_mul_ps(m_simd,_mm_set_ps1(s));
        return *static_cast<C*>(this);
    }

    /// Divides \c this tuple by a scalar \a s.
    C const& operator/=(float s) {
        assert(abs(s)>std::numeric_limits<float>::epsilon());
        return (*this)*=1/s;
    }

    /// Performs scalar multiplication from the right of each element.
    friend C operator*(C const& t,float s) {
        return C(t)*=s;
    }

    /// Performs scalar multiplication from the left of each element.
    friend C operator*(float s,C const& t) {
        return t*s;
    }

    /// Performs scalar division from the right of each element.
    friend C operator/(C const& t,float s) {
        // The value of s is checked downstream in operator/=(float s).
        return C(t)/=s;
    }

    /// Performs scalar division from the left of each element.
    friend C operator/(float s,C const& t) {
        C tmp;
        tmp.m_simd=_mm_mul_ps(_mm_set_ps1(s),_mm_rcp_ps(t.m_simd));
        return tmp;
    }

    //@}

    /**
     * \name Extremes determination methods
     */
    //@{

    /// Determines the minimum element of \c this tuple.
    float getMinElement() const {
        return meta::LoopFwd<4,meta::OpCalcMin>::iterate(getData());
    }

    /// Determines the maximum element of \c this tuple.
    float getMaxElement() const {
        return meta::LoopFwd<4,meta::OpCalcMax>::iterate(getData());
    }

    /// Determines the absolute minimum element of \c this tuple.
    float getAbsMinElement() const {
        return meta::LoopFwd<4,meta::OpCalcMin>::iterateAbsValues(getData());
    }

    /// Determines the absolute maximum element of \c this tuple.
    float getAbsMaxElement() const {
        return meta::LoopFwd<4,meta::OpCalcMax>::iterateAbsValues(getData());
    }

    /// Calculates the element-wise minimum of \c this tuple and another tuple
    /// \a t.
    C getMinElements(C const& t) const {
        C tmp;
        tmp.m_simd=_mm_min_ps(m_simd,t.m_simd);
        return tmp;
    }

    /// Calculates the element-wise maximum of \c this tuple and another tuple
    /// \a t.
    C getMaxElements(C const& t) const {
        C tmp;
        tmp.m_simd=_mm_max_ps(m_simd,t.m_simd);
        return tmp;
    }

    //@}

    /**
     * \name Miscellaneous methods
     */
    //@{

    /// Linearly interpolates between \c this tuple and another tuple \a t based
    /// on a scalar \a s. For performance reasons, \a s is not clamped to [0,1].
    C getLerp(C const& t,float s) const {
        C tmp;
        tmp.m_simd=
            _mm_add_ps(
                m_simd,
                _mm_mul_ps(
                    _mm_sub_ps(t.m_simd,m_simd),
                    _mm_set_ps1(s)
                )
            );
        return tmp;
    }

    //@}

    /**
     * \name Element-wise comparison operators
     */
    //@{

    /// Returns whether all elements in \a t are less than their counterpart in
    /// \a u.
    friend bool operator<(C const& t,C const& u) {
        return _mm_movemask_ps(_mm_cmplt_ps(t.m_simd,u.m_simd))==0xf;
    }

    /// Returns whether all elements in \a t are less than or equal to their
    /// counterpart in \a u.
    friend bool operator<=(C const& t,C const& u) {
        return _mm_movemask_ps(_mm_cmple_ps(t.m_simd,u.m_simd))==0xf;
    }

    /// Returns whether all elements in \a t are greater than their counterpart
    /// in \a u.
    friend bool operator>(C const& t,C const& u) {
        return _mm_movemask_ps(_mm_cmpgt_ps(t.m_simd,u.m_simd))==0xf;
    }

    /// Returns whether all elements in \a t are greater than or equal to their
    /// counterpart in \a u.
    friend bool operator>=(C const& t,C const& u) {
        return _mm_movemask_ps(_mm_cmpge_ps(t.m_simd,u.m_simd))==0xf;
    }

    /// Returns whether all elements in \a t equal their counterpart in \a u.
    friend bool operator==(C const& t,C const& u) {
        return _mm_movemask_ps(_mm_cmpeq_ps(t.m_simd,u.m_simd))==0xf;
    }

    /// Returns whether the elements in \a t are not equal to their counterparts
    /// in \a u.
    friend bool operator!=(C const& t,C const& u) {
        return !(t==u);
    }

    //@}

#ifndef GALE_TINY

    /**
     * \name Streaming input / output methods
     */
    //@{

    /// Reads tuple values from an input stream.
    friend std::istream& operator>>(std::istream& s,C& t) {
        return s
            >> t.m_simd.m128_f32[0]
            >> t.m_simd.m128_f32[1]
            >> t.m_simd.m128_f32[2]
            >> t.m_simd.m128_f32[3]
        ;
    }

    /// Writes tuple values to an output stream.
    friend std::ostream& operator<<(std::ostream& s,C const& t) {
        return s
            << '(' << t.m_simd.m128_f32[0]
            << ',' << t.m_simd.m128_f32[1]
            << ',' << t.m_simd.m128_f32[2]
            << ',' << t.m_simd.m128_f32[3]
            << ')'
        ;
    }

    //@}

#endif

  protected:

    __m128 m_simd;   ///< SIMD data type for a 128-bit register.
};
