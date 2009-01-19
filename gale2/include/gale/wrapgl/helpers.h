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

#ifndef HELPERS_H
#define HELPERS_H

/**
 * \file
 * Collection of miscellaneous OpenGL helper functions
 */

namespace gale {

namespace wrapgl {

/// Saves the current modelview and projection matrices and sets an orthogonal
/// projection that maps each raster position exactly to a screen space pixel.
void pushOrtho2D();

/// Restores the projection and modelview matrices, which were e.g. previously
/// saved by pushOrtho2D(), from the matrix stack.
void popOrtho2D();

/// Draws a logo into the lower left viewport corner, similar to a TV logo.
void drawLogo();

} // namespace wrapgl

} // namespace gale

#endif // HELPERS_H
