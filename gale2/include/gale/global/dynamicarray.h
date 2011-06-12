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

#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

/**
 * \file
 * A dynamic array implementation to store arbitrary objects
 */

#include "../math/essentials.h"
#include "../meta/tools.h"

namespace gale {

namespace global {

/**
 * Simple dynamic array implementation to be used in favor of e.g. std::vector
 * for very small programs that cannot use STL due to size constraints. In
 * contrast to std::vector, for efficiency the default constructor is only
 * called on non-built-in types, and objects that contain pointers to themselves
 * are not supported.
 */
template<class T>
class DynamicArray
{
  public:

    /// Definition for external access to the data type.
    typedef T Type;

    /**
     * \name Constructors and destructor
     */
    //@{

    /// Creates a dynamic array, optionally of the given \a size and \a capacity.
    DynamicArray(int const size=0,int const capacity=0)
    :   m_data(NULL)
    ,   m_size(0)
    ,   m_capacity(0)
    {
        setCapacity(capacity);
        setSize(size);
    }

    /// Creates a deep copy of the given dynamic array.
    DynamicArray(DynamicArray const& other)
    :   m_data(NULL)
    ,   m_size(0)
    ,   m_capacity(0)
    {
        setSize(other.m_size);

        // The copy constructor is called when copying to an newly created
        // object, so we just create all array members in-place for their part.
        for (int i=0;i<m_size;++i) {
            new(&m_data[i]) T(other.m_data[i]);
        }
    }

    /// Creates a dynamic array from the given static array.
    template<int size>
    DynamicArray(T const (&array)[size])
    :   m_data(NULL)
    ,   m_size(0)
    ,   m_capacity(0)
    {
        insert(array);
    }

    /// Destroys all items in the array and frees all memory.
    ~DynamicArray() {
        clear();
        free(m_data);
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

    /// Casts \c this array to a pointer of type \a T. As an intended side
    /// effect, this also provides indexed data access.
    operator T*() {
        return data();
    }

    /// Casts \c this array to a pointer of type \a T \c const. As an intended
    /// side effect, this also provides indexed data access.
    operator T const*() const {
        return data();
    }

    /// Returns a reference to the first element in the array.
    T& first() {
        return m_data[0];
    }

    /// Returns a reference to the last element in the array.
    T& last() {
        return m_data[m_size-1];
    }

    /// Returns a constant reference to the first element in the array.
    T const& first() const {
        return m_data[0];
    }

    /// Returns a constant reference to the last element in the array.
    T const& last() const {
        return m_data[m_size-1];
    }

    //@}

    /**
     * \name Initialization / assignment operators
     */
    //@{

    /// Deeply copies the \a other dynamic array to this dynamic array.
    DynamicArray& operator=(DynamicArray const& other) {
        setCapacity(other.m_capacity);
        setSize(other.m_size);

        // The assignment operator is called when copying to an already existing
        // object, so we just assign all array members for their part.
        for (int i=0;i<m_size;++i) {
            // NOTE: It is T's assignment operator's job to make sure this
            // causes no memory leaks.
            m_data[i]=other.m_data[i];
        }

        return *this;
    }

    /// Returns an array initializer object for a scalar assignment to be able to
    /// use a comma separated list of values for array assignment.
    meta::ArrayInitializer<T> operator=(T const& value) {
        m_data[0]=value;
        return meta::ArrayInitializer<T>(m_data+1);
    }

    //@}

    /**
     * \name Capacity and size related methods
     */
    //@{

    /// Returns the array's current capacity.
    int getCapacity() const {
        return m_capacity;
    }

    /// Sets the array's \a capacity if it is not less than its size. Use 0 as
    /// the \a capacity to trim the memory usage to the actual array's size.
    void setCapacity(int capacity) {
        if (capacity<m_size) {
            capacity=m_size;
        }

        if (capacity!=m_capacity) {
            T* data=static_cast<T*>(realloc(m_data,capacity*sizeof(T)));
            if (data) {
                m_data=data;
                m_capacity=capacity;
            }
        }
    }

    /// Returns the array's current size.
    int getSize() const {
        return m_size;
    }

    /// Set the new \a size of the array, adjusts the capacity if required.
    void setSize(int size) {
        if (size<0) {
            size=0;
        }

        int i;

        // If we shrink in size, destroy abundant items.
        for (i=size;i<m_size;++i) {
            m_data[i].~T();
        }

        if (size>m_capacity) {
            if (m_capacity==0) {
                // Get only the needed amount of memory at first.
                setCapacity(size);
            }
            else {
                // Reserve memory for 1.5 times the new size.
                setCapacity(size+size/2);
            }
        }

        // If we grow in size, construct insetted items.
        for (i=m_size;i<size;++i) {
            new(&m_data[i]) T;
        }

        m_size=size;
    }

    //@}

    /**
     * \name Array modification methods
     */
    //@{

    /// Destroys all items in the array and sets its size to 0. The capacity
    /// remains unchanged.
    void clear() {
        for (int i=0;i<m_size;++i) {
            m_data[i].~T();
        }
        m_size=0;
    }

    /// Inserts an \a item at the given \a position into the array. If \a position
    /// is -1, the item gets appended at the end of the array. Returns the index
    /// of the newly added item.
    int insert(T const& item,int position=-1) {
        if (position<0 || position>m_size) {
            position=m_size;
        }

        // Adjust the size for a new object.
        setSize(m_size+1);

        int offset=m_size-position-1;
        if (offset>0) {
            // Move insetted items to create a gap.
            memmove(&m_data[position+1],&m_data[position],offset*sizeof(T));
        }

        m_data[position]=item;

        return position;
    }

    /// Inserts a dynamic \a array at the given \a position into the dynamic array.
    /// If \a position is -1, the item gets appended at the end of the array.
    /// Returns the first index of the newly added array.
    int insert(DynamicArray const& array,int position=-1) {
        if (position<0 || position>m_size) {
            position=m_size;
        }

        // Adjust the size for a new object.
        setSize(m_size+array.m_size);

        // Move insetted items to create a gap.
        int offset=m_size-position-array.m_size;
        if (offset>0) {
            memmove(&m_data[position+array.m_size],&m_data[position],offset*sizeof(T));
        }

        memcpy(&m_data[position],array,array.m_size*sizeof(T));

        return position;
    }

    /// Inserts a static \a array at the given \a position into the dynamic array.
    /// If \a position is -1, the item gets appended at the end of the array.
    /// Returns the first index of the newly added array.
    template<int size>
    int insert(T const (&array)[size],int position=-1) {
        if (position<0 || position>m_size) {
            position=m_size;
        }

        // Adjust the size for a new object.
        setSize(m_size+size);

        // Move insetted items to create a gap.
        int offset=m_size-position-size;
        if (offset>0) {
            memmove(&m_data[position+size],&m_data[position],offset*sizeof(T));
        }

        memcpy(&m_data[position],array,size*sizeof(T));

        return position;
    }

    /// Assumes the array to be sorted and inserts \a item accordingly. If
    /// \a forced, duplicate entries are be allowed. Returns the index of the
    /// newly added item.
    int insertSorted(T const& item,bool const forced=false) {
        int index;
        if (!findSorted(item,index) || forced) {
            insert(item,index);
        }
        return index;
    }

    /// Removes \a count items starting at \a begin from the array. If \a begin
    /// is negative, item are removed from the end of the array. If \a count is
    /// -1, all remaining items starting at \a begin are removed.
    void remove(int begin=-1,int count=1) {
        if (begin<0) {
            begin=m_size+begin;
        }

        int end=begin+count;
        if (count<0 || end>m_size) {
            end=m_size;
        }

        // Destroy removed items.
        for (int i=begin;i<end;++i) {
            m_data[i].~T();
        }

        // Move abundant items to close the gap.
        int offset=m_size-end;
        if (offset>0) {
            memmove(&m_data[begin],&m_data[end],offset*sizeof(T));
        }

        m_size-=end-begin;
    }

    //@}

    /**
     * \name Find methods
     */
    //@{

    /// Returns the first array index of \a item, or -1 if it cannot be found.
    int find(T const& item) const {
        T* ptr=m_data;
        for (int i=0;i<m_size;++i) {
            if (*ptr++==item) {
                return i;
            }
        }
        return -1;
    }

    /// Returns the last array index of \a item, or -1 if it cannot be found.
    int findRev(T const& item) const {
        T* ptr=m_data+m_size;
        for (int i=m_size;--i>=0;) {
            if (*--ptr==item) {
                return i;
            }
        }
        return -1;
    }

    /// Assumes the array to be sorted and returns whether \a item could be
    /// found and at which \a index. If it was not found, \a index contains the
    /// position where it was expected to be found.
    bool findSorted(T const& item,int& index) const {
        index=0;

        int first=0,last=m_size-1;

        // Use binary search to find the item.
        while (first<=last) {
            index=(first+last)/2;

            if (item>m_data[index]) {
                ++index;
                first=index;
            }
            else if (item<m_data[index]) {
                last=index-1;
            }
            else {
                return true;
            }
        }

        return false;
    }

    //@}

  protected:

    T* m_data;      ///< Pointer to the memory chunk storing the array.
    int m_capacity; ///< The array's capacity, i.e. allocated memory, in units of T.
    int m_size;     ///< The array's size, i.e. used memory, in units of T.
};

} // namespace global

} // namespace gale

#endif // DYNAMICARRAY_H
