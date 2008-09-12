/* cxxtools/dlloader.h
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

#ifndef CXXTOOLS_DLLOADER_H
#define CXXTOOLS_DLLOADER_H

#include <stdexcept>
#include <string>

namespace cxxtools
{

namespace dl
{
  class Library;
  class Symbol;

  /**
   base-class for errors.
   */
  class Error : public std::runtime_error
  {
    public:
      Error();
      Error(const std::string& msg);
  };

  /**
   this exception is thrown, when a Library can't be dlopen'd.
   */
  class DlopenError : public Error
  {
      std::string libname;
    public:
      DlopenError(const std::string& l);
      ~DlopenError() throw()
      { }

      /// returns the libname, which was tried.
      const std::string& getLibname() const  { return libname; }
  };

  /**
   this exception is thrown, when a symbol is not found it a library.
   */
  class SymbolNotFound : public Error
  {
      std::string symbol;

    public:
      SymbolNotFound(const std::string& s);
      ~SymbolNotFound() throw()
      { }

      /// returns the symbol, which was not found.
      const std::string& getSymbol() const  { return symbol; }
  };

  /**
   Library represents a shared-library.

   To load a shared-library, just instantiate this class with a
   shared-library-name. The class is copyable. The library is unloaded
   (with dlclose) when the last reference is deleted.
   */
  class Library
  {
      void* handle;
      mutable const Library* prev;
      mutable const Library* next;

    public:
      /// loads a shared library.
      Library(const char* name)
        : handle(0), prev(this), next(this)
        { open(name); }
      /// default constructor.
      Library()
        : handle(0), prev(this), next(this)
        { }
      /// Copy-constrcutor - increments reference to the library.
      Library(const Library& src);
      /// unloads the library if some is referenced.
      ~Library();

      Library& operator=(const Library& src);

      /// loads a shared library.
      /// If the class references already another library the
      /// reference is decremented and unloaded, if the class was the
      /// last reference.
      void open(const char* name);
      /// unloads the library if some is referenced.
      /// Does nothing, if no library was referenced.
      void close();

      /// extracts a symbol from the library.
      Symbol sym(const char* name) const;

      /// extracts a symbol from the library.
      void* operator[] (const char* name) const;

      /// returns true, if this is the only reference.
      bool isLastRef() const
      { return next == this; }

      void* getHandle() const   { return handle; }
  };

  class Symbol
  {
      Library lib;
      void* sym;

    public:
      Symbol()
        : sym(0)
        { }
      Symbol(const Library& _lib, void* _sym)
        : lib(_lib), sym(_sym)
        { }

      void* getSym() const  { return sym; }
      const Library& getLib() const  { return lib; }

      operator void* () const  { return sym; }
  };

  /// extracts a symbol from the library.
  inline void* Library::operator[] (const char* name) const
    { return sym(name); }

} // namespace dl

} // namespace cxxtools

#endif // CXXTOOLS_DLLOADER_H

