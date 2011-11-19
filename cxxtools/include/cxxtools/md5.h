/*
 * Copyright (C) 2011 Tommi Maekitalo
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

#ifndef CXXTOOLS_MD5_H
#define CXXTOOLS_MD5_H

#include <cxxtools/md5stream.h>
#include <iterator>
#include <algorithm>

namespace cxxtools
{

template <typename iterator_type>
std::string md5(iterator_type from, iterator_type to)
{
  Md5stream s;
  std::copy(from, to, std::ostream_iterator<char>(s));
  return s.getHexDigest();
}

template <typename data_type>
std::string md5(const data_type& data)
{
  Md5stream s;
  s << data;
  return s.getHexDigest();
}

template <typename data_type> class md5_hash
{
  std::string digest;

public:
  explicit md5_hash(const data_type& data)
  {
    unsigned char _digest[16];
    Md5stream s;
    s << data;
    s.getDigest(_digest);
    digest.assign(reinterpret_cast<char*>(&_digest[0]), reinterpret_cast<char*>(&_digest[0] + 16));
  }

  md5_hash(typename data_type::const_iterator from, 
		   typename data_type::const_iterator to)
  {
    unsigned char _digest[16];
    Md5stream s;
    std::copy(from, to, std::ostream_iterator<char>(s));
    s.getDigest(_digest);
    digest.assign(reinterpret_cast<char*>(&_digest[0]), reinterpret_cast<char*>(&_digest[0] + 16));
  }

  static const unsigned short blockSize = 64;

  std::string getHexDigest() const
  {
    static const char hex[] = "0123456789abcdef";
    std::string ret;
    ret.reserve(32);
    for (unsigned n = 0; n < 16; ++n)
    {
      ret.push_back(hex[(digest[n] >> 4) & 0xf]);
      ret.push_back(hex[digest[n] & 0xf]);
    }
    return ret;
  }

  std::string getDigest() const
  { return digest; }
};

}

#endif  // CXXTOOLS_MD5_H
