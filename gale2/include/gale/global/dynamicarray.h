/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
 *
 * Copyright (C) 2005-2008  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
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

    /**
     * \name Constructors, destructor and assignment
     */
    //@{

    /// Creates an empty array without any memory allocated.
    DynamicArray(int size=0,int capacity=0)
    :   m_data(NULL)
    ,   m_size(0)
    ,   m_capacity(0)
    {
        setCapacity(capacity);
        setSize(size);
    }

    /// Creates a copy of the given dynamic array.
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

    /// Assigns an \a other dynamic array to this dynamic array.
    DynamicArray& operator=(DynamicArray const& other) {
        setSize(other.m_size);

        // The assignment operator is called when copying to an already existing
        // object, so we just assign all array members for their part.
        for (int i=0;i<m_size;++i) {
            m_data[i]=other.m_data[i];
        }

        return *this;
    }

    /// Creates a dynamic array from the given static array.
    template<size_t size>
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

    /// Returns a \c constant pointer to the internal data array.
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

    //@}

    /**
     * \name Initialization / assignment operators
     */
    //@{

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
            m_data=static_cast<T*>(realloc(m_data,capacity*sizeof(T)));
            m_capacity=capacity;
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
    /// is -1, the item gets appended at the end of the array.
    void insert(T const& item,int position=-1) {
        if (position==-1 || position>m_size) {
            position=m_size;
        }

        // Adjust the size for a new object.
        setSize(m_size+1);

        // Move insetted items to create a gap.
        memmove(&m_data[position+1],&m_data[position],(m_size-position-1)*sizeof(T));

        m_data[position]=item;
    }

    /// Inserts a static \a array at the given \a position into the dynamic array.
    /// If \a position is -1, the item gets appended at the end of the array.
    template<size_t size>
    void insert(T const (&array)[size],int position=-1) {
        if (position==-1 || position>m_size) {
            position=m_size;
        }

        // Adjust the size for a new object.
        setSize(m_size+size);

        // Move insetted items to create a gap.
        memmove(&m_data[position+size],&m_data[position],(m_size-position-size)*sizeof(T));

        memcpy(&m_data[position],array,size*sizeof(T));
    }

    /// Removes \a count items starting at \a begin from the array. If \a begin
    /// is negative, item are removed from the end of the array. If \a count is
    /// -1, all remaining items starting at \a begin are removed.
    void remove(int begin=-1,int count=1) {
        if (begin<0) {
            begin=m_size+begin;
        }

        int end=begin+count;
        if (count==-1 || end>m_size) {
            end=m_size;
        }

        // Destroy removed items.
        for (int i=begin;i<end;++i) {
            m_data[i].~T();
        }

        // Move abundant items to close the gap.
        memmove(&m_data[begin],&m_data[end],(m_size-end)*sizeof(T));

        m_size-=end-begin;
    }

    //@}

    /**
     * \name Sorted array methods
     */
    //@{

    /// Inserts an \a item into a sorted array. If \a forced, the item will also
    /// be inserted if it already exists, resulting in multiple occurrences.
    void insertSorted(T const& item,bool forced=false) {
        int index;
        if (!findSorted(item,index) || forced) {
            insert(item,index);
        }
    }

    /// Returns whether the \a item could be found in a sorted array, and at
    /// which \a index. If the \a item was not found, \a index contains the
    /// position where it was expected to be found.
    bool findSorted(T const& item,int& index) const {
        if (m_size<1) {
            return false;
        }

        int first=0,last=m_size-1;

        while (last-first>1) {
            index=(first+last)/2;

            if (item>m_data[index]) {
                first=index;
            }
            else if (item<m_data[index]) {
                last=index;
            }
            else {
                return true;
            }
        }

        if (item<=m_data[first]) {
            index=first;
        }
        else if (item<=m_data[last]) {
            index=last;
        }
        else {
            index=-1;
        }

        return index>0 && item==m_data[index];
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
