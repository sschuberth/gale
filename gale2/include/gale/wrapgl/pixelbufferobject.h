/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
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

#ifndef PIXELBUFFEROBJECT_H
#define PIXELBUFFEROBJECT_H

/**
 * \file
 * A wrapper for Pixel Buffer Objects
 */

#include "vertexbufferobject.h"

namespace gale {

namespace wrapgl {

#include "GLEX_ARB_pixel_buffer_object.h"

/**
 * \name Type definitions for the different buffer types, see
 * <http://www.opengl.org/registry/specs/ARB/pixel_buffer_object.txt>
 */
//@{

typedef VertexBufferObject<GL_PIXEL_PACK_BUFFER_ARB,GL_PIXEL_PACK_BUFFER_BINDING_ARB> PixelPackBufferObject;
typedef VertexBufferObject<GL_PIXEL_UNPACK_BUFFER_ARB,GL_PIXEL_UNPACK_BUFFER_BINDING_ARB> PixelUnpackBufferObject;

//@}

} // namespace wrapgl

} // namespace gale

#endif // PIXELBUFFEROBJECT_H
