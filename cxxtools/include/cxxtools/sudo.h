/* cxxtools/sudo.h
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2003 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef CXXTOOLS_SUDO_H
#define CXXTOOLS_SUDO_H

#include <sys/types.h>
#include <utility>
#include <stdlib.h>

namespace cxxtools
{

namespace sudo
{
  void set_user(uid_t uid);
  void set_user(const char* user);

  //////////////////////////////////////////////////////////////////////
  // sudo::fork_wait
  //
  // führt einen ::fork aus.
  // Im Child-Prozess wird im ersten Rückgabewert (first) 'false'
  // geliefert.
  // Der Parent-Prozess wartet auf den Child-Prozess und kehrt erst zurück,
  // wenn der Child-Prozess beendet ist. Der erste Rückgabewert ist 'true'.
  // Der zweite Rückgabewert (second) liefert den Status des
  // Childprozesses.
  //
  // Im Fehlerfall wird eine std::runtime_error geworfen.
  //
  std::pair<bool, int> fork_wait();

  //////////////////////////////////////////////////////////////////////
  // sudo führt die übergebene Funktion, bzw. den Funktor unter der
  // angegebenen Benutzerberechtigung aus.
  // Der Benutzer wird über die Funktion set_user eingestellt und kann
  // so entweder als const char* oder uid_t übergeben werden.
  // Im Fehlerfall wird eine std::runtime_error geworfen.
  //
  // Der Funktor liefert den Rückgabewert.
  //
  template <typename user_type, typename generator_type>
  int sudo(user_type user, generator_type f)
  {
    std::pair<bool, int> ret = fork_wait();
    if (!ret.first)
    {
      // child
      try
      {
        set_user(user);
        exit( f() );
      }
      catch (...)
      {
        exit(-1);
      }
    }

    // parent
    return ret.second;
  }

}

}

#endif // CXXTOOLS_SUDO_H
