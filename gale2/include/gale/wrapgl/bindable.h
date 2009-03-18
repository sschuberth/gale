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
 * All objects that can be bound to the OpenGL state should be derived from this
 * class. The template argument \c T is the name for the target, \c B is the
 * name to query the binding, and \c M is the name to query the object's maximum
 * size.
 */
template<GLenum T,GLenum B,GLenum M>
class Bindable
{
  public:

    static GLenum const TARGET  = T; ///< The target's name, e.g. \c GL_TEXTURE_2D.
    static GLenum const BINDING = B; ///< The binding's name, e.g. \c GL_TEXTURE_BINDING_2D.
    static GLenum const MAX     = M; ///< The name to query the maximum size, e.g. \c GL_MAX_TEXTURE_SIZE.

    /// Returns an object's maximum size. The exact interpretation depends on
    /// the object type, i.e. for textures this is the maximum extent in texels
    /// in any dimension.
    static GLint maxSize() {
        GLint param;
        glGetIntegerv(MAX,&param);
        G_ASSERT_OPENGL
        return param;
    }

    /// Returns the handle of the currently bound object.
    static GLuint getCurrent() {
        GLint param;
        glGetIntegerv(BINDING,&param);
        G_ASSERT_OPENGL
        return static_cast<GLuint>(param);
    }

    /// Sets the current binding to the object described by \a handle. If
    /// \a handle is 0, the current object will be unbound.
    virtual void setCurrent(GLuint handle) const=0;

    /// Checks whether the given \a handle describes a valid object.
    virtual bool isValid(GLuint handle) const=0;

    /// Returns this object's handle.
    GLuint handle() const {
        return m_handle;
    }

    /// Binds this object to the OpenGL state, making it the current one.
    void makeCurrent() const {
        setCurrent(m_handle);
    }

  protected:

    GLuint m_handle; ///< This object's handle.
};

} // namespace wrapgl

} // namespace gale

#endif // BINDABLE_H
