/* dlloader.cpp
   Copyright (C) 2003 Tommi M√§kitalo

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
along with Cxxtools; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#include "cxxtools/dlloader.h"

namespace dl
{
  error::error()
    : std::runtime_error(dlerror())
  { }

  dlopen_error::dlopen_error(const std::string& l)
    : error(),
      libname(l)
  { }

  symbol_not_found::symbol_not_found(const std::string& s)
    : error(),
      symbol(s)
  { }

  library::library(const library& src)
    : handle(src.handle),
      prev(&src),
      next(src.next)
  {
    src.next = this;
    next->prev = this;
  }

  library& library::operator=(const library& src)
  {
    close();

    handle = src.handle;

    if (handle)
    {
      prev = const_cast<library*>(&src);
      next = src.next;
      const_cast<library&>(src).next = this;
      next->prev = this;
    }

    return *this;
  }

  void library::open(const char* name, int flag)
  {
    close();

    handle = dlopen(name, flag);
    if (!handle)
      throw dlopen_error(name);
  }

  void library::close()
  {
    if (handle)
    {
      if (prev == this)
      {
        dlclose(handle);
      }
      else
      {
        prev->next = next;
        next->prev = prev;
      }
      handle = 0;
      next = prev = this;
    }
  }

  symbol library::sym(const char* name) const
  {
    // Warum nimmt dlsym ein "char*" statt "const char*"?
    // Ich gehe einfach mal davon aus, daﬂ er nichts reinschreibt.
    void* sym = dlsym(handle, const_cast<char*>(name));
    if (sym == 0)
      throw symbol_not_found(name);

    return symbol(*this, sym);
  }

}; // namespace dl
