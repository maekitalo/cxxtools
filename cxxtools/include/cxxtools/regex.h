/*
 * Copyright (C) 2005-2008 Tommi Maekitalo
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
 */


#ifndef CXXTOOLS_REGEX_H
#define CXXTOOLS_REGEX_H

#include <string>
#include <sys/types.h>
#include <regex.h>
#include <cxxtools/smartptr.h>

namespace cxxtools
{
  class RegexSMatch;

  template <typename objectType>
  class RegexDestroyPolicy;

  template <>
  class RegexDestroyPolicy<regex_t>
  {
    protected:
      void destroy(regex_t* expr)
      {
        ::regfree(expr);
        delete expr;
      }
  };

  /// regex(3)-wrapper.
  class Regex
  {
      SmartPtr<regex_t, ExternalRefCounted, RegexDestroyPolicy> expr;

      void checkerr(int ret) const;

    public:
      Regex()
      { }

      explicit Regex(const char* ex, int cflags = REG_EXTENDED)
        : expr(new regex_t())
      {
        checkerr(::regcomp(expr.getPointer(), ex, cflags));
      }

      explicit Regex(const std::string& ex, int cflags = REG_EXTENDED)
        : expr(new regex_t())
      {
        checkerr(::regcomp(expr.getPointer(), ex.c_str(), cflags));
      }

      bool match(const std::string& str_, RegexSMatch& smatch, int eflags = 0) const;
      bool match(const std::string& str_, int eflags = 0) const;

      void free()  { expr = 0; }
  };

  /// collects matches in a regex
  class RegexSMatch
  {
      friend class Regex;

      std::string str;
      regmatch_t matchbuf[10];

    public:
      /// returns the number of expressions, which were found
      unsigned size() const;
      /// returns the start position of the n-th expression
      regoff_t offsetBegin(unsigned n) const   { return matchbuf[n].rm_so; }
      /// returns the end position of the n-th expression
      regoff_t offsetEnd(unsigned n) const     { return matchbuf[n].rm_eo; }

      /// returns true if the n-th element is set.
      bool has(unsigned n) const
        { return matchbuf[n].rm_so >= 0; }
      /// returns the n-th element. No range checking is done.
      std::string get(unsigned n) const;
      /// replace each occurence of "$n" with the n-th element (n: 0..9).
      std::string format(const std::string& s) const;
      /// returns the n-th element. No range checking is done.
      std::string operator[] (unsigned n) const
        { return get(n); }
  };

}

#endif // CXXTOOLS_REGEX_H

