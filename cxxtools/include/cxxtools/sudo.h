/* cxxtools/sudo.h
   Copyright (C) 2003 Tommi MÃ¤kitalo

This file is part of cxxtools.

Cxxtools is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Cxxtools is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with cxxtools; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#ifndef CXXTOOLS_SUDO_H
#define CXXTOOLS_SUDO_H

#include <sys/types.h>
#include <utility>

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

#endif // CXXTOOLS_SUDO_H
