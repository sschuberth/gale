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
 * This is a bindable OpenGL object implementation for Vertex Buffer Objects, see
 * <http://www.opengl.org/registry/specs/ARB/vertex_buffer_object.txt>
 */
template<GLenum T>
class VertexBufferObject:public Bindable<T,VertexBufferObject<T> >
{
    template<GLenum B,class I> friend class Bindable;

  public:

    /// Sets the current binding to the object described by \a handle. If
    /// \a handle is 0, the current object will be unbound.
    static void setCurrent(GLuint handle) {
        glBindBufferARB(T,handle);
        G_ASSERT_OPENGL
    }

    /**
     * \name Methods to operate on the object's data
     */
    //@{

    /// Copies the \a size number of bytes, optionally starting at \a offset,
    /// from the object's contents to \a data.
    void getData(GLsizeiptrARB size,void* data,GLintptrARB offset=0) const {
        makeCurrent();
        glGetBufferSubDataARB(T,offset,size,data);
        G_ASSERT_OPENGL
    }

    /// Initializes or sets the object's store to the \a size number of bytes
    /// pointed to by \a data. Optionally defines a usage pattern for the data.
    void setData(GLsizeiptrARB size,void const* data,GLenum usage=GL_STATIC_DRAW_ARB) const {
        makeCurrent();
        glBufferDataARB(T,size,data,usage);
        G_ASSERT_OPENGL
    }

    /// Modifies the object's store to the \a size number of bytes pointed to by
    /// \a data, optionally starting at the given \a offset.
    void setData(GLsizeiptrARB size,void const* data,GLintptrARB offset) const {
        makeCurrent();
        glBufferSubDataARB(T,offset,size,data);
        G_ASSERT_OPENGL
    }

    /// Returns the size of the object's data store.
    GLsizeiptrARB getSize() const {
        return (GLsizeiptrARB)getParameter(GL_BUFFER_SIZE_ARB);
    }

    /// Returns the usage type of the object's data store.
    GLenum getUsageType() const {
        return (GLenum)getParameter(GL_BUFFER_USAGE_ARB);
    }

    /// Returns the access type of the object's data store.
    GLenum getAccessType() const {
        return (GLenum)getParameter(GL_BUFFER_ACCESS_ARB);
    }

    /// Returns whether the object's data store is currently mapped to
    /// client memory.
    bool isMapped() const {
        return getParameter(GL_BUFFER_MAPPED_ARB)!=GL_FALSE;
    }

    /// Maps the object's data store to client memory for direct access using
    /// the optionally specified \a access type, and returns the pointer.
    void* map(GLenum access=GL_WRITE_ONLY_ARB) const {
        makeCurrent();
        glMapBufferARB(T,access);
        G_ASSERT_OPENGL
    }

    /// Unmaps the object's data store from client memory, returns \c true on
    /// success, \c false otherwise.
    bool unmap() const {
        makeCurrent();
        GLboolean result=glUnmapBufferARB(T);
        G_ASSERT_OPENGL
        return result!=GL_FALSE;
    }

    /// If the object's data store is currently mapped to client memory, returns
    /// the pointer to it, or \c NULL otherwise.
    void* getMappedData() const {
        makeCurrent();
        GLvoid* value;
        glGetBufferPointervARB(T,GL_BUFFER_MAP_POINTER_ARB,&value);
        return value;
    }

    //@}

  protected:

    /// Creates a new (initially unbound) OpenGL object and stores the \a handle.
    static void createObject(GLuint& handle) {
        handle=0;
        if (GLEX_ARB_vertex_buffer_object || GLEX_ARB_vertex_buffer_object_init()) {
            glGenBuffersARB(1,&handle);
        }
        G_ASSERT_OPENGL
    }

    /// Destroys the OpenGL object identified by \a handle. If the object is
    /// currently bound, the default object becomes current.
    static void destroyObject(GLuint handle) {
        glDeleteBuffersARB(1,&handle);
        G_ASSERT_OPENGL
    }

    /// Checks whether the given object \a handle is valid.
    static bool isValidObject(GLuint handle) {
        GLboolean result=glIsBufferARB(handle);
        G_ASSERT_OPENGL
        return result!=GL_FALSE;
    }

  private:

    /// Returns the buffer parameter specified by \a name.
    GLint getParameter(GLenum name) const {
        makeCurrent();
        GLint value;
        glGetBufferParameterivARB(T,name,&value);
        return value;
    }
};

/**
 * \name Type definitions for the different buffer types
 */
//@{

typedef VertexBufferObject<GL_ARRAY_BUFFER_ARB> ArrayBufferObject;
typedef VertexBufferObject<GL_ELEMENT_ARRAY_BUFFER_ARB> IndexBufferObject;

//@}

} // namespace wrapgl

} // namespace gale

#endif // VERTEXBUFFEROBJECT_H
