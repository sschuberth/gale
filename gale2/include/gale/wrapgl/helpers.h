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
 * Miscellaneous helper routines
 */

#include "camera.h"
#include "renderer.h"

namespace gale {

namespace wrapgl {

/**
 * This class provides a collection of OpenGL helper functions.
 */
class Helper
{
  public:

    /// Saves the current modelview / projection matrices and sets an orthogonal
    /// projection that maps each raster position exactly to a viewport pixel.
    static void pushOrtho2D();

    /// Saves the current modelview / projection matrices and sets an orthogonal
    /// projection that maps the lower left viewport pixel to the coordinate
    /// \a ll_x / \a ll_y, and the upper right viewport pixel to the coordinate
    /// \a ur_x / \a ur_y.
    static void pushOrtho2D(GLdouble ll_x,GLdouble ll_y,GLdouble ur_x,GLdouble ur_y);

    /// Restores the projection / modelview matrices which were previously saved
    /// by pushOrtho2D().
    static void popOrtho2D();
};

/**
 * A class to draw a logo into the current viewport.
 */
class Logo
{
  public:

    /// Identifiers for the corner to draw the logo into.
    enum Corner {
        CORNER_LL ///< Lower Left
    ,   CORNER_LR ///< Lower Right
    ,   CORNER_UR ///< Upper Right
    ,   CORNER_UL ///< Upper Left
    };

    /// Prepares to draw a logo with the given camera \a fov and \a distance.
    Logo(double const fov=90*math::Constd::DEG_TO_RAD(),float const distance=3.0f);

    /// Frees the logo's render resources.
    ~Logo();

    /// Draws a logo into the specified viewport \a corner, similar to a TV
    /// overlay logo.
    void draw(Corner corner=CORNER_LL);

  private:

    /// Identifiers for the display lists.
    enum List {
        LIST_PROLOGUE ///< Prologue display list (use before rendering).
    ,   LIST_EPILOGUE ///< Epilogue display list (use after rendering).
    ,   LIST_FRAME    ///< Display list to draw the logo's frame.
    ,   LIST_COUNT    ///< Special entry to name the number of enum entries.
    };

    Camera m_camera;          ///< The logo's camera.

    GLuint m_list_range;      ///< Start number of the range of display list IDs.
    PreparedMesh m_cube_prep; ///< The cube mesh prepared for rendering.
};

} // namespace wrapgl

} // namespace gale
