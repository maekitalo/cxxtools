/***************************************************************************
 *   Copyright (C) 2006-2008 Marc Boris Duerner                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef CXXTOOLS_SYSTEM_SHAREDLIB_H
#define CXXTOOLS_SYSTEM_SHAREDLIB_H

#include <cxxtools/systemerror.h>
#include <string>

namespace cxxtools {

class Symbol;

/** @brief Thrown, when a shared library could not be loaded
*/
class OpenLibraryFailed : public SystemError
{
    public:
        //! @brief Contructs from a message string and source info
        OpenLibraryFailed(const std::string& msg, const cxxtools::SourceInfo& si);

        //! @brief Destructor
        ~OpenLibraryFailed() throw()
		{}
};

/** @brief Thrown, when a symbol is not found in a library
*/
class SymbolNotFound : public SystemError
{
    std::string _symbol;

    public:
        SymbolNotFound(const std::string& sym, const cxxtools::SourceInfo& si);

        //! @brief Destructor
        ~SymbolNotFound() throw()
        {}

        //! @brief Returns the symbol, which was not found
        const std::string& symbol() const
        { return _symbol; }
};

/** @brief Shared library loader

    This class can be used to dynamically load shared libraries and
    resolve symbols from it. The example below shows how to retrieve
    the address of the function 'myProcedure' in library 'MySharedLib':

    @code
        SharedLib shlib("MySharedLib.dll");
        void* procAddr = shlib["myProcedure"];

        typedef int (*MyProcType)();
        MyProcType proc =(MyProcType)procAddr;
        int result = proc();
    @endcode
*/
class SharedLib
{
    public:
        /** @brief Default Constructor which does not load a library.
         */
        SharedLib();

        /** @brief Loads a shared library.

             If a file could not be found at the given path, the path will be extended
             by the platform-specific shared library extension first and then also by the
             shared library prefix. If still no file can be found an exception is thrown.

             The library is loaded immediately.
        */
        SharedLib(const std::string& path);

        SharedLib(const SharedLib& other);

        SharedLib& operator=(const SharedLib& other);

        /** @brief The destructor unloads the shared library from memory.
         */
        ~SharedLib();

        /** @brief Loads a shared library.

             If a file could not be found at the given path, the path will be extended
             by the platform-specific shared library extension first and then also by the
             shared library prefix. If still no file can be found an exception is thrown.
             Calling this method twice might close the previously loaded library.
        */
        SharedLib& open(const std::string& path);

        /** @brief Resolves the symbol \a symbol from the shared library
            Returns the address of the symbol or 0 if it was not found.
         */
        void* operator[](const char* symbol)
		{ return this->resolve(symbol); }

        /** @brief Resolves the symbol \a symbol from the shared library
            Returns the address of the symbol or 0 if it was not found.
         */
        void* resolve(const char* symbol);

        /** @brief Returns null if invalid
         */
        operator const void*() const;

        Symbol getSymbol(const char* symbol);

        /** @brief Returns true if invalid
         */
        bool operator!() const;

        /** @brief Returns the path to the shared library image
         */
        const std::string& path() const;

        static std::string find(const std::string& path);

        /** @brief Returns the extension for shared libraries

            Returns ".so" on Linux, ".dll" on Windows.
        */
        static std::string suffix();

        /**  @brief Returns the prefix for shared libraries

             Returns "lib" on Linux, "" on Windows 
        */
        static std::string prefix();

    protected:
        void detach();

    private:
        //! @internal
        class SharedLibImpl* _impl;

        //! @internal
        std::string _path;
};

/** @brief Symbol resolved from a shared library
*/
class Symbol
{
    public:
        Symbol()
        : _sym(0)
        { }

        Symbol(const SharedLib& lib, void* sym)
        : _lib(lib), _sym(sym)
        { }

        void* sym() const
        { return _sym; }

        const SharedLib& library() const
        { return _lib; }

        operator void*() const
        { return _sym; }

    private:
        SharedLib _lib;
        void* _sym;
};

} // namespace cxxtools

#endif
