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

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

/**
 * \file
 * A frame buffer object wrapper
 */

#include "texture.h"

namespace gale {

namespace wrapgl {

#include "GLEX_EXT_framebuffer_object.h"

/**
 * This class provides an application-created frame buffer that can be bound to
 * a render surface to redirect OpenGL rendering to it.
 */
class FrameBuffer:public Bindable<FrameBuffer,GL_FRAMEBUFFER_EXT,GL_FRAMEBUFFER_BINDING_EXT,GL_MAX_COLOR_ATTACHMENTS_EXT>
{
  public:

    /**
     * A render buffer is one of the two image buffer types that can be attached
     * to a frame buffer (the other buffer type being textures).
     */
    class RenderBuffer:public Bindable<RenderBuffer,GL_RENDERBUFFER_EXT,GL_RENDERBUFFER_BINDING_EXT,GL_MAX_RENDERBUFFER_SIZE_EXT>
    {
      public:

        /// Checks whether the given \a id describes a valid render buffer.
        static bool isValid(GLuint const id) {
            return glIsRenderbufferEXT(id)!=GL_FALSE;
        }

        /// Creates a single render buffer.
        RenderBuffer() {
            if (GLEX_EXT_framebuffer_object!=GL_TRUE) {
                // Initialize an OpenGL extension to be able to replace the
                // system-provided frame buffer with a custom set of image buffers,
                // see <http://opengl.org/registry/specs/ARB/wgl_pixel_format.txt>.
                G_ASSERT_CALL(GLEX_EXT_framebuffer_object_init())
            }

            glGenRenderbuffersEXT(1,&m_id);
            G_ASSERT_OPENGL
        }

        /// Destroys this render buffer, freeing its ID again.
        ~RenderBuffer() {
            glDeleteRenderbuffersEXT(1,&m_id);
            G_ASSERT_OPENGL
        }

        /// Sets the current binding to the render buffer described by \a id or
        /// to the this render buffer if the ID is negative.
        void setCurrentBinding(GLint const id=-1) const {
            glBindRenderbufferEXT(TARGET,id<0?m_id:id);
            G_ASSERT_OPENGL
        }

        /// Initializes the render buffer with the given \a width, \a height and
        /// internal \a format.
        void init(GLsizei const width,GLsizei const height,GLenum const format) const {
            bind();
            glRenderbufferStorageEXT(TARGET,format,width,height);
            G_ASSERT_OPENGL
        }

        /// Returns the value(s) of the parameter with the given \a name.
        void getParameter(GLenum const name,GLint* const params) const {
            bind();
            glGetRenderbufferParameterivEXT(TARGET,name,params);
            G_ASSERT_OPENGL
        }
    };

    /// Checks whether the given \a id describes a valid frame buffer.
    static bool isValid(GLuint const id) {
        return glIsFramebufferEXT(id)!=GL_FALSE;
    }

    /// Creates an empty frame buffer with no image buffers attached.
    FrameBuffer() {
        if (GLEX_EXT_framebuffer_object!=GL_TRUE) {
            // Initialize an OpenGL extension to be able to replace the
            // system-provided frame buffer with a custom set of image buffers,
            // see <http://opengl.org/registry/specs/ARB/wgl_pixel_format.txt>.
            G_ASSERT_CALL(GLEX_EXT_framebuffer_object_init())
        }

        glGenFramebuffersEXT(1,&m_id);
        G_ASSERT_OPENGL
    }

    /// Destroys this frame buffer, freeing its ID again. Attached image buffers
    /// are not destroyed.
    ~FrameBuffer() {
        glDeleteFramebuffersEXT(1,&m_id);
        G_ASSERT_OPENGL
    }

    /// Sets the current binding to the frame buffer described by \a id or to
    /// the this frame buffer if the ID is negative.
    void setCurrentBinding(GLint const id=-1) const {
        glBindFramebufferEXT(TARGET,id<0?m_id:id);
        G_ASSERT_OPENGL
    }

    /// Attaches the given render buffer \a image to the logical buffer
    /// specified by \a attachment.
    void attach(RenderBuffer const& image,GLenum const attachment) const {
        bind();
        glFramebufferRenderbufferEXT(TARGET,attachment,image.TARGET,image.getID());
        G_ASSERT_OPENGL
    }

    /// Attaches the given 1D texture \a image to the logical buffer specified
    /// by \a attachment.
    void attach(Texture1D const& image,GLenum const attachment,GLint const level=0) const {
        bind();
        glFramebufferTexture1DEXT(TARGET,attachment,image.TARGET,image.getID(),level);
        G_ASSERT_OPENGL
    }

    /// Attaches the given 2D texture \a image to the logical buffer specified
    /// by \a attachment.
    void attach(Texture2D const& image,GLenum const attachment,GLint const level=0) const {
        bind();
        glFramebufferTexture2DEXT(TARGET,attachment,image.TARGET,image.getID(),level);
        G_ASSERT_OPENGL
    }

    /// Attaches the given 3D texture \a image to the logical buffer specified
    /// by \a attachment.
    void attach(Texture3D const& image,GLenum const attachment,GLint const zoffset,GLint const level=0) const {
        bind();
        glFramebufferTexture3DEXT(TARGET,attachment,image.TARGET,image.getID(),level,zoffset);
        G_ASSERT_OPENGL
    }

    /// Returns the value(s) of the parameter with the given \a name for the
    /// specified \a attachment.
    void getParameter(GLenum const attachment,GLenum const name,GLint* const params) const {
        bind();
        glGetFramebufferAttachmentParameterivEXT(TARGET,attachment,name,params);
        G_ASSERT_OPENGL
    }

    /// Returns this frame buffer's validity status.
    GLenum getStatus() {
        bind();
        return glCheckFramebufferStatusEXT(TARGET);
    }
};

} // namespace wrapgl

} // namespace gale

#endif // FRAMEBUFFER_H
