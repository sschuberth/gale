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

#ifndef FRAMEBUFFEROBJECTEXT_H
#define FRAMEBUFFEROBJECTEXT_H

/**
 * \file
 * Defines to map ARB_framebuffer_object to EXT_framebuffer_{object,blit,multisample}
 */

/// \cond DOXYGEN_IGNORE

/*
 * EXT_framebuffer_object
 */

#define GL_FRAMEBUFFER                                  GL_FRAMEBUFFER_EXT
#define GL_RENDERBUFFER                                 GL_RENDERBUFFER_EXT
#define GL_STENCIL_INDEX1                               GL_STENCIL_INDEX1_EXT
#define GL_STENCIL_INDEX4                               GL_STENCIL_INDEX4_EXT
#define GL_STENCIL_INDEX8                               GL_STENCIL_INDEX8_EXT
#define GL_STENCIL_INDEX16                              GL_STENCIL_INDEX16_EXT
#define GL_RENDERBUFFER_WIDTH                           GL_RENDERBUFFER_WIDTH_EXT
#define GL_RENDERBUFFER_HEIGHT                          GL_RENDERBUFFER_HEIGHT_EXT
#define GL_RENDERBUFFER_INTERNAL_FORMAT                 GL_RENDERBUFFER_INTERNAL_FORMAT_EXT
#define GL_RENDERBUFFER_RED_SIZE                        GL_RENDERBUFFER_RED_SIZE_EXT
#define GL_RENDERBUFFER_GREEN_SIZE                      GL_RENDERBUFFER_GREEN_SIZE_EXT
#define GL_RENDERBUFFER_BLUE_SIZE                       GL_RENDERBUFFER_BLUE_SIZE_EXT
#define GL_RENDERBUFFER_ALPHA_SIZE                      GL_RENDERBUFFER_ALPHA_SIZE_EXT
#define GL_RENDERBUFFER_DEPTH_SIZE                      GL_RENDERBUFFER_DEPTH_SIZE_EXT
#define GL_RENDERBUFFER_STENCIL_SIZE                    GL_RENDERBUFFER_STENCIL_SIZE_EXT
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE           GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME           GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL         GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_EXT
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_EXT
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET    GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT
#define GL_COLOR_ATTACHMENT0                            GL_COLOR_ATTACHMENT0_EXT
#define GL_COLOR_ATTACHMENT1                            GL_COLOR_ATTACHMENT1_EXT
#define GL_COLOR_ATTACHMENT2                            GL_COLOR_ATTACHMENT2_EXT
#define GL_COLOR_ATTACHMENT3                            GL_COLOR_ATTACHMENT3_EXT
#define GL_COLOR_ATTACHMENT4                            GL_COLOR_ATTACHMENT4_EXT
#define GL_COLOR_ATTACHMENT5                            GL_COLOR_ATTACHMENT5_EXT
#define GL_COLOR_ATTACHMENT6                            GL_COLOR_ATTACHMENT6_EXT
#define GL_COLOR_ATTACHMENT7                            GL_COLOR_ATTACHMENT7_EXT
#define GL_COLOR_ATTACHMENT8                            GL_COLOR_ATTACHMENT8_EXT
#define GL_COLOR_ATTACHMENT9                            GL_COLOR_ATTACHMENT9_EXT
#define GL_COLOR_ATTACHMENT10                           GL_COLOR_ATTACHMENT10_EXT
#define GL_COLOR_ATTACHMENT11                           GL_COLOR_ATTACHMENT11_EXT
#define GL_COLOR_ATTACHMENT12                           GL_COLOR_ATTACHMENT12_EXT
#define GL_COLOR_ATTACHMENT13                           GL_COLOR_ATTACHMENT13_EXT
#define GL_COLOR_ATTACHMENT14                           GL_COLOR_ATTACHMENT14_EXT
#define GL_COLOR_ATTACHMENT15                           GL_COLOR_ATTACHMENT15_EXT
#define GL_DEPTH_ATTACHMENT                             GL_DEPTH_ATTACHMENT_EXT
#define GL_STENCIL_ATTACHMENT                           GL_STENCIL_ATTACHMENT_EXT
#define GL_FRAMEBUFFER_COMPLETE                         GL_FRAMEBUFFER_COMPLETE_EXT
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT            GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT    GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS            GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT
#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS               GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER           GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER           GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT
#define GL_FRAMEBUFFER_UNSUPPORTED                      GL_FRAMEBUFFER_UNSUPPORTED_EXT
#define GL_FRAMEBUFFER_BINDING                          GL_FRAMEBUFFER_BINDING_EXT
#define GL_RENDERBUFFER_BINDING                         GL_RENDERBUFFER_BINDING_EXT
#define GL_MAX_COLOR_ATTACHMENTS                        GL_MAX_COLOR_ATTACHMENTS_EXT
#define GL_MAX_RENDERBUFFER_SIZE                        GL_MAX_RENDERBUFFER_SIZE_EXT
#define GL_INVALID_FRAMEBUFFER_OPERATION                GL_INVALID_FRAMEBUFFER_OPERATION_EXT

#define glIsRenderbuffer                      glIsRenderbufferEXT
#define glBindRenderbuffer                    glBindRenderbufferEXT
#define glDeleteRenderbuffers                 glDeleteRenderbuffersEXT
#define glGenRenderbuffers                    glGenRenderbuffersEXT
#define glRenderbufferStorage                 glRenderbufferStorageEXT
#define glGetRenderbufferParameteriv          glGetRenderbufferParameterivEXT
#define glIsFramebuffer                       glIsFramebufferEXT
#define glBindFramebuffer                     glBindFramebufferEXT
#define glDeleteFramebuffers                  glDeleteFramebuffersEXT
#define glGenFramebuffers                     glGenFramebuffersEXT
#define glCheckFramebufferStatus              glCheckFramebufferStatusEXT
#define glFramebufferTexture1D                glFramebufferTexture1DEXT
#define glFramebufferTexture2D                glFramebufferTexture2DEXT
#define glFramebufferTexture3D                glFramebufferTexture3DEXT
#define glFramebufferRenderbuffer             glFramebufferRenderbufferEXT
#define glGetFramebufferAttachmentParameteriv glGetFramebufferAttachmentParameterivEXT
#define glGenerateMipmap                      glGenerateMipmapEXT

/*
 * EXT_framebuffer_blit
 */

#define GL_READ_FRAMEBUFFER         GL_READ_FRAMEBUFFER_EXT
#define GL_DRAW_FRAMEBUFFER         GL_DRAW_FRAMEBUFFER_EXT
#define GL_DRAW_FRAMEBUFFER_BINDING GL_DRAW_FRAMEBUFFER_BINDING_EXT
#define GL_READ_FRAMEBUFFER_BINDING GL_READ_FRAMEBUFFER_BINDING_EXT

#define glBlitFramebuffer glBlitFramebufferEXT

/*
 * EXT_framebuffer_multisample
 */

#define GL_RENDERBUFFER_SAMPLES               GL_RENDERBUFFER_SAMPLES_EXT
#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT
#define GL_MAX_SAMPLES                        GL_MAX_SAMPLES_EXT

#define glRenderbufferStorageMultisample glRenderbufferStorageMultisampleEXT

/// \endcond

#endif // FRAMEBUFFEROBJECTEXT_H
