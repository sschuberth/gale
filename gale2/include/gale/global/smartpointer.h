/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
 *
 * Copyright (C) 2005-2009  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
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

#ifndef SMARTPOINTER_H
#define SMARTPOINTER_H

/**
 * \file
 * A collection of smart pointer idiom implementations
 */

namespace gale {

namespace global {

/**
 * A non-intrusive reference counting smart pointer.
 */
template <class T>
class SharedPtr
{
  public:

    /// Creates a new shared pointer to the given \a object.
    explicit SharedPtr(T* object=NULL)
    :   m_object(object)
    ,   m_count(new int(1))
    {}

    /// Initialized this pointer with an \a other one, increasing the common
    /// reference count.
    SharedPtr(SharedPtr const& other)
    :   m_object(other.m_object)
    ,   m_count(other.m_count)
    {
        ++*m_count;
    }

    /// Decreases the reference count and deletes the pointer if necessary.
    ~SharedPtr() {
        unref();
    }

    /// Assigns \a other to this pointer, decreasing this pointer's old
    /// reference count and increasing the new common one.
    SharedPtr& operator=(SharedPtr const& other) {
        if (&other!=this) {
            unref();
            m_object=other.m_object;
            m_count=other.m_count;
            ++*m_count;
        }
        return *this;
    }

    /// Dereferences the underlying pointer.
    T& operator*() const {
        return *m_object;
    }

    /// Returns the underlying pointer, allowing member access.
    T* operator->() const {
        return m_object;
    }

  private:

    /// Decreases the reference count and deletes the pointer if necessary.
    void unref() {
        if (--*m_count<=0) {
            delete m_object;
            delete m_count;
        }
    }

    T* m_object;  ///< The shared object pointer.
    int* m_count; ///< The shared reference count.
};

} // namespace global

} // namespace gale

#endif // SMARTPOINTER_H
