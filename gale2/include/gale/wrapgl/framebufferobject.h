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

#ifndef FRAMEBUFFEROBJECT_H
#define FRAMEBUFFEROBJECT_H

/**
 * \file
 * A wrapper for Framebuffer Objects
 */

#include "textureobject.h"

namespace gale {

namespace wrapgl {

#include "GLEX_EXT_framebuffer_object.h"
#include "GLEX_EXT_framebuffer_multisample.h"

#ifdef GLEX_EXT_FRAMEBUFFER_OBJECT_H
    #include "framebufferobjectext.h"
#endif

/**
 * This is a bindable OpenGL object implementation for Renderbuffer Objects, see
 * <http://www.opengl.org/registry/specs/{ARB,EXT}/framebuffer_object.txt>
 */
class RenderBufferObject:public Bindable<GL_RENDERBUFFER_BINDING,RenderBufferObject>
{
    template<GLenum B,class I> friend class Bindable;

  public:

    /// Sets the current binding to the object described by \a handle. If
    /// \a handle is 0, the current object will be unbound.
    static void setCurrent(GLuint const handle) {
        glBindRenderbuffer(GL_RENDERBUFFER,handle);
        G_ASSERT_OPENGL
    }

    /**
     * \name Methods to operate on the object's data
     */
    //@{

    /// Initializes or sets the object's storage to the given \a width, \a height
    /// and \a format, optionally with multi-sampling using the specified number
    /// of \a samples enabled.
    void setStorage(GLsizei const width,GLsizei const height,GLenum const format,GLsizei const samples=0) const {
        makeCurrent();

#if defined(GLEX_ARB_FRAMEBUFFER_OBJECT_H) || defined(GLEX_EXT_FRAMEBUFFER_MULTISAMPLE_H)
        if (glRenderbufferStorageMultisample) {
            glRenderbufferStorageMultisample(GL_RENDERBUFFER,samples,format,width,height);
        }
        else
#endif
        glRenderbufferStorage(GL_RENDERBUFFER,format,width,height);

        G_ASSERT_OPENGL
    }

    /// Returns the width of the object's storage.
    GLsizei getWidth() const {
        return static_cast<GLsizei>(getParameter(GL_RENDERBUFFER_WIDTH));
    }

    /// Returns the height of the object's storage.
    GLsizei getHeight() const {
        return static_cast<GLsizei>(getParameter(GL_RENDERBUFFER_HEIGHT));
    }

    /// Returns the format of the object's storage.
    GLenum getFormat() const {
        return static_cast<GLenum>(getParameter(GL_RENDERBUFFER_INTERNAL_FORMAT));
    }

    /// Returns the actual (not necessarily requested) number of bits for the
    /// red color component.
    GLint getRedBits() const {
        return static_cast<GLint>(getParameter(GL_RENDERBUFFER_RED_SIZE));
    }

    /// Returns the actual (not necessarily requested) number of bits for the
    /// green color component.
    GLint getGreenBits() const {
        return static_cast<GLint>(getParameter(GL_RENDERBUFFER_GREEN_SIZE));
    }

    /// Returns the actual (not necessarily requested) number of bits for the
    /// blue color component.
    GLint getBlueBits() const {
        return static_cast<GLint>(getParameter(GL_RENDERBUFFER_BLUE_SIZE));
    }

    /// Returns the actual (not necessarily requested) number of bits for the
    /// alpha color component.
    GLint getAlphaBits() const {
        return static_cast<GLint>(getParameter(GL_RENDERBUFFER_ALPHA_SIZE));
    }

    /// Returns the actual (not necessarily requested) number of bits for the
    /// depth buffer component.
    GLint getDepthBits() const {
        return static_cast<GLint>(getParameter(GL_RENDERBUFFER_DEPTH_SIZE));
    }

    /// Returns the actual (not necessarily requested) number of bits for the
    /// stencil buffer component.
    GLint getStencilBits() const {
        return static_cast<GLint>(getParameter(GL_RENDERBUFFER_STENCIL_SIZE));
    }

    /// Returns the actual (not necessarily requested) number of samples used
    /// for multi-sampling.
    GLsizei getSamples() const {
#if defined(GLEX_ARB_FRAMEBUFFER_OBJECT_H) || defined(GLEX_EXT_FRAMEBUFFER_MULTISAMPLE_H)
        return static_cast<GLsizei>(getParameter(GL_RENDERBUFFER_SAMPLES));
#else
        return 0;
#endif
    }

    //@}

  protected:

    /// Creates a new (initially unbound) OpenGL object and stores the \a handle.
    static void createObject(GLuint& handle) {
        handle=0;
#ifdef GLEX_ARB_FRAMEBUFFER_OBJECT_H
        if (GLEX_ARB_framebuffer_object || GLEX_ARB_framebuffer_object_init()) {
            glGenRenderbuffers(1,&handle);
            G_ASSERT_OPENGL
        }
#elif defined(GLEX_EXT_FRAMEBUFFER_OBJECT_H)
        if (GLEX_EXT_framebuffer_object || GLEX_EXT_framebuffer_object_init()) {
            glGenRenderbuffers(1,&handle);
            G_ASSERT_OPENGL
        }
#endif
    }

    /// Destroys the OpenGL object identified by \a handle. If the object is
    /// currently bound, the default object becomes current.
    static void destroyObject(GLuint const handle) {
        if (glDeleteRenderbuffers) {
            glDeleteRenderbuffers(1,&handle);
            G_ASSERT_OPENGL
        }
    }

    /// Checks whether the given object \a handle is valid.
    static bool isValidObject(GLuint const handle) {
        GLboolean result=glIsRenderbuffer && glIsRenderbuffer(handle);
        G_ASSERT_OPENGL
        return result!=GL_FALSE;
    }

  private:

    /// Returns the buffer parameter specified by \a name.
    GLint getParameter(GLenum const name) const {
        makeCurrent();
        GLint value;
        glGetRenderbufferParameteriv(GL_RENDERBUFFER,name,&value);
        G_ASSERT_OPENGL
        return value;
    }
};

/**
 * This is a bindable OpenGL object implementation for Framebuffer Objects, see
 * <http://www.opengl.org/registry/specs/{ARB,EXT}/framebuffer_object.txt>
 */
class FrameBufferObject:public Bindable<GL_FRAMEBUFFER_BINDING,FrameBufferObject>
{
    template<GLenum B,class I> friend class Bindable;

  public:

    /// Sets the current binding to the object described by \a handle. If
    /// \a handle is 0, the current object will be unbound.
    static void setCurrent(GLuint const handle,GLenum const target=GL_FRAMEBUFFER) {
        glBindFramebuffer(target,handle);
        G_ASSERT_OPENGL
    }

    /**
     * \name Methods related to the framebuffer object's target
     */
    //@{

    /// Initializes a framebuffer object to become the current read and draw
    /// buffer when it is bound.
    FrameBufferObject(GLenum const target=GL_FRAMEBUFFER)
    :   m_target(target) {}

    /// Binds the framebuffer object to the OpenGL state as the currently set
    /// target, making it the current read and / or draw buffer.
    void makeCurrent() const {
        setCurrent(m_handle,m_target);
    }

    /// Returns for which rendering operation the object becomes the target when
    /// it is bound.
    GLenum getTarget() const {
        return m_target;
    }

    /// Sets for which rendering operation the object becomes the target when it
    /// is bound
    void setTarget(GLenum const target) {
        m_target=target;
    }

    //@}

    /**
     * \name Methods to operate on the object's data
     */
    //@{

    /// Returns the buffer parameter specified by \a name.
    GLint getParameter(GLenum const attachment,GLenum const name) const {
        makeCurrent();
        GLint value;
        glGetFramebufferAttachmentParameteriv(m_target,attachment,name,&value);
        G_ASSERT_OPENGL
        return value;
    }

    /// Returns the status of the framebuffer to ensure its completeness.
    GLenum check() const {
        makeCurrent();
        GLenum status=glCheckFramebufferStatus(m_target);
        G_ASSERT_OPENGL
        return status;
    }

    /// Attaches the given \a level of the specified 1D \a texture to the
    /// framebuffer image identified by \a attachment.
    void attach(GLenum const attachment,Texture1D const& texture,GLint const level=0) const {
        makeCurrent();
        glFramebufferTexture1D(m_target,attachment,texture.target(),texture.handle(),level);
        G_ASSERT_OPENGL
    }

    /// Attaches the given \a level of the specified 2D \a texture to the
    /// framebuffer image identified by \a attachment.
    void attach(GLenum const attachment,Texture2D const& texture,GLint const level=0) const {
        makeCurrent();
        glFramebufferTexture2D(m_target,attachment,texture.target(),texture.handle(),level);
        G_ASSERT_OPENGL
    }

    /// Attaches the given \a level and \a layer of the specified 3D \a texture
    /// to the framebuffer image identified by \a attachment.
    void attach(GLenum const attachment,Texture3D const& texture,GLint const level=0,GLint const layer=0) const {
        makeCurrent();
        glFramebufferTexture3D(m_target,attachment,texture.target(),texture.handle(),level,layer);
        G_ASSERT_OPENGL
    }

    /// Attaches the specified \a buffer to the framebuffer image identified by
    /// \a attachment.
    void attach(GLenum const attachment,RenderBufferObject const& buffer) {
        glFramebufferRenderbuffer(m_target,attachment,GL_RENDERBUFFER,buffer.handle());
        G_ASSERT_OPENGL
    }

    /// Detaches the framebuffer image identified by \a attachment.
    void detach(GLenum const attachment) const {
        makeCurrent();

        // For detaching, we do not need to know which texture type or buffer
        // was attached.
        glFramebufferRenderbuffer(m_target,attachment,GL_RENDERBUFFER,0);
        G_ASSERT_OPENGL
    }

    //@}

  protected:

    /// Creates a new (initially unbound) OpenGL object and stores the \a handle.
    static void createObject(GLuint& handle) {
        handle=0;
#ifdef GLEX_ARB_FRAMEBUFFER_OBJECT_H
        if (GLEX_ARB_framebuffer_object || GLEX_ARB_framebuffer_object_init()) {
            glGenFramebuffers(1,&handle);
            G_ASSERT_OPENGL
        }
#elif defined(GLEX_EXT_FRAMEBUFFER_OBJECT_H)
        if (GLEX_EXT_framebuffer_object || GLEX_EXT_framebuffer_object_init()) {
            glGenFramebuffers(1,&handle);
            G_ASSERT_OPENGL
        }
#endif
    }

    /// Destroys the OpenGL object identified by \a handle. If the object is
    /// currently bound, the default object becomes current.
    static void destroyObject(GLuint const handle) {
        if (glDeleteFramebuffers) {
            glDeleteFramebuffers(1,&handle);
            G_ASSERT_OPENGL
        }
    }

    /// Checks whether the given object \a handle is valid.
    static bool isValidObject(GLuint const handle) {
        GLboolean result=glIsFramebuffer && glIsFramebuffer(handle);
        G_ASSERT_OPENGL
        return result!=GL_FALSE;
    }

    GLenum m_target; ///< Specifies for which rendering operation the object becomes the target when it is bound.
};

} // namespace wrapgl

} // namespace gale

#endif // FRAMEBUFFEROBJECT_H
