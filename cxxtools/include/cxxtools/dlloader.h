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

#ifndef CXXTOOLS_DLLOADER_H
#define CXXTOOLS_DLLOADER_H

#include <dlfcn.h>
#include <stdexcept>

namespace cxxtools
{

namespace dl
{
  class library;
  class symbol;

  /**
   base-class for errors.
   */
  class error : public std::runtime_error
  {
    public:
      error();
  };

  /**
   this exception is thrown, when a library can't be dlopen'd.
   */
  class dlopen_error : public error
  {
      std::string libname;
    public:
      dlopen_error(const std::string& l);
      ~dlopen_error() throw()
      { }

      /// returns the libname, which was tried.
      const std::string& getLibname() const  { return libname; }
  };

  /**
   this exception is thrown, when a symbol is not found it a library.
   */
  class symbol_not_found : public error
  {
      std::string symbol;

    public:
      symbol_not_found(const std::string& s);
      ~symbol_not_found() throw()
      { }

      /// returns the symbol, which was not found.
      const std::string& getSymbol() const  { return symbol; }
  };

  /**
   library represents a shared-library.

   To load a shared-library, just instantiate this class with a
   shared-library-name. The class is copyable. The library is unloaded
   (with dlclose) when the last reference is deleted.
   */
  class library
  {
      void* handle;
      mutable const library* prev;
      mutable const library* next;

    public:
      /// loads a shared library.
      library(const char* name, int flag = RTLD_NOW | RTLD_GLOBAL)
        : handle(0), prev(this), next(this)
        { open(name, flag); }
      /// default constructor.
      library()
        : handle(0), prev(this), next(this)
        { }
      /// Copy-constrcutor - increments reference to the library.
      library(const library& src);
      /// unloads the library if some is referenced.
      ~library()
      { close(); }

      library& operator=(const library& src);

      /// loads a shared library.
      /// If the class references already another library the
      /// reference is decremented and unloaded, if the class was the
      /// last reference.
      void open(const char* name, int flag = RTLD_NOW);
      /// unloads the library if some is referenced.
      /// Does nothing, if no library was referenced.
      void close();

      /// extracts a symbol from the library.
      symbol sym(const char* name) const;

      /// extracts a symbol from the library.
      void* operator[] (const char* name) const;

      /// returns true, if this is the only reference.
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

  /// extracts a symbol from the library.
  inline void* library::operator[] (const char* name) const
    { return sym(name); }

} // namespace dl

} // namespace cxxtools

#endif // CXXTOOLS_DLLOADER_H

