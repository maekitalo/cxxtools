/* cxxtools/dlloader.h
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
along with cxxtools; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#ifndef DLLOADER_H
#define DLLOADER_H

#include <dlfcn.h>
#include <stdexcept>

namespace dl
{
  class library;
  class symbol;

  class error : public std::runtime_error
  {
    public:
      error();
  };

  class dlopen_error : public error
  {
      std::string libname;
    public:
      dlopen_error(const std::string& l);
      ~dlopen_error() throw()
      { }

      const std::string& getLibname() const  { return libname; }
  };

  class symbol_not_found : public error
  {
      std::string symbol;

    public:
      symbol_not_found(const std::string& s);
      ~symbol_not_found() throw()
      { }

      const std::string& getSymbol() const  { return symbol; }
  };

  class library
  {
      void* handle;
      mutable const library* prev;
      mutable const library* next;

    public:
      library(const char* name, int flag = RTLD_NOW | RTLD_GLOBAL)
        : handle(0), prev(this), next(this)
        { open(name, flag); }
      library()
        : handle(0), prev(this), next(this)
        { }
      library(const library& src);
      ~library()
      { close(); }

      library& operator=(const library& src);

      void open(const char* name, int flag = RTLD_NOW);
      void close();

      symbol sym(const char* name) const;

      bool isLastRef() const
      { return next == this; }
  };

  class symbol
  {
      library lib;
      void* sym;

    public:
      symbol()
        : sym(0)
        { }
      symbol(const library& _lib, void* _sym)
        : lib(_lib), sym(_sym)
        { }

      void* getSym() const  { return sym; }
      const library& getLib() const  { return lib; }

      operator void* () const  { return sym; }
  };

}; // namespace dl

#endif // DLLOADER_H

