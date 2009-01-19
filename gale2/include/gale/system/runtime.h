/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
 *
 * Copyright (C) 2005-2008  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
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

#ifndef RUNTIME_H
#define RUNTIME_H

/**
 * \file
 * Minimal replacements for C runtime functions
 */

#ifdef GALE_TINY

/// Minimal "new" operator implementation.
void* operator new(size_t bytes);

/// Minimal array "new" operator implementation.
void* operator new[](size_t bytes);

/// Minimal placement "new" operator implementation.
void* operator new(size_t bytes,void* place);

/// Minimal "delete" operator implementation.
void operator delete(void* pointer);

/// Minimal array "delete" operator implementation.
void operator delete[](void* pointer);

#endif // GALE_TINY

#endif // RUNTIME_H
