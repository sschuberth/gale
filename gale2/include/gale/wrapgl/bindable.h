/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at https://github.com/sschuberth/gale/.
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
 * A basis for bindable OpenGL objects
 */

#include "../global/defines.h"
#include "../global/platform.h"

namespace gale {

namespace wrapgl {

/**
 * This is a wrapper class implementing the Curiously Recurring Template Pattern (CRTP) for all
 * objects that can be bound to the OpenGL state. The template argument \c B is the name to query
 * the binding and \c I refers to the implementation class, which needs to provide the following
 * static methods: createObject(), destroyObject(), setCurrent(), hasSameType() and optionally
 * getCurrent().
 */
template<GLenum B,class I>
class Bindable
{
  public:

    /// Returns the handle of the currently bound object of type \c B.
    static GLuint getCurrent() {
        GLint param;
        glGetIntegerv(B,&param);
        G_ASSERT_OPENGL
        return static_cast<GLuint>(param);
    }

    /// Releases this object type from the OpenGL state, restoring the default.
    static void release() {
        I::setCurrent(0);
    }

    /// Creates a new (initially unbound) OpenGL object.
    Bindable()
    :   m_dirty_state(false)
    {
        I::createObject(m_handle);
    }

    /// Destroys this OpenGL object. If the object is currently bound, the
    /// default object becomes current.
    ~Bindable() {
        I::destroyObject(m_handle);
    }

    /// Returns this object's handle.
    GLuint handle() const {
        return m_handle;
    }

    /// Returns whether this object is in use as part of current OpenGL state.
    bool isCurrent() const {
        return I::getCurrent()==m_handle;
    }

    /// Binds this object to the OpenGL state, making it the current one.
    void bind() const {
        I::setCurrent(m_handle);
    }

    /// Returns whether the object currently is consistent.
    bool isDirtyState() const {
        return m_dirty_state;
    }

    /// Sets whether the object currently is consistent.
    void setDirtyState(bool const state) const {
        m_dirty_state=state;
    }

    /// Checks whether this object's handle is valid.
    bool isValidHandle() const {
        return I::hasSameType(m_handle);
    }

  protected:

    bool mutable m_dirty_state; ///< Indicates whether the object currently represents a valid state vector.

    GLuint m_handle; ///< The handle identifying the object.
};

} // namespace wrapgl

} // namespace gale
