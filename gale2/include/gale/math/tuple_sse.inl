/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
 *
 * Copyright (C) 2005-2010  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
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

#ifdef GALE_USE_SSE

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

    /// Create a tuple whose elements are either not initialized at all or
    /// initialized to 0 if \c GALE_INIT_DATA is defined.
    TupleBase() {
#ifdef GALE_INIT_DATA
        m_simd=_mm_setzero_ps();
#endif
    }

    /// Allows to initialize 4-tuples directly.
    TupleBase(float const e0,float const e1,float const e2,float const e3) {
        m_simd=_mm_setr_ps(e0,e1,e2,e3);
    }

    //@}

    /**
     * \name Element access methods
     */
    //@{

    /// Returns a pointer to the internal data array.
    float* data() {
        return m_simd.m128_f32;
    }

    /// Returns a constant pointer to the internal data array.
    float const* data() const {
        return m_simd.m128_f32;
    }

    /// Casts \c this tuple to a pointer of type \c float. As an intended side
    /// effect, this also provides indexed data access.
    operator float*() {
        return data();
    }

    /// Casts \c this tuple to a constant pointer of type \c float. As an
    /// intended side effect, this also provides indexed data access.
    operator float const*() const {
        return data();
    }

    /// Assigns new values to the first four tuple elements.
    void set(float const e0,float const e1,float const e2,float const e3) {
        m_simd=_mm_setr_ps(e0,e1,e2,e3);
    }

    //@}

    /**
     * \name Arithmetic tuple / tuple operators
     */
    //@{

    /// Element-wise increments \c this tuple by tuple \a t.
    C const& operator+=(C const& t) {
        m_simd=_mm_add_ps(m_simd,t.m_simd);
        return *static_cast<C*>(this);
    }

    /// Element-wise decrements \c this tuple by tuple \a t.
    C const& operator-=(C const& t) {
        m_simd=_mm_sub_ps(m_simd,t.m_simd);
        return *static_cast<C*>(this);
    }

    /// Element-wise multiplies \c this tuple by tuple \a t.
    C const& operator*=(C const& t) {
        m_simd=_mm_mul_ps(m_simd,t.m_simd);
        return *static_cast<C*>(this);
    }

    /// Element-wise divides \c this tuple by tuple \a t.
    C const& operator/=(C const& t) {
        // The value of t is checked downstream in operator/=(float s).
        return (*this)*=1/t;
    }

    /// Returns tuple \a t unchanged; provided for convenience.
    friend C const& operator+(C const& t) {
        return t;
    }

    /// Returns the element-wise negation of tuple \a t.
    friend C operator-(C const& t) {
        C tmp;
        tmp.m_simd=_mm_sub_ps(_mm_set_ps1(0),t.m_simd);
        return tmp;
    }

    /// Returns the element-wise sum of tuples \a t and \a u.
    friend C operator+(C const& t,C const& u) {
        return C(t)+=u;
    }

    /// Returns the element-wise difference of tuples \a t and \a u.
    friend C operator-(C const& t,C const& u) {
        return C(t)-=u;
    }

    /// Returns the element-wise product of tuples \a t and \a u.
    friend C operator*(C const& t,C const& u) {
        return C(t)*=u;
    }

    /// Returns the element-wise quotient of tuples \a t and \a u.
    friend C operator/(C const& t,C const& u) {
        // The value of u is checked downstream in OpArithReci.
        return C(t)/=u;
    }

    //@}

    /**
     * \name Arithmetic tuple / scalar operators
     */
    //@{

    /// Multiplies each element of \c this tuple by a scalar \a s.
    C const& operator*=(float const s) {
        m_simd=_mm_mul_ps(m_simd,_mm_set_ps1(s));
        return *static_cast<C*>(this);
    }

    /// Divides each element of \c this tuple by a scalar \a s.
    C const& operator/=(float const s) {
        G_ASSERT(abs(s)>Numerics<float>::ZERO_TOLERANCE())
        return (*this)*=1/s;
    }

    /// Multiplies each element of tuple \a t by a scalar \a s from the right.
    friend C operator*(C const& t,float const s) {
        return C(t)*=s;
    }

    /// Multiplies each element of tuple \a t by a scalar \a s from the left.
    friend C operator*(float const s,C const& t) {
        return t*s;
    }

    /// Divides each element of tuple \a t by a scalar \a s.
    friend C operator/(C const& t,float const s) {
        // The value of s is checked downstream in operator/=(float s).
        return C(t)/=s;
    }

    /// Divides a scalar \a s by each element of tuple \a t.
    friend C operator/(float const s,C const& t) {
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
    float minElement() const {
        return meta::LoopFwd<4,meta::OpCalcMin>::iterate(data());
    }

    /// Determines the maximum element of \c this tuple.
    float maxElement() const {
        return meta::LoopFwd<4,meta::OpCalcMax>::iterate(data());
    }

    /// Determines the absolute minimum element of \c this tuple.
    float absMinElement() const {
        return meta::LoopFwd<4,meta::OpCalcMin>::iterateAbsValues(data());
    }

    /// Determines the absolute maximum element of \c this tuple.
    float absMaxElement() const {
        return meta::LoopFwd<4,meta::OpCalcMax>::iterateAbsValues(data());
    }

    /// Calculates the element-wise minimum of \c this tuple and tuple \a t.
    C minElements(C const& t) const {
        C tmp;
        tmp.m_simd=_mm_min_ps(m_simd,t.m_simd);
        return tmp;
    }

    /// Calculates the element-wise maximum of \c this tuple and tuple \a t.
    C maxElements(C const& t) const {
        C tmp;
        tmp.m_simd=_mm_max_ps(m_simd,t.m_simd);
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

    /**
     * \name Miscellaneous methods
     */
    //@{

    /// Linearly interpolates between the tuples \a t and \a u based on a scalar
    /// \a s. For performance reasons, \a s is not clamped to [0,1].
    friend C lerp(C const& t,C const& u,float const s) {
        C tmp;
        tmp.m_simd=_mm_add_ps(
            t.m_simd
        ,   _mm_mul_ps(
                _mm_sub_ps(u.m_simd,t.m_simd)
            ,   _mm_set_ps1(s)
            )
        );
        return tmp;
    }

    //@}

#ifndef GALE_TINY_CODE

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

#endif // GALE_TINY_CODE

  protected:

    __m128 m_simd;   ///< SIMD data type for a 128-bit register.
};

#endif // GALE_USE_SSE
