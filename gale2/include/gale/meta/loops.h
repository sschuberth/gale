/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
 *
 * Copyright (C) 2005-2007  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
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

#ifndef LOOPS_H
#define LOOPS_H

/**
 * \file
 * Meta-template loop implementations
 */

#include "../global/defines.h"
#include "../math/essentials.h"

namespace gale {

namespace meta {

/**
 * Meta-template forward loop implementation; iterates ascendingly over \a NUM
 * elements in at least one array, evaluating \a OP on the elements.
 */
template<unsigned int NUM,class OP>
struct LoopFwd
{
    enum {
        index=NUM-1 ///< Stores the array index to use in this recursion.
    };

    /// Iterates over a destination array applying a scalar.
    template<typename A,typename B>
    static G_INLINE void iterate(A* a,B const& b) {
        LoopFwd<index,OP>::iterate(a,b);
        OP::evaluate(a[index],b);
    }

    /// Iterates over a destination array and a source array.
    template<typename A,typename B>
    static G_INLINE void iterate(A* a,B const* b) {
        LoopFwd<index,OP>::iterate(a,b);
        OP::evaluate(a[index],b[index]);
    }

    /// Iterates over a destination array and two source arrays.
    template<typename A,typename B,typename C>
    static G_INLINE void iterate(A* a,B const* b,C const* c) {
        LoopFwd<index,OP>::iterate(a,b,c);
        a[index]=OP::evaluate(b[index],c[index]);
    }

    /// Iterates over a destination array and two source arrays applying a
    /// scalar.
    template<typename A,typename B,typename C,typename D>
    static G_INLINE void iterate(A* a,B const* b,C const* c,D const& d) {
        LoopFwd<index,OP>::iterate(a,b,c,d);
        a[index]=OP::evaluate(b[index],c[index],d);
    }

    /// Iterates over a single array evaluating a scalar.
    template<typename A>
    static G_INLINE A iterate(A const* a) {
        return OP::evaluate(a[index],LoopFwd<index,OP>::iterate(a));
    }

    /// Iterates over a single array of absolute values evaluating a scalar.
    template<typename A>
    static G_INLINE A iterateAbsValues(A const* a) {
        return OP::evaluate(
            math::abs(a[index]),
            LoopFwd<index,OP>::iterateAbsValues(a)
        );
    }

    /// Iterates over two arrays adding evaluated values.
    template<typename A,typename B>
    static G_INLINE A iterateCombAdd(A const* a,B const* b) {
        return LoopFwd<index,OP>::iterateCombAdd(a,b)
             + OP::evaluate(a[index],b[index]);
    }

    /// Iterates over two arrays evaluating an "and" condition.
    template<typename A,typename B>
    static G_INLINE bool iterateCondAnd(A const* a,B const* b) {
        return LoopFwd<index,OP>::iterateCondAnd(a,b)
            && OP::evaluate(a[index],b[index]);
    }

    /// Iterates over a destination array \a a setting its elements according to
    /// the bit mask \a b to either the value of \a set or \a clear.
    template<typename A,typename B>
    static G_INLINE void iterateIndexMask(A* a,B const& b,A set=1,A clear=0) {
        LoopFwd<index,OP>::iterateIndexMask(a,b,set,clear);
        OP::evaluate(a[index],b&(1<<index)?set:clear);
    }
};

/**
 * Partial template specialization for special handling of the first iteration
 * and to stop the meta-recursion.
 */
template<class OP>
struct LoopFwd<1,OP>
{
    /// Iterates over a destination array applying a scalar.
    template<typename A,typename B>
    static G_INLINE void iterate(A* a,B const& b) {
        OP::evaluate(a[0],b);
    }

    /// Iterates over a destination array and a source array.
    template<typename A,typename B>
    static G_INLINE void iterate(A* a,B const* b) {
        OP::evaluate(a[0],b[0]);
    }

    /// Iterates over a destination array and two source arrays.
    template<typename A,typename B,typename C>
    static G_INLINE void iterate(A* a,B const* b,C const* c) {
        a[0]=OP::evaluate(b[0],c[0]);
    }

    /// Iterates over a destination array and two source arrays applying a
    /// scalar.
    template<typename A,typename B,typename C,typename D>
    static G_INLINE void iterate(A* a,B const* b,C const* c,D const& d) {
        a[0]=OP::evaluate(b[0],c[0],d);
    }

    /// Iterates over a single array evaluating a scalar.
    template<typename A>
    static G_INLINE A const& iterate(A const* a) {
        return a[0];
    }

    /// Iterates over a single array of absolute values evaluating a scalar.
    template<typename A>
    static G_INLINE A iterateAbsValues(A const* a) {
        return math::abs(a[0]);
    }

    /// Iterates over two arrays adding evaluated values.
    template<typename A,typename B>
    static G_INLINE A iterateCombAdd(A const* a,B const* b) {
        return OP::evaluate(a[0],b[0]);
    }

    /// Iterates over two arrays evaluating an "and" condition.
    template<typename A,typename B>
    static G_INLINE bool iterateCondAnd(A const* a,B const* b) {
        return OP::evaluate(a[0],b[0]);
    }

    /// Iterates over a destination array \a a setting its elements according to
    /// the bit mask \a b to either the value of \a set or \a clear.
    template<typename A,typename B>
    static G_INLINE void iterateIndexMask(A* a,B const& b,A set=1,A clear=0) {
        OP::evaluate(a[0],b&1?set:clear);
    }
};

} // namespace meta

} // namespace gale

#endif // LOOPS_H
