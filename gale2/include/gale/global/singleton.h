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

#ifndef SINGLETON_H
#define SINGLETON_H

/**
 * \file
 * A "Singleton" design pattern implementation
 */

namespace gale {

namespace global {

/**
 * Singleton design pattern implementation as described in "More Effective
 * C++" by Scott Meyers (Addison-Wesley, 1996a), item 26. You need to derive
 * from this class and pass the derived class as template argument \c I, see
 * the CPUInfo class for an example usage.
 */
template<class I>
class Singleton
{
  public:

    /// Returns a reference to the one and only instance of this class.
    static I& the() {
        static I instance;
        return instance;
    }

  protected:

    /// Prevent the creation of a new singleton instance.
    Singleton() {}

    /// Prevent the destruction, e.g. via pointer deletion.
    ~Singleton() {}

  private:

    /// Disable automatic copy constructor generation by the compiler.
    Singleton(Singleton const&);

    /// Disable assignments (which always are self-assignments for singletons).
    Singleton& operator=(Singleton const&);
};

} // namespace global

} // namespace gale

#endif // SINGLETON_H
