/*
 * Copyright (C) 2017 Tommi Maekitalo
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

#ifndef CXXTOOLS_HEXDUMP_H
#define CXXTOOLS_HEXDUMP_H

#include <iostream>
#include <cxxtools/hdstream.h>

namespace cxxtools
{

/// @Internal
struct HexDump
{
  const char* _p;
  unsigned _n;

  HexDump(const char* p, unsigned n)
    : _p(p),
      _n(n)
      { }
};

/// @Internal
inline HexDump hexDump(const char* p, unsigned n)
{ return HexDump(p, n); }

/** Dumps a memory area with a pointer and size.
 *
 *  Example:
 *  \code
 *    const char* buffer = ...;
 *    unsigned bufsize = ...
 *    std::cout << cxxtools::hexDump(buffer, bufsize);
 *  \endcode
 */
inline std::ostream& operator<< (std::ostream& out, const HexDump& hd)
{
  std::ostream::sentry sentry(out);
  Hdstreambuf buf(out.rdbuf());
  if (buf.sputn(hd._p, hd._n) == hd._n)
     buf.pubsync();
   else
     out.setstate(std::ios::failbit);
   return out;
}

/// @Internal
template <typename It>
struct HexDumpIt
{
    It begin;
    It end;
    HexDumpIt(It begin_, It end_)
        : begin(begin_),
          end(end_)
          { }
};

/// @Internal
template <typename It>
HexDumpIt<It> hexDump(It begin, It end)
{ return HexDumpIt<It>(begin, end); }

/** Dumps data specified by 2 iterators.
 *
 *  Example:
 *  \code
 *    std::vector<char> data = ...;
 *    std::cout << cxxtools::hexDump(data.begin(), data.end());
 *  \endcode
 */
template <typename It>
std::ostream& operator<< (std::ostream& out, const HexDumpIt<It>& hd)
{
  Hdstreambuf buf(out.rdbuf());
  for (It it = hd.begin; it != hd.end; ++it)
      buf.sputc(*it);
  buf.pubsync();
  return out;
}

/// @Internal
template <typename Container>
struct HexDumpC
{
    const Container& c;
    explicit HexDumpC(const Container& c_)
        : c(c_)
          { }
};

/// @Internal
template <typename Container>
HexDumpC<Container> hexDump(const Container& c)
{ return HexDumpC<Container>(c); }

/** Dumps data from a container class.
 *
 *  Example:
 *  \code
 *    std::vector<char> data = ...;
 *    std::cout << cxxtools::hexDump(data);
 *  \endcode
 */
template <typename Container>
std::ostream& operator<< (std::ostream& out, const HexDumpC<Container>& hd)
{
  Hdstreambuf buf(out.rdbuf());
  for (typename Container::const_iterator it = hd.c.begin(); it != hd.c.end(); ++it)
      buf.sputc(*it);
  buf.pubsync();
  return out;
}

}

#endif  // CXXTOOLS_HEXDUMP_H
