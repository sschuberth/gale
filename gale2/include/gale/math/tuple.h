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

#ifndef TUPLE_H
#define TUPLE_H

/**
 * \file
 * Mathematical tuple classes
 */

#include "../meta/loops.h"
#include "../meta/operators.h"

#ifndef GALE_TINY_CODE
    #include <iostream>
#endif

namespace gale {

namespace math {

// Make sure data members are tightly packed.
#pragma pack(push,1)

/**
 * Tuple base class implementation, with \a N elements of type \a T. Derived
 * classes are specified as \a C to provide the proper return and argument type.
 */
template<unsigned int N,typename T,class C>
class TupleBase
{
  public:

    /**
     * \name Constructors
     */
    //@{

    /// Create a tuple whose elements are either not initialized at all or
    /// initialized to 0 if \c GALE_INIT_DATA is defined.
    TupleBase() {
        // Do not allow tuples with less than 2 elements.
        G_ASSERT(N>=2)

#ifdef GALE_INIT_DATA
        meta::LoopFwd<N,meta::OpAssign>::iterate(data(),0);
#endif
    }

    /// Allows to initialize 2-tuples directly.
    TupleBase(T const e0,T const e1) {
        G_ASSERT(N==2)
        m_data[0]=e0;
        m_data[1]=e1;
    }

    /// Allows to initialize 3-tuples directly.
    TupleBase(T const e0,T const e1,T const e2) {
        G_ASSERT(N==3)
        m_data[0]=e0;
        m_data[1]=e1;
        m_data[2]=e2;
    }

    /// Allows to initialize 4-tuples directly.
    TupleBase(T const e0,T const e1,T const e2,T const e3) {
        G_ASSERT(N==4)
        m_data[0]=e0;
        m_data[1]=e1;
        m_data[2]=e2;
        m_data[3]=e3;
    }

    //@}

    /**
     * \name Element access methods
     */
    //@{

    /// Returns a pointer to the internal data array.
    T* data() {
        return m_data;
    }

    /// Returns a constant pointer to the internal data array.
    T const* data() const {
        return m_data;
    }

    /// Casts \c this tuple to a pointer of type \a T. As an intended side
    /// effect, this also provides indexed data access.
    operator T*() {
        return data();
    }

    /// Casts \c this tuple to a pointer of type \a T \c const. As an intended
    /// side effect, this also provides indexed data access.
    operator T const*() const {
        return data();
    }

    /// Assigns new values to the first two tuple elements.
    void set(T const e0,T const e1) {
        G_ASSERT(N>=2)
        m_data[0]=e0;
        m_data[1]=e1;
    }

    /// Assigns new values to the first three tuple elements.
    void set(T const e0,T const e1,T const e2) {
        G_ASSERT(N>=3)
        m_data[0]=e0;
        m_data[1]=e1;
        m_data[2]=e2;
    }

    /// Assigns new values to the first four tuple elements.
    void set(T const e0,T const e1,T const e2,T const e3) {
        G_ASSERT(N>=4)
        m_data[0]=e0;
        m_data[1]=e1;
        m_data[2]=e2;
        m_data[3]=e3;
    }

    //@}

    /**
     * \name Arithmetic tuple / tuple operators
     */
    //@{

    /// Element-wise increments \c this tuple by tuple \a t.
    C const& operator+=(C const& t) {
        meta::LoopFwd<N,meta::OpArithAdd>::iterate(data(),t.data());
        return *static_cast<C*>(this);
    }

    /// Element-wise decrements \c this tuple by tuple \a t.
    C const& operator-=(C const& t) {
        meta::LoopFwd<N,meta::OpArithSub>::iterate(data(),t.data());
        return *static_cast<C*>(this);
    }

    /// Element-wise multiplies \c this tuple by tuple \a t.
    C const& operator*=(C const& t) {
        meta::LoopFwd<N,meta::OpArithMul>::iterate(data(),t.data());
        return *static_cast<C*>(this);
    }

    /// Element-wise divides \c this tuple by tuple \a t.
    C const& operator/=(C const& t) {
        // The value of t is checked downstream in OpArithDiv.
        meta::LoopFwd<N,meta::OpArithDiv>::iterate(data(),t.data());
        return *static_cast<C*>(this);
    }

    /// Returns tuple \a t unchanged; provided for convenience.
    friend C const& operator+(C const& t) {
        return t;
    }

    /// Returns the element-wise negation of tuple \a t.
    friend C operator-(C const& t) {
        C tmp;
        meta::LoopFwd<N,meta::OpAssignNeg>::iterate(tmp.data(),t.data());
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
    C const& operator*=(T const s) {
        meta::LoopFwd<N,meta::OpArithMul>::iterate(data(),s);
        return *static_cast<C*>(this);
    }

    /// Divides each element of \c this tuple by a scalar \a s.
    C const& operator/=(T const s) {
        // The value of t is checked downstream in OpArithDiv.
        meta::LoopFwd<N,meta::OpArithDiv>::iterate(data(),s);
        return *static_cast<C*>(this);
    }

    /// Multiplies each element of tuple \a t by a scalar \a s from the right.
    friend C operator*(C const& t,T const s) {
        return C(t)*=s;
    }

    /// Multiplies each element of tuple \a t by a scalar \a s from the left.
    friend C operator*(T const s,C const& t) {
        return t*s;
    }

    /// Divides each element of tuple \a t by a scalar \a s.
    friend C operator/(C const& t,T const s) {
        // The value of s is checked downstream in operator/=(T s).
        return C(t)/=s;
    }

    /// Divides a scalar \a s by each element of tuple \a t.
    friend C operator/(T const s,C const& t) {
        C tmp;
        meta::LoopFwd<N,meta::OpArithReci>::iterate(tmp.data(),t.data());
        return s*tmp;
    }

    //@}

    /**
     * \name Extremes determination methods
     */
    //@{

    /// Determines the minimum element of \c this tuple.
    T minElement() const {
        return meta::LoopFwd<N,meta::OpCalcMin>::iterate(data());
    }

    /// Determines the maximum element of \c this tuple.
    T maxElement() const {
        return meta::LoopFwd<N,meta::OpCalcMax>::iterate(data());
    }

    /// Determines the absolute minimum element of \c this tuple.
    T absMinElement() const {
        return meta::LoopFwd<N,meta::OpCalcMin>::iterateAbsValues(data());
    }

    /// Determines the absolute maximum element of \c this tuple.
    T absMaxElement() const {
        return meta::LoopFwd<N,meta::OpCalcMax>::iterateAbsValues(data());
    }

    /// Calculates the element-wise minimum of \c this tuple and tuple \a t.
    C minElements(C const& t) const {
        C tmp;
        meta::LoopFwd<N,meta::OpCalcMin>
            ::iterate(tmp.data(),data(),t.data());
        return tmp;
    }

    /// Calculates the element-wise maximum of \c this tuple and tuple \a t.
    C maxElements(C const& t) const {
        C tmp;
        meta::LoopFwd<N,meta::OpCalcMax>
            ::iterate(tmp.data(),data(),t.data());
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
        return meta::LoopFwd<N,meta::OpCmpLess>
                   ::iterateCondAnd(t.data(),u.data());
    }

    /// Returns whether all elements in \a t are less than or equal to their
    /// counterpart in \a u.
    friend bool operator<=(C const& t,C const& u) {
        return meta::LoopFwd<N,meta::OpCmpLessEqual>
                   ::iterateCondAnd(t.data(),u.data());
    }

    /// Returns whether all elements in \a t are greater than their counterpart
    /// in \a u.
    friend bool operator>(C const& t,C const& u) {
        return meta::LoopFwd<N,meta::OpCmpGreater>
                   ::iterateCondAnd(t.data(),u.data());
    }

    /// Returns whether all elements in \a t are greater than or equal to their
    /// counterpart in \a u.
    friend bool operator>=(C const& t,C const& u) {
        return meta::LoopFwd<N,meta::OpCmpGreaterEqual>
                   ::iterateCondAnd(t.data(),u.data());
    }

    /// Returns whether all elements in \a t equal their counterpart in \a u
    /// with regard to a tolerance depending on the precision of data type \a T.
    friend bool operator==(C const& t,C const& u) {
        return meta::LoopFwd<N,meta::OpCmpEqual>
                   ::iterateCondAnd(t.data(),u.data());
    }

    /// Returns whether the elements in \a t are not equal to their counterparts
    /// in \a u with regard to a tolerance depending on the precision of data
    /// type \a T.
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
        meta::LoopFwd<N,meta::OpCalcLerp>
            ::iterate(tmp.data(),t.data(),u.data(),s);
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
        for (int i=0;i<N;++i) {
            s >> t[i];
        }
        return s;
    }

    /// Writes tuple values to an output stream.
    friend std::ostream& operator<<(std::ostream& s,C const& t) {
        s << '(';
        for (int i=0;i<N-1;++i) {
            s << t[i] << ',';
        }
        return s << t[N-1] << ')';
    }

    //@}

#endif // GALE_TINY_CODE

  protected:

    T m_data[N]; ///< Tightly packed array of \a N elements of type \a T.
};

#include "tuple_sse.inl"

#pragma pack(pop)

/**
 * Tuple class implementation as an example how to derive from TupleBase. This
 * adds no functionality but shows how to get rid of the C template argument.
 */
template<unsigned int N,typename T>
class Tuple:public TupleBase<N,T,Tuple<N,T> >
{
    /// This type definition simplifies base class access to identifiers that
    /// are not visible until instantiation time because they do not dependent
    /// on template arguments.
    typedef TupleBase<N,T,Tuple<N,T> > Base;

  public:

    /// Definition for external access to the data type.
    typedef T Type;

    /**
     * \name Constructors
     */
    //@{

    /// Create a tuple whose elements are either not initialized at all or
    /// initialized to 0 if \c GALE_INIT_DATA is defined.
    Tuple() {}

    /// Allows to initialize 2-tuples directly.
    Tuple(T const e0,T const e1)
    :   Base(e0,e1) {}

    /// Allows to initialize 3-tuples directly.
    Tuple(T const e0,T const e1,T const e2)
    :   Base(e0,e1,e2) {}

    /// Allows to initialize 4-tuples directly.
    Tuple(T const e0,T const e1,T const e2,T const e3)
    :   Base(e0,e1,e2,e3) {}

    //@}
};

} // namespace math

} // namespace gale

#endif // TUPLE_H
