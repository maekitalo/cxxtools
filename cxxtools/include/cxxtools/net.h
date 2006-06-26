/* cxxtools/net.h
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2005 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
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

#ifndef CXXTOOLS_NET_H
#define CXXTOOLS_NET_H

#include <string>
#include <stdexcept>
#include <iterator>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

namespace cxxtools
{

namespace net
{
  //////////////////////////////////////////////////////////////////////
  /**
   * net::Exception wird bei Fehlern ausgelöst.
   */
  class Exception : public std::runtime_error
  {
    public:
      /// Konstruktor erzeugt eine Instanz mit einer Fehlernummer
      /// und einer Fehlermeldung.
      Exception(int Errno, const std::string& msg);
      /// Konstruktor erzeugt eine Instanz mit einer Fehlermeldung.
      /// Die Nummer wird aus errno übernommen.
      explicit Exception(const std::string& msg);

      /// Liefert die Fehlernummer zurück.
      int getErrno() const
      { return m_Errno; }

    private:
      int m_Errno;
  };

  //////////////////////////////////////////////////////////////////////
  /**
   * net::Timeout wird bei Zeitüberschreitung ausgelöst.
   */
  class Timeout : public Exception
  {
    public:
      /// Konstruktor erzeugt eine Instanz mit einer Fehlermeldung.
      Timeout()
        : Exception(0, "Timeout")
      { }
  };

  //////////////////////////////////////////////////////////////////////
  /**
   * Basisklasse für Sockets
   */
  class Socket
  {

    public:
      /// Konstruktor erzeugt eine Instanz und erzeugt einen Socket
      /// auf Betriebssystemebene. Bei Fehlern wird eine Exception
      /// ausgelöst.
      Socket(int domain, int type, int protocol) throw (Exception);

      /// Konstruktor initialisiert die Klasse mit einem bestehenden
      /// Socket.
      explicit Socket(int fd = -1)
        : m_sockFd(fd),
          m_timeout(-1)
        { }

      /// Im Destruktor wird der Socket geschlossen, wenn er offen ist.
      /// Bei einem Fehler wird eine Fehlermeldung auf stderr geschrieben.
      virtual ~Socket();

      /// Liefert true zurück, wenn der Socket geöffnet ist
      bool good() const     { return m_sockFd >= 0; }
      /// Liefert true zurück, wenn der Socket nicht geöffnet ist
      bool bad() const      { return m_sockFd <  0; }
      /// Liefert true zurück, wenn der Socket geöffnet ist
      operator bool() const { return m_sockFd >= 0; }

      /// Erzeugt einen Socket auf Betriebssystemebene. Ist bereits einer
      /// geöffnet, wird dieser vorher geschlossen. Bei Fehlern wird eine
      /// Exception ausgelöst.
      void create(int domain, int type, int protocol) throw (Exception);
      /// Schießt den Socket, wenn er geöffnet ist.
      void close();

      /// Liefert den Sockethandle zurück
      int getFd() const     { return m_sockFd; }

      /// wrapper um ::getsockname
      struct sockaddr_storage getSockAddr() const throw (Exception);

      /// timeout in milliseconds
      void setTimeout(int t);
      int getTimeout() const  { return m_timeout; }

    protected:
      void setFd(int sockFd);

      /// execute poll(2) - throws Timeout-exception, when no data available
      /// after timeout
      short doPoll(short events) const;

    private:
      int m_sockFd;
      int m_timeout;

      Socket(const Socket&);               // no implementation
      Socket& operator= (const Socket&);   // no implementation
  };

  class Addrinfo
  {
      struct addrinfo* ai;
      void init(const std::string& ipaddr, unsigned short port,
                const addrinfo& hints);

    public:
      Addrinfo(const std::string& ipaddr, unsigned short port);
      Addrinfo(const std::string& ipaddr, unsigned short port,
               const addrinfo& hints)
        : ai(0)
        { init(ipaddr, port, hints); }
      ~Addrinfo();

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
