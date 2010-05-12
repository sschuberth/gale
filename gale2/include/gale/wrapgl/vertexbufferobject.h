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

#ifndef VERTEXBUFFEROBJECT_H
#define VERTEXBUFFEROBJECT_H

/**
 * \file
 * A wrapper for Vertex Buffer Objects
 */

#include "bindable.h"

namespace gale {

namespace wrapgl {

#include "GLEX_ARB_vertex_buffer_object.h"

/**
 * This is a bindable OpenGL object implementation for Vertex Buffer Objects of
 * target type \c T, see
 * <http://www.opengl.org/registry/specs/ARB/vertex_buffer_object.txt>
 */
template<GLenum T,GLenum B>
class VertexBufferObject:public Bindable<B,VertexBufferObject<T,B> >
{
    template<GLenum B,class I> friend class Bindable;

  public:

    /// Class constant for external access to the target type.
    static GLenum const TARGET=T;

    /// Sets the current binding to the object described by \a handle. If
    /// \a handle is 0, the current object will be unbound.
    static void setCurrent(GLuint const handle) {
        glBindBufferARB(T,handle);
        G_ASSERT_OPENGL
    }

    /**
     * \name Methods to operate on the buffer's data
     */
    //@{

    /// Copies the \a size number of bytes, optionally starting at \a offset,
    /// from the buffer's contents to \a data.
    void getData(GLsizeiptrARB const size,GLvoid* data,GLintptrARB const offset=0) const {
        makeCurrent();
        glGetBufferSubDataARB(T,offset,size,data);
        G_ASSERT_OPENGL
    }

    /// Initializes or sets the buffer's storage to the \a size number of bytes
    /// pointed to by \a data. Optionally defines a usage pattern for the data.
    void setData(GLsizeiptrARB const size,GLvoid const* data,GLenum const usage=GL_STATIC_DRAW_ARB) const {
        makeCurrent();
        glBufferDataARB(T,size,data,usage);
        G_ASSERT_OPENGL
    }

    /// Modifies the buffer's storage to the \a size number of bytes pointed to
    /// by \a data, optionally starting at the given \a offset.
    void setData(GLsizeiptrARB const size,GLvoid const* data,GLintptrARB const offset) const {
        makeCurrent();
        glBufferSubDataARB(T,offset,size,data);
        G_ASSERT_OPENGL
    }

    /// Returns the size of the buffer's storage in bytes.
    GLsizeiptrARB getSize() const {
        return static_cast<GLsizeiptrARB>(getParameter(GL_BUFFER_SIZE_ARB));
    }

    /// Returns the usage type of the buffer's storage.
    GLenum getUsageType() const {
        return static_cast<GLenum>(getParameter(GL_BUFFER_USAGE_ARB));
    }

    /// Returns the access type of the buffer's storage.
    GLenum getAccessType() const {
        return static_cast<GLenum>(getParameter(GL_BUFFER_ACCESS_ARB));
    }

    /// Returns whether the buffer's storage is currently mapped to client
    /// memory.
    bool isMapped() const {
        return getParameter(GL_BUFFER_MAPPED_ARB)!=GL_FALSE;
    }

    /// Maps the buffer's storage to client memory for direct access using the
    /// optionally specified \a access type, and returns the pointer.
    void* map(GLenum const access=GL_WRITE_ONLY_ARB) const {
        makeCurrent();
        GLvoid* data=glMapBufferARB(T,access);
        G_ASSERT_OPENGL
        return data;
    }

    /// Unmaps the buffer's storage from client memory, returns \c true on
    /// success, \c false otherwise.
    bool unmap() const {
        makeCurrent();
        GLboolean result=glUnmapBufferARB(T);
        G_ASSERT_OPENGL
        return result!=GL_FALSE;
    }

    /// If the buffer's storage is currently mapped to client memory, returns
    /// the pointer to it, or \c NULL otherwise.
    void* getBufferPointer() const {
        makeCurrent();
        GLvoid* data;
        glGetBufferPointervARB(T,GL_BUFFER_MAP_POINTER_ARB,&data);
        G_ASSERT_OPENGL
        return data;
    }

    //@}

  protected:

    /// Creates a new (initially unbound) OpenGL object and stores the \a handle.
    static void createObject(GLuint& handle) {
        handle=0;
        if (GLEX_ARB_vertex_buffer_object || GLEX_ARB_vertex_buffer_object_init()) {
            glGenBuffersARB(1,&handle);
            G_ASSERT_OPENGL
        }
    }

    /// Destroys the OpenGL object identified by \a handle. If the object is
    /// currently bound, the default object becomes current.
    static void destroyObject(GLuint const handle) {
        if (glDeleteBuffersARB) {
            glDeleteBuffersARB(1,&handle);
            G_ASSERT_OPENGL
        }
    }

    /// Checks whether the \a handle is of this object's type.
    static bool isOfType(GLuint const handle) {
        GLboolean result=glIsBufferARB && glIsBufferARB(handle);
        G_ASSERT_OPENGL
        return result!=GL_FALSE;
    }

  private:

    /// Returns the buffer parameter specified by \a name.
    GLint getParameter(GLenum const name) const {
        makeCurrent();
        GLint value;
        glGetBufferParameterivARB(T,name,&value);
        G_ASSERT_OPENGL
        return value;
    }
};

/**
 * \name Type definitions for the different buffer types
 */
//@{

typedef VertexBufferObject<GL_ARRAY_BUFFER_ARB,GL_ARRAY_BUFFER_BINDING_ARB> ArrayBufferObject;
typedef VertexBufferObject<GL_ELEMENT_ARRAY_BUFFER_ARB,GL_ELEMENT_ARRAY_BUFFER_BINDING_ARB> IndexBufferObject;

//@}

} // namespace wrapgl

} // namespace gale

#endif // VERTEXBUFFEROBJECT_H
