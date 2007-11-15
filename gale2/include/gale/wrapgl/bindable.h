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

#ifndef BINDABLE_H
#define BINDABLE_H

/**
 * \file
 * Basis for bindable OpenGL objects
 */

#include "../global/defines.h"
#include "../global/platform.h"

namespace gale {

namespace wrapgl {

/**
 * All objects that can be bound to the OpenGL state should be derived from this
 * class. The template argument \c C is the derived class, \c T is the enum for
 * the target, \c B the enum for the binding, and \c M the enum for querying the
 * object's maximum size.
 */
template<class C,GLenum T,GLenum B,GLenum M>
class Bindable
{
  public:

    static GLenum const TARGET  = T; ///< The enum value for the target.
    static GLenum const BINDING = B; ///< The enum value for the binding.
    static GLenum const MAX     = M; ///< The enum value to query the maximum size.

    /// Returns the object's maximum size. The exact interpretation depends on
    /// the object type, i.e. for textures this is the maximum dimension in
    /// texels, for frame buffers the maximum number of color buffer attachments.
    static GLint getMaxSize() {
        GLint param;
        glGetIntegerv(MAX,&param);
        G_ASSERT_OPENGL
        return param;
    }

    /// Returns the ID of the currently bound object.
    static GLuint getCurrentBinding() {
        GLint param;
        glGetIntegerv(BINDING,&param);
        G_ASSERT_OPENGL
        return static_cast<GLuint>(param);
    }

    /// Returns this object's ID.
    GLuint getID() const {
        return m_id;
    }

    /// Sets the current binding to the object described by \a id or to this
    /// object if the ID is negative.
    virtual void setCurrentBinding(GLint id=-1) const=0;

    /// Binds this object to the OpenGL state, making it the current one.
    void bind() const {
        if (getCurrentBinding()!=getID()) {
            setCurrentBinding();
        }
    }

    /// Unbinds this object from the OpenGL state, restoring the default object.
    void unbind() const {
        setCurrentBinding(0);
    }

  protected:

    /// Helper constructor for special objects with a fixed \a id.
    Bindable(GLuint id=0):m_id(id) {
        // This is a trick to ensure derived classes implement this static method.
        G_ASSERT_CALL(C::isValid(id));
    }

    GLuint m_id; ///< The ID of this OpenGL object.
};

} // namespace wrapgl

} // namespace gale

#endif // BINDABLE_H
