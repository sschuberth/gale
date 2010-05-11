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

#include "gale/wrapgl/framebufferobject.h"

namespace gale {

namespace wrapgl {

void RenderBufferObject::initFallback() {
    if (!(GLEX_ARB_framebuffer_object || GLEX_ARB_framebuffer_object_init())) {
        if (GLEX_EXT_framebuffer_object || GLEX_EXT_framebuffer_object_init()) {
            glIsRenderbuffer                      = glIsRenderbufferEXT;
            glBindRenderbuffer                    = glBindRenderbufferEXT;
            glDeleteRenderbuffers                 = glDeleteRenderbuffersEXT;
            glGenRenderbuffers                    = glGenRenderbuffersEXT;
            glRenderbufferStorage                 = glRenderbufferStorageEXT;
            glGetRenderbufferParameteriv          = glGetRenderbufferParameterivEXT;
            glIsFramebuffer                       = glIsFramebufferEXT;
            glBindFramebuffer                     = glBindFramebufferEXT;
            glDeleteFramebuffers                  = glDeleteFramebuffersEXT;
            glGenFramebuffers                     = glGenFramebuffersEXT;
            glCheckFramebufferStatus              = glCheckFramebufferStatusEXT;
            glFramebufferTexture1D                = glFramebufferTexture1DEXT;
            glFramebufferTexture2D                = glFramebufferTexture2DEXT;
            glFramebufferTexture3D                = glFramebufferTexture3DEXT;
            glFramebufferRenderbuffer             = glFramebufferRenderbufferEXT;
            glGetFramebufferAttachmentParameteriv = glGetFramebufferAttachmentParameterivEXT;
            glGenerateMipmap                      = glGenerateMipmapEXT;
        }
        if (GLEX_EXT_framebuffer_blit || GLEX_EXT_framebuffer_blit_init()) {
            glBlitFramebuffer                     = glBlitFramebufferEXT;
        }
        if (GLEX_EXT_framebuffer_multisample || GLEX_EXT_framebuffer_multisample_init()) {
            glRenderbufferStorageMultisample      = glRenderbufferStorageMultisampleEXT;
        }
    }
}

} // namespace wrapgl

} // namespace gale
