/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at http://developer.berlios.de/projects/gale/
 *
 * Copyright (C) 2005-2006  Sebastian Schuberth <sschuberth@gmail.com>
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

#ifndef GALE_TINY
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
class TupleBase {

  public:

    /// Data type definition for external access to the template argument.
    typedef T Type;

    /**
     * \name Constructors
     */
    //@{

    /// For performance reasons, do not initialize any data by default.
    TupleBase() {
    }

    /// Allows to initialize 2-tuples directly.
    TupleBase(T const& e0,T const& e1) {
        G_ASSERT(N==2)
        m_data[0]=e0;
        m_data[1]=e1;
    }

    /// Allows to initialize 3-tuples directly.
    TupleBase(T const& e0,T const& e1,T const& e2) {
        G_ASSERT(N==3)
        m_data[0]=e0;
        m_data[1]=e1;
        m_data[2]=e2;
    }

    /// Allows to initialize 4-tuples directly.
    TupleBase(T const& e0,T const& e1,T const& e2,T const& e3) {
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
    T* getData() {
        return m_data;
    }

    /// Returns a \c constant pointer to the internal data array.
    T const* getData() const {
        return m_data;
    }

    /// Casts \c this tuple to a pointer of type \a T. As an intended side
    /// effect, this also provides indexed data access.
    operator T*() {
        return getData();
    }

    /// Casts \c this tuple to a \c constant pointer of type \a T. As an
    /// intended side effect, this also provides indexed data access.
    operator T const*() const {
        return getData();
    }

    //@}

    /**
     * \name Element-wise arithmetic operators
     */
    //@{

    /// Increments \c this tuple by another tuple \a t.
    C const& operator+=(C const& t) {
        meta::LoopFwd<N,meta::OpArithInc>::iterate(getData(),t.getData());
        return *static_cast<C*>(this);
    }

    /// Decrements \c this tuple by another tuple \a t.
    C const& operator-=(C const& t) {
        meta::LoopFwd<N,meta::OpArithDec>::iterate(getData(),t.getData());
        return *static_cast<C*>(this);
    }

    /// Multiplies \c this tuple by another tuple \a t.
    C const& operator*=(C const& t) {
        meta::LoopFwd<N,meta::OpArithMul>::iterate(getData(),t.getData());
        return *static_cast<C*>(this);
    }

    /// Divides \c this tuple by another tuple \a t.
    C const& operator/=(C const& t) {
        // The value of t is checked downstream in OpArithReci.
        return (*this)*=1/t;
    }

    /// Multiplies \c this tuple by a scalar \a s.
    C const& operator*=(T s) {
        meta::LoopFwd<N,meta::OpArithMul>::iterate(getData(),s);
        return *static_cast<C*>(this);
    }

    /// Divides \c this tuple by a scalar \a s.
    C const& operator/=(T s) {
        G_ASSERT(math::abs(s)>std::numeric_limits<T>::epsilon())
        return (*this)*=1/s;
    }

    //@}

    /**
     * \name Extremes determination methods
     */
    //@{

    /// Determines the minimum element of \c this tuple.
    T getMinElement() const {
        return meta::LoopFwd<N,meta::OpCalcMin>::iterate(getData());
    }

    /// Determines the maximum element of \c this tuple.
    T getMaxElement() const {
        return meta::LoopFwd<N,meta::OpCalcMax>::iterate(getData());
    }

    /// Determines the absolute minimum element of \c this tuple.
    T getAbsMinElement() const {
        return meta::LoopFwd<N,meta::OpCalcMin>::iterateAbsValues(getData());
    }

    /// Determines the absolute maximum element of \c this tuple.
    T getAbsMaxElement() const {
        return meta::LoopFwd<N,meta::OpCalcMax>::iterateAbsValues(getData());
    }

    /// Calculates the element-wise minimum of \c this tuple and another tuple
    /// \a t.
    C getMinElements(C const& t) const {
        C tmp;
        meta::LoopFwd<N,meta::OpCalcMin>::
          iterate(tmp.getData(),getData(),t.getData());
        return tmp;
    }

    /// Calculates the element-wise maximum of \c this tuple and another tuple
    /// \a t.
    C getMaxElements(C const& t) const {
        C tmp;
        meta::LoopFwd<N,meta::OpCalcMax>::
          iterate(tmp.getData(),getData(),t.getData());
        return tmp;
    }

    //@}

    /**
     * \name Miscellaneous methods
     */
    //@{

    /// Linearly interpolates between \c this tuple and another tuple \a t based
    /// on a scalar \a s. For performance reasons, \a s is not clamped to [0,1].
    C getLerp(C const& t,double s) const {
        C tmp;
        meta::LoopFwd<N,meta::OpCalcLerp>::
          iterate(tmp.getData(),getData(),t.getData(),s);
        return tmp;
    }

    //@}

    /**
     * \name Element-wise arithmetic operators
     */
    //@{

    /// Returns tuple \a t unchanged; provided for convenience.
    friend C const& operator+(C const& t) {
        return t;
    }

    /// Returns the negation of tuple \a t.
    friend C operator-(C const& t) {
        C tmp;
        meta::LoopFwd<N,meta::OpAssignNeg>::iterate(tmp.getData(),t.getData());
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
        // The value of t is checked downstream in OpArithReci.
        return C(t)/=u;
    }

    /// Performs scalar multiplication from the right of each element.
    friend C operator*(C const& t,T s) {
        return C(t)*=s;
    }

    /// Performs scalar multiplication from the left of each element.
    friend C operator*(T s,C const& t) {
        return t*s;
    }

    /// Performs scalar division from the right of each element.
    friend C operator/(C const& t,T s) {
        // The value of t is checked downstream in operator/=(T s).
        return C(t)/=s;
    }

    /// Performs scalar division from the left of each element.
    friend C operator/(T s,C const& t) {
        C tmp;
        meta::LoopFwd<N,meta::OpArithReci>::iterate(tmp.getData(),t.getData());
        return s*tmp;
    }

    //@}

    /**
     * \name Element-wise comparison operators
     */
    //@{

    /// Returns whether all elements in \a t are less than their counterpart in
    /// \a u.
    friend bool operator<(C const& t,C const& u) {
        return meta::LoopFwd<N,meta::OpCmpLess>::
                 iterateCondAnd(t.getData(),u.getData());
    }

    /// Returns whether all elements in \a t are less than or equal to their
    /// counterpart in \a u.
    friend bool operator<=(C const& t,C const& u) {
        return meta::LoopFwd<N,meta::OpCmpLessEqual>::
                 iterateCondAnd(t.getData(),u.getData());
    }

    /// Returns whether all elements in \a t are greater than their counterpart
    /// in \a u.
    friend bool operator>(C const& t,C const& u) {
        return meta::LoopFwd<N,meta::OpCmpGreater>::
                 iterateCondAnd(t.getData(),u.getData());
    }

    /// Returns whether all elements in \a t are greater than or equal to their
    /// counterpart in \a u.
    friend bool operator>=(C const& t,C const& u) {
        return meta::LoopFwd<N,meta::OpCmpGreaterEqual>::
                 iterateCondAnd(t.getData(),u.getData());
    }

    /// Returns whether all elements in \a t equal their counterpart in \a u
    /// using an epsilon-environment depending on the precision of \a T.
    friend bool operator==(C const& t,C const& u) {
        return meta::LoopFwd<N,meta::OpCmpEqualEps>::
                 iterateCondAnd(t.getData(),u.getData());
    }

    /// Returns whether the elements in \a t are not equal to their counterparts
    /// in \a u using an epsilon-environment depending on the precision of \a T.
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
        for (int i=0;i<N;++i)
            s >> t[i];
        return s;
    }

    /// Writes tuple values to an output stream.
    friend std::ostream& operator<<(std::ostream& s,C const& t) {
        s << '(';
        for (int i=0;i<N-1;++i)
            s << t[i] << ',';
        return s << t[N-1] << ')';
    }

    //@}
#endif

  protected:

    /// Tightly packed array of \a N elements of type \a T.
    T m_data[N];
};

#pragma pack(pop)

/**
 * Tuple class implementation as an example how to derive from TupleBase. This
 * adds no functionality but shows how to get rid of the C template argument.
 */
template<unsigned int N,typename T>
class Tuple:public TupleBase<N,T,Tuple<N,T> > {
    /// This type definition simplifies base class access to identifiers that
    /// are not visible until instantiation time because they do not dependent
    /// on template arguments.
    typedef TupleBase<N,T,Tuple<N,T> > Base;

  public:

    /**
     * \name Constructors
     */
    //@{

    /// For performance reasons, do not initialize any data by default.
    Tuple() {
    }

    /// Allows to initialize 2-tuples directly.
    Tuple(T const& e0,T const& e1):Base(e0,e1) {
    }

    /// Allows to initialize 3-tuples directly.
    Tuple(T const& e0,T const& e1,T const& e2):Base(e0,e1,e2) {
    }

    /// Allows to initialize 4-tuples directly.
    Tuple(T const& e0,T const& e1,T const& e2,T const& e3):Base(e0,e1,e2,e3) {
    }

    //@}
};

} // namespace math

} // namespace gale

#endif // TUPLE_H
