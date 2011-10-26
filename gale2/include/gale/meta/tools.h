/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <https://sourceforge.net/projects/gale-opengl/>.
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

#pragma once

/**
 * \file
 * Miscellaneous meta-template tools
 */

namespace gale {

namespace meta {

/**
 * Helper class allowing to initialize arrays with a comma separated list. See
 * the DynamicArray assignment operator implementation for an example usage.
 */
template<typename T>
struct ArrayInitializer
{
    /// Definition for external access to the data type.
    typedef T Type;

    /// Constructor to initialize the iterator.
    ArrayInitializer(T* iterator)
    :   iterator(iterator)
    {}

    /// Operator to assign a list of comma separated values to the array pointed
    /// to by the iterator.
    ArrayInitializer& operator,(T const& value) {
        *iterator++=value;
        return *this;
    }

    T* iterator; ///< Stores the current position within the array.
};

} // namespace meta

} // namespace gale
