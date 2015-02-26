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
 * A wrapper for Texture Objects
 */

#include "bindable.h"

namespace gale {

namespace wrapgl {

#include "GLEX_EXT_texture3D.h"

/**
 * This is a bindable OpenGL object implementation for Texture Objects of target
 * type \c T.
 */
template<GLenum T,GLenum B>
class TextureObject:public Bindable<B,TextureObject<T,B> >
{
    template<GLenum B,class I> friend class Bindable;

  public:

    /// Class constant for external access to the target type.
    static GLenum const TARGET=T;

    /**
     * \name Methods to operate on the texture's data
     */
    //@{

    /// Copies the texture in given \a data_format and \a data_type to the host
    /// memory location pointed to by \a data. Optionally selects a specific
    /// mipmap \a level.
    void getData(GLvoid* data,GLenum const data_format,GLenum const data_type,GLint const level=0) {
        bind();
        glGetTexImage(T,level,data_format,data_type,data);
        G_ASSERT_OPENGL
    }

    /// Returns the floating-point parameter specified by \a name into \a values.
    void getParameter(GLenum const name,GLfloat* values) const {
        bind();
        glGetTexParameterfv(T,name,values);
        G_ASSERT_OPENGL
    }

    /// Sets the floating-point parameter specified by \a name to \a values.
    void setParameter(GLenum const name,GLfloat const* values) const {
        bind();
        glTexParameterfv(T,name,values);
        G_ASSERT_OPENGL
    }

    /// Returns the integer parameter specified by \a name into \a values.
    void getParameter(GLenum const name,GLint* values) const {
        bind();
        glGetTexParameteriv(T,name,values);
        G_ASSERT_OPENGL
    }

    /// Sets the integer parameter specified by \a name to \a values.
    void setParameter(GLenum const name,GLint const* values) const {
        bind();
        glTexParameteriv(T,name,values);
        G_ASSERT_OPENGL
    }

    /// Returns the texture's priority to be resident in texture memory.
    GLclampf getPriority() const {
        GLfloat value;
        getParameter(GL_TEXTURE_PRIORITY,&value)
        return static_cast<GLclampf>(value);
    }

    /// Sets the texture's priority to be resident in texture memory.
    void setPriority(GLclampf const priority) const {
        bind();
        glPrioritizeTextures(1,&m_handle,priority);
        G_ASSERT_OPENGL
    }

    /// Returns whether the texture is resident in texture memory.
    bool isResident() const {
        return getParameter(GL_TEXTURE_RESIDENT)!=GL_FALSE;
    }

    //@}

  private:

    /**
     * \name Implementation of the bindable interface
     */
    //@{

    /// Creates a new (initially unbound) OpenGL object and stores the \a handle.
    static void createObject(GLuint& handle) {
        glGenTextures(1,&handle);
        G_ASSERT_OPENGL
    }

    /// Destroys the OpenGL object identified by \a handle. If the object is
    /// currently bound, the default object becomes current.
    static void destroyObject(GLuint const handle) {
        glDeleteTextures(1,&handle);
        G_ASSERT_OPENGL
    }

    /// Sets the current binding to the object described by \a handle. If
    /// \a handle is 0, the current object will be unbound.
    static void setCurrent(GLuint const handle) {
        glBindTexture(T,handle);
        G_ASSERT_OPENGL
    }

    /// Checks whether the \a handle is of this object's type.
    static bool hasSameType(GLuint const handle) {
        GLboolean result=glIsTexture(handle);
        G_ASSERT_OPENGL
        return result!=GL_FALSE;
    }

    //@}
};

/**
 * Minimal common interface for texture information. This is required as the
 * template texture classes do not share a common base class otherwise.
 */
struct G_NO_VTABLE TextureInfo
{
    /// Returns the target the texture is bound to.
    virtual GLenum target() const=0;
};

/**
 * A class representing a one-dimensional texture.
 */
class Texture1D:public TextureObject<GL_TEXTURE_1D,GL_TEXTURE_BINDING_1D>,public TextureInfo
{
  public:

    /// Returns the target the texture is bound to.
    GLenum target() const {
        return TARGET;
    }

    /// Initializes or sets a texture of the given \a width to the contents
    /// pointed to by \a data. The contents have the given \a data_format and
    /// \a data_type in host memory and are to be used as the given \a format
    /// in OpenGL. Optionally, \a mipmap \a level and a \a border may be
    /// specified.
    void setData(GLsizei const width,GLvoid const* data,GLenum const data_format,GLenum const data_type,GLint const format,GLint const level=0,bool const border=false) const {
        bind();
        glTexImage1D(TARGET,level,format,width,border,data_format,data_type,data);
        G_ASSERT_OPENGL
    }
};

/**
 * A class representing a two-dimensional texture.
 */
class Texture2D:public TextureObject<GL_TEXTURE_2D,GL_TEXTURE_BINDING_2D>,public TextureInfo
{
  public:

    /// Returns the target the texture is bound to.
    GLenum target() const {
        return TARGET;
    }

    /// Initializes or sets a texture of the given \a width and \a height to the
    /// contents pointed to by \a data. The contents have the given
    /// \a data_format and \a data_type in host memory and are to be used as the
    /// given \a format in OpenGL. Optionally, \a mipmap \a level and a
    /// \a border may be specified.
    void setData(GLsizei const width,GLsizei const height,GLvoid const* data,GLenum const data_format,GLenum const data_type,GLint const format,GLint const level=0,bool const border=false) const {
        bind();
        glTexImage2D(TARGET,level,format,width,height,border,data_format,data_type,data);
        G_ASSERT_OPENGL
    }
};

/**
 * A class representing a three-dimensional texture.
 */
class Texture3D:public TextureObject<GL_TEXTURE_3D_EXT,GL_TEXTURE_BINDING_3D_EXT>,public TextureInfo
{
  public:

    /// Returns the target the texture is bound to.
    GLenum target() const {
        return TARGET;
    }

    /// Initializes or sets a texture of the given \a width, \a height and
    /// \a depth to the contents pointed to by \a data. The contents have the
    /// given \a data_format and \a data_type in host memory and are to be used
    /// as the given \a format in OpenGL. Optionally, \a mipmap \a level and a
    /// \a border may be specified.
    void setData(GLsizei const width,GLsizei const height,GLsizei const depth,GLvoid const* data,GLenum const data_format,GLenum const data_type,GLint const format,GLint const level=0,bool const border=false) const {
        bind();
        glTexImage3DEXT(TARGET,level,format,width,height,depth,border,data_format,data_type,data);
        G_ASSERT_OPENGL
    }
};

} // namespace wrapgl

} // namespace gale
