////////////////////////////////////////////////////////////////////////
// dlloader.h
//

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
      library(const char* name, int flag = RTLD_NOW)
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

