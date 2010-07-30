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

#ifndef SHADEROBJECT_H
#define SHADEROBJECT_H

/**
 * \file
 * Wrappers for Shader / Program Objects
 */

#include "bindable.h"

namespace gale {

namespace wrapgl {

#include "GLEX_VERSION_2_0.h"

/**
 * This is a wrapper class for Shader Objects as promoted to OpenGL 2.0, see
 * <http://www.opengl.org/documentation/specs/version2.0/glspec20.pdf>
 */
class ShaderObject
{
  public:

    /// Checks whether the \a handle is a shader.
    static bool isOfType(GLuint const handle) {
        GLboolean result=glIsShader && glIsShader(handle);
        G_ASSERT_OPENGL
        return result!=GL_FALSE;
    }

    /// Creates a shader of the given \a type, e.g. GL_FRAGMENT_SHADER or
    /// GL_VERTEX_SHADER.
    ShaderObject(GLenum type)
    :   m_handle(0)
    {
        if (GLEX_VERSION_2_0 || GLEX_VERSION_2_0_init()) {
            m_handle=glCreateShader(type);
            G_ASSERT_OPENGL
        }
    }

    /// Deletes the shader and frees its resources.
    ~ShaderObject() {
        if (glDeleteShader) {
            glDeleteShader(m_handle);
            G_ASSERT_OPENGL
        }
    }

    /// Returns this shader's handle.
    GLuint handle() const {
        return m_handle;
    }

    /// Compiles the shader and returns whether it succeed or not.
    bool compile() const {
        glCompileShader(m_handle);
        G_ASSERT_OPENGL
        return getParameter(GL_COMPILE_STATUS)!=GL_FALSE;
    }

    /// Returns the compile log in \a infoLog of size \a bufSize. The actual
    /// number of characters written into \a infoLog, excluding the terminating \\0,
    /// is returned in \a length, if specified.
    void getLog(GLchar* infoLog,GLsizei bufSize,GLsizei* length=NULL) const {
        glGetShaderInfoLog(m_handle,bufSize,length,infoLog);
        G_ASSERT_OPENGL
    }

    /// Returns the integer parameter specified by \a name into \a values.
    GLint getParameter(GLenum const name) const {
        GLint value;
        glGetShaderiv(m_handle,name,&value);
        G_ASSERT_OPENGL
        return value;
    }

    /// Returns the shader source code in \a source of size \a bufSize. The actual
    /// number of characters written into \a source, excluding the terminating \\0,
    /// is returned in \a length, if specified.
    void getSource(GLchar* source,GLsizei bufSize,GLsizei* length=NULL) const {
        glGetShaderSource(m_handle,bufSize,length,source);
        G_ASSERT_OPENGL
    }

    /// Sets the shader source code to the array of \a strings with \a count
    /// entries. If the \a strings are not \\0 terminated, their \a lengths need
    /// to be specified.
    void setSource(GLchar const** strings,GLsizei count=1,GLint const* lengths=NULL) const {
        glShaderSource(m_handle,count,strings,lengths);
        G_ASSERT_OPENGL
    }

    /// Convenience method to check for any log entries.
    bool hasLog() const {
        // Some GLSL implementations return a single \0 for an empty log.
        return getParameter(GL_INFO_LOG_LENGTH)>1;
    }

  protected:

    GLuint m_handle; ///< The handle identifying the object.
};

/**
 * This is a bindable OpenGL object implementation for Program Objects, see
 * <http://www.opengl.org/documentation/specs/version2.0/glspec20.pdf>
 */
class ProgramObject:public Bindable<GL_CURRENT_PROGRAM,ProgramObject>
{
    template<GLenum B,class I> friend class Bindable;

  public:

    /// Sets the current binding to the object described by \a handle. If
    /// \a handle is 0, the current object will be unbound.
    static void setCurrent(GLuint const handle) {
        glUseProgram(handle);
        G_ASSERT_OPENGL
    }

    /// Attaches the given \a shader to this program.
    void attach(ShaderObject const& shader) {
        glAttachShader(m_handle,shader.handle());
        G_ASSERT_OPENGL
    }

    /// Detaches the given \a shader from this program.
    void detach(ShaderObject const& shader) {
        glDetachShader(m_handle,shader.handle());
        G_ASSERT_OPENGL
    }

    /// Links the program and returns whether it succeed or not.
    bool link() const {
        glLinkProgram(m_handle);
        G_ASSERT_OPENGL
        return getParameter(GL_LINK_STATUS)!=GL_FALSE;
    }

    /// Validates the program with respect to the current OpenGL state and
    /// returns whether the program is guaranteed to execute or not.
    bool validate() const {
        glValidateProgram(m_handle);
        G_ASSERT_OPENGL
        return getParameter(GL_VALIDATE_STATUS)!=GL_FALSE;
    }

    /// Returns the up to \a maxCount handles of the shaders attached to this
    /// program in \a obj. The actual number of handles written into \a shaders
    /// is returned in \a count, if specified.
    void getAttachedShaders(GLuint* obj,GLsizei maxCount,GLsizei* count=NULL) const {
        glGetAttachedShaders(m_handle,maxCount,count,obj);
        G_ASSERT_OPENGL
    }

    /// Returns the program log in \a infoLog of size \a bufSize. The actual
    /// number of characters written into \a infoLog, excluding the terminating \\0,
    /// is returned in \a length, if specified.
    void getLog(GLchar* infoLog,GLsizei bufSize,GLsizei* length=NULL) const {
        glGetProgramInfoLog(m_handle,bufSize,length,infoLog);
        G_ASSERT_OPENGL
    }

    /// Returns the integer parameter specified by \a name into \a values.
    GLint getParameter(GLenum const name) const {
        GLint value;
        glGetProgramiv(m_handle,name,&value);
        G_ASSERT_OPENGL
        return value;
    }

    /// Convenience method to check for any log entries.
    bool hasLog() const {
        // Some GLSL implementations return a single \0 for an empty log.
        return getParameter(GL_INFO_LOG_LENGTH)>1;
    }

  private:

    /// Creates a new (initially unbound) OpenGL object and stores the \a handle.
    static void createObject(GLuint& handle) {
        handle=0;
        if (GLEX_VERSION_2_0 || GLEX_VERSION_2_0_init()) {
            handle=glCreateProgram();
            G_ASSERT_OPENGL
        }
    }

    /// Destroys the OpenGL object identified by \a handle. If the object is
    /// currently bound, the default object becomes current.
    static void destroyObject(GLuint const handle) {
        if (glDeleteProgram) {
            glDeleteProgram(handle);
            G_ASSERT_OPENGL
        }
    }

    /// Checks whether the \a handle is of this object's type.
    static bool isOfType(GLuint const handle) {
        GLboolean result=glIsProgram && glIsProgram(handle);
        G_ASSERT_OPENGL
        return result!=GL_FALSE;
    }
};

} // namespace wrapgl

} // namespace gale

#endif // SHADEROBJECT_H
