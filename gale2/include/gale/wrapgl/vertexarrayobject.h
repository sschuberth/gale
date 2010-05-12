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

#ifndef VERTEXARRAYOBJECT_H
#define VERTEXARRAYOBJECT_H

/**
 * \file
 * A wrapper for Vertex Array Objects
 */

#include "bindable.h"

namespace gale {

namespace wrapgl {

#include "GLEX_ARB_vertex_array_object.h"

/**
 * This is a bindable OpenGL object implementation for Vertex Array Objects, see
 * <http://www.opengl.org/registry/specs/ARB/vertex_array_object.txt>
 */
class VertexArrayObject:public Bindable<GL_VERTEX_ARRAY_BINDING,VertexArrayObject>
{
    template<GLenum B,class I> friend class Bindable;

  public:

    /// Sets the current binding to the object described by \a handle. If
    /// \a handle is 0, the current object will be unbound.
    static void setCurrent(GLuint const handle) {
        glBindVertexArray(handle);
        G_ASSERT_OPENGL
    }

  private:

    /// Creates a new (initially unbound) OpenGL object and stores the \a handle.
    static void createObject(GLuint& handle) {
        handle=0;
        if (GLEX_ARB_vertex_array_object || GLEX_ARB_vertex_array_object_init()) {
            glGenVertexArrays(1,&handle);
            G_ASSERT_OPENGL
        }
    }

    /// Destroys the OpenGL object identified by \a handle. If the object is
    /// currently bound, the default object becomes current.
    static void destroyObject(GLuint const handle) {
        if (glDeleteVertexArrays) {
            glDeleteVertexArrays(1,&handle);
            G_ASSERT_OPENGL
        }
    }

    /// Checks whether the \a handle is of this object's type.
    static bool isOfType(GLuint const handle) {
        GLboolean result=glIsVertexArray && glIsVertexArray(handle);
        G_ASSERT_OPENGL
        return result!=GL_FALSE;
    }
};

} // namespace wrapgl

} // namespace gale

#endif // VERTEXARRAYOBJECT_H
