/* dlloader.cpp
   Copyright (C) 2003 Tommi Maekitalo

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
  return dlopen((std::string(name) + ".so").c_str(), RTLD_NOW|RTLD_GLOBAL);
}

#endif

namespace cxxtools
{

namespace dl
{
  Error::Error()
    : std::runtime_error(DLERROR())
  { }

  DlopenError::DlopenError(const std::string& l)
    : Error(),
      libname(l)
  { }

  SymbolNotFound::SymbolNotFound(const std::string& s)
    : Error(),
      symbol(s)
  { }

  Library::Library(const Library& src)
    : handle(src.handle),
      prev(&src),
      next(src.next)
  {
    src.next = this;
    next->prev = this;
  }

  Library& Library::operator=(const Library& src)
  {
    if (handle == src.handle)
      return *this;

    close();

    handle = src.handle;

    if (handle)
    {
      prev = const_cast<Library*>(&src);
      next = src.next;
      const_cast<Library&>(src).next = this;
      next->prev = this;
    }

    return *this;
  }

  void Library::open(const char* name)
  {
    close();

    DLINIT();

    handle = DLOPEN(name);
    if (!handle)
    {
      DLEXIT();
      throw DlopenError(name);
    }
  }

  void Library::close()
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

  Symbol Library::sym(const char* name) const
  {
    void* sym = DLSYM(handle, name);
    if (sym == 0)
      throw SymbolNotFound(name);

    return Symbol(*this, sym);
  }

} // namespace dl

} // namespace cxxtools
