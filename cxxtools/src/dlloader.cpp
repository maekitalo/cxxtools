/* dlloader.cpp
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2003 Tommi Maekitalo
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "cxxtools/dlloader.h"
#include "cxxtools/log.h"
#include "config.h"
#include <cxxtools/thread.h>

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

log_define("cxxtools.dlloader")

static cxxtools::Mutex mutex;

namespace cxxtools
{

namespace dl
{
  namespace
  {
    std::string errorString()
    {
      const char* msg = DLERROR();
      return msg ? std::string(msg) : "unknown error in dlloader";
    }
  }

  Error::Error()
    : std::runtime_error(errorString())
  { }

  Error::Error(const std::string& msg)
    : std::runtime_error(msg + ": " + errorString())
  { }

  DlopenError::DlopenError(const std::string& l)
    : Error("library \"" + l + '"'),
      libname(l)
  { }

  SymbolNotFound::SymbolNotFound(const std::string& s)
    : Error("symbol \"" + s + '"'),
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

  Library::~Library()
  {
    try
    {
      close();
    }
    catch (...)
    { }
  }

  Library& Library::operator=(const Library& src)
  {
    if (handle == src.handle)
      return *this;

    close();

    cxxtools::MutexLock lock(mutex);
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

    cxxtools::MutexLock lock(mutex);
    log_debug("dlinit");
    DLINIT();

    log_debug("dlopen(\"" << name << "\")");
    handle = DLOPEN(name);
    if (!handle)
    {
      log_debug("dlopen(\"" << name << "\") failed");
      DLEXIT();
      throw DlopenError(name);
    }

    log_debug("dlopen => " << handle);
  }

  void Library::close()
  {
    cxxtools::MutexLock lock(mutex);
    if (handle)
    {
      if (prev == this)
      {
        log_debug("dlclose " << handle);
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
    log_debug("dlsym(" << handle << ", \"" << name << "\")");
    void* sym = DLSYM(handle, name);
    if (sym == 0)
    {
      log_debug("dlsym: symbol \"" << name << "\" not found");
      throw SymbolNotFound(name);
    }

    log_debug("dlsym => " << sym);
    return Symbol(*this, sym);
  }

} // namespace dl

} // namespace cxxtools
