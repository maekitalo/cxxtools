/*
 * Copyright (C) 2006 by Marc Boris Duerner, Tommi Maekitalo
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

#ifndef CXXTOOLS_NET_TcpServer_H
#define CXXTOOLS_NET_TcpServer_H

#include <cxxtools/selectable.h>
#include <cxxtools/signal.h>
#include <cxxtools/ioerror.h>
#include <string>
#include <exception>

namespace cxxtools
{

namespace net
{

  class AddressInUse : public IOError
  {
    public:
      AddressInUse()
        : IOError("address in use")
        { }

      AddressInUse(const std::string& ipaddr, unsigned short int port);
  };

  class AcceptTerminated : public std::exception
  {
      public:
          const char* what() const throw ()
          { return "accept terminated"; }
  };

  class TcpServer : public Selectable
  {
    class TcpServerImpl* _impl;

    public:
      enum { INHERIT = 1, DEFER_ACCEPT = 2 };

      TcpServer();

      /** @brief Creates a server socket and listens on an address
      */
      TcpServer(const std::string& ipaddr, unsigned short int port, int backlog = 5, unsigned flags = 0);

      ~TcpServer();

      void listen(const std::string& ipaddr, unsigned short int port, int backlog = 5, unsigned flags = 0);

      // inherit doc
      virtual SelectableImpl& simpl();

      /** @brief Stops a blocking accept
          The stopAccept method can be called from a thread to break a
          blocking accept call in another thread. The call to accept
          is terminated by a AcceptTerminated exception.
       */
      void terminateAccept();

      TcpServerImpl& impl() const;

      Signal<TcpServer&> connectionPending;

    protected:
      // inherit doc
      virtual void onClose();

      // inherit doc
      virtual bool onWait(Timespan timeout);

      // inherit doc
      virtual void onAttach(SelectorBase&);

      // inherit doc
      virtual void onDetach(SelectorBase&);
  };

} // namespace net

} // namespace cxxtools

#endif // CXXTOOLS_NET_TcpServer_H
