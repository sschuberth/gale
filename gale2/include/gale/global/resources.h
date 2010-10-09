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

#ifndef RESOURCES_H
#define RESOURCES_H

/**
 * \file
 * Various resources
 */

#include "platform.h"

/// Returns an icon handle to the image referred to by \a index in the memory
/// pointed to by \a data.
inline HICON CreateIconFromPointer(LPBYTE data,WORD index=0)
{
#ifndef GALE_TINY_CODE
    // Check ICONDIR structure entries, see
    // <http://msdn.microsoft.com/en-us/library/ms997538.aspx>.
    LPWORD icondir=LPWORD(data);
    if (!icondir || icondir[0]!=0 || icondir[1]!=1 || icondir[2]<=index) {
        return NULL;
    }
#endif

    // Skip the ICONDIR and preceding ICONDIRENTRY structures.
    LPDWORD icondirentry=LPDWORD(data+6+index*16);

    // Add the image offset to the raw data pointer.
    data+=icondirentry[3];

    return CreateIconFromResource(data,icondirentry[2],TRUE,0x00030000);
}

/// The raw Three Kings icon data.
extern unsigned char threekings_ico[];

#endif // RESOURCES_H
