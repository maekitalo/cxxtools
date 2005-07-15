/* dlloader.cpp
   Copyright (C) 2003 Tommi Mäkitalo

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
#include "config.h"

#ifdef USE_LIBTOOL

#include <ltdl.h>

#define DLERROR()             lt_dlerror()
#define DLINIT()              lt_dlinit()
#define DLOPEN(name)          lt_dlopenext(name)
#define DLCLOSE(handle)       lt_dlclose(static_cast<lt_dlhandle>(handle))
#define DLEXIT()              lt_dlexit()
#define DLSYM(handle, name)   lt_dlsym(static_cast<lt_dlhandle>(handle), const_cast<char*>(name))

#else

// no LIBTOOL

#include <dlfcn.h>

#define DLERROR()             dlerror()
#define DLINIT()
#define DLOPEN(name)          cxx_dlopen(name)
#define DLCLOSE(handle)       dlclose(handle)
#define DLEXIT()
#define DLSYM(handle, name)   dlsym(handle, const_cast<char*>(name))

static void* cxx_dlopen(const char* name)
{
  std::string n = name;
  n.append(".so");
  return dlopen(n.c_str(), RTLD_NOW|RTLD_GLOBAL);
}

#endif

namespace cxxtools
{

namespace dl
{
  error::error()
    : std::runtime_error(DLERROR())
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
    if (handle == src.handle)
      return *this;

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

  void library::open(const char* name)
  {
    close();

    DLINIT();

    handle = DLOPEN(name);
    if (!handle)
    {
      DLEXIT();
      throw dlopen_error(name);
    }
  }

  void library::close()
  {
    if (handle)
    {
      if (prev == this)
      {
        DLCLOSE(handle);
        DLEXIT();
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
    void* sym = DLSYM(handle, name);
    if (sym == 0)
      throw symbol_not_found(name);

    return symbol(*this, sym);
  }

} // namespace dl

} // namespace cxxtools
