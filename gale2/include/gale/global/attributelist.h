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

#ifndef ATTRIBUTELIST_H
#define ATTRIBUTELIST_H

/**
 * \file
 * A static array implementation to store attribute pairs
 */

namespace gale {

namespace global {

/**
 * Zero terminated list of type / value pairs that is used to pass attributes to
 * several API calls.
 */
template<typename T,int N=256>
class AttributeList
{
  public:

    /// Definition for external access to the data type.
    typedef T Type;

    /// Creates an empty list.
    AttributeList() {
        clear();
    }

    /// Marks the list as empty.
    void clear() {
        m_size=0;
        m_attributes[0]=0;
    }

    /// Returns the number of entries in the list, i.e. 0 if the list is empty.
    int size() const {
        return m_size;
    }

    /// Appends / replaces the specified \a type and \a value to / in the list.
    bool insert(T const type,T const value) {
        if (m_size>=N-2) {
            return false;
        }

        int pos=find(type);
        if (pos>0) {
            // Replace its value if the type already exists.
            m_attributes[pos+1]=value;
            return false;
        }

        // Append the new attribute.
        m_attributes[m_size++]=type;
        m_attributes[m_size++]=value;
        m_attributes[m_size]=0;

        return true;
    }

    /// Removes the given \a type (together with its value) from the list.
    bool remove(T const type) {
        int pos=find(type);
        if (pos<0) {
            // Return immediately if the type does not exist.
            return false;
        }

        // Remove the attribute by overwriting it with succeeding attributes.
        while (pos+2<=m_size) {
            m_attributes[pos]=m_attributes[pos+2];
            ++pos;
        }
        m_size-=2;

        return true;
    }

    /// Returns a pointer to the attribute list in memory.
    operator T*() {
        return m_attributes;
    }

    /// Returns a pointer to the constant attribute list in memory.
    operator T const*() const {
        return m_attributes;
    }

  private:

    /// Searches for the given \a type in the list returning its index or -1 if
    /// it cannot be found.
    int find(T const type) {
        int i=0;
        while (i<m_size) {
            // Avoid warnings about floating-point comparisons being unreliable.
            int type_int=static_cast<int>(m_attributes[i]);
            if (type_int==0) {
                break;
            }
            if (type_int==static_cast<int>(type)) {
                return i;
            }
            i+=2;
        }
        return -1;
    }

    int m_size; ///< Current size of the list, i.e. the index of the terminating zero.
    T m_attributes[N]; ///< Tightly packed array of type / value pairs.
};

/**
 * \name Type definitions as required to describe pixel format attributes
 */
//@{

typedef AttributeList<int> AttributeListi;
typedef AttributeList<float> AttributeListf;

//@}

} // namespace global

} // namespace gale

#endif // ATTRIBUTELIST_H
