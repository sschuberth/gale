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

#ifndef ATTRIBUTELIST_H
#define ATTRIBUTELIST_H

/**
 * \file
 * Convenience data structures.
 */

namespace gale {

namespace system {

/**
 * This class implements a zero terminated list of type / value pairs that is
 * used to pass attributes to several API calls.
 */
template<typename T,int N=256>
class AttributeList {
  public:
    /// Creates an empty list.
    AttributeList() {
        clear();
    }

    /// Deletes all attributes in the list.
    void clear() {
        m_size=0;
        m_attributes[m_size]=0;
    }

    /// Appends / replaces the specified \a type and \a value to / in the list.
    bool insert(T type,T value) {
        int pos=find(type);
        if (pos>0) {
            // Replace its value if the type already exists.
            m_attributes[pos+1]=value
            return false;
        }

        // Append the new attribute.
        G_ASSERT(m_size<N-2)
        m_attributes[m_size++]=type;
        m_attributes[m_size++]=value;
        m_attributes[m_size]=0;
        return true;
    }

    /// Removes the given \a type (together with its value) from the list.
    bool remove(T type) {
        int pos=find(type);
        if (pos<0) {
            // Return immediately if the type does not exist.
            return false;
        }

        // Remove the attribute by overwriting it with succeeding attributes.
        while (pos<m_size) {
            m_attributes[pos]=m_attributes[pos+2];
            if (m_attributes[pos]==0)
                break;
            ++pos;
            m_attributes[pos]=m_attributes[pos+2];
            ++pos;
        }
        m_size-=2;
        m_attributes[m_size]=0;
        return true;
    }

    /// Returns a pointer to the attribute list in memory.
    operator T*() {
        return m_attributes;
    }

    /// Returns a constant pointer to the attribute list in memory.
    operator T const*() const {
        return m_attributes;
    }

  private:
    /// Searches for the given \a type in the list returning its index or -1 if
    /// it cannot be found.
    int find(T type) {
        int i=0;
        while (i<m_size) {
            // Avoid warnings about floating-point comparisons being unreliable.
            int type_int=static_cast<int>(m_attributes[i]);
            if (type_int==0)
                break;
            if (type_int==static_cast<int>(type))
                return i;
            i+=2;
        }
        return -1;
    }

    /// Current size of the list, i.e. the index of the terminating zero.
    int m_size;
    /// Tightly packed array of type / value pairs.
    T m_attributes[N];
};

/**
 * \name Type definitions as required by the WGL_ARB_pixel_format extension.
 */
//@{
typedef AttributeList<int> AttributeListi;
typedef AttributeList<float> AttributeListf;
//@}

} // namespace system

} // namespace gale

#endif // ATTRIBUTELIST_H