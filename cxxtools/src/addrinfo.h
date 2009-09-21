/*
 * Copyright (C) 2005 Tommi Maekitalo
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

#ifndef CXXTOOLS_ADDRINFO_H
#define CXXTOOLS_ADDRINFO_H

#include <cxxtools/noncopyable.h>
#include <string>
#include <iterator>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

namespace cxxtools {

namespace net {

  class AddrInfo : private cxxtools::NonCopyable
  {
      struct addrinfo* ai;

    public:
      void init(const std::string& ipaddr, unsigned short port);
      void init(const std::string& ipaddr, unsigned short port,
                const addrinfo& hints);

      AddrInfo()
        : ai(0)
        { }
      AddrInfo(const std::string& ipaddr, unsigned short port)
        : ai(0)
        { init(ipaddr, port); }
      AddrInfo(const std::string& ipaddr, unsigned short port,
               const addrinfo& hints)
        : ai(0)
        { init(ipaddr, port, hints); }
      ~AddrInfo();

      class const_iterator : public std::iterator<std::forward_iterator_tag, addrinfo>
      {
          struct addrinfo* current;

        public:
          explicit const_iterator(struct addrinfo* ai = 0)
            : current(ai)
            { }
          bool operator== (const const_iterator& it) const
            { return current == it.current; }
          bool operator!= (const const_iterator& it) const
            { return current != it.current; }
          const_iterator& operator++ ()
            { current = current->ai_next; return *this; }
          const_iterator operator++ (int)
            {
              const_iterator ret(current);
              current = current->ai_next;
              return ret;
            }
          reference operator* () const
            { return *current; }
          pointer operator-> () const
            { return current; }
      };

      const_iterator begin() const  { return const_iterator(ai); }
      const_iterator end() const    { return const_iterator(); }
  };

} // namespace net

} // namespace cxxtools

#endif // CXXTOOLS_NET_H
