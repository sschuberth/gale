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
 * A 3D graphics API abstraction for rendering geometry
 */

#include "camera.h"
#include "preparedmesh.h"

namespace gale {

namespace wrapgl {

/**
 * A collection of static rendering methods for different geometries using
 * OpenGL as the 3D graphics API back-end.
 */
struct Renderer
{
    /// Renders the given prepared mesh \a prep.
    static void draw(PreparedMesh const& prep);

    /// Renders the given axis-aligned bounding \a box.
    static void draw(model::AABB const& box);

    /// Renders the view frustum of the given \a camera.
    static void draw(Camera const& camera);
};

} // namespace wrapgl

} // namespace gale
