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

#ifndef BINDABLE_H
#define BINDABLE_H

/**
 * \file
 * A basis for bindable OpenGL objects
 */

#include "../global/defines.h"
#include "../global/platform.h"

namespace gale {

namespace wrapgl {

/**
 * This is a wrapper class for all objects that can be bound to the OpenGL
 * state. The template argument \c B is the name to query the binding and \c I
 * refers to the implementation class.
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

    /// Creates a new (initially unbound) OpenGL object.
    Bindable() {
        I::createObject(m_handle);
    }

    /// Destroys this OpenGL object. If the object is currently bound, the
    /// default object becomes current.
    ~Bindable() {
        I::destroyObject(m_handle);
    }

    /// Binds this object to the OpenGL state, making it the current one.
    void makeCurrent() const {
        I::setCurrent(m_handle);
    }

    /// Checks whether this object's handle is valid.
    bool isValid() const {
        return I::isValidObject(m_handle);
    }

  protected:

    GLuint m_handle; ///< The handle identifying the object.
};

} // namespace wrapgl

} // namespace gale

#endif // BINDABLE_H
