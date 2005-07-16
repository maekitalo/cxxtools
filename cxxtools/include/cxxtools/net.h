/* cxxtools/net.h
   Copyright (C) 2005 Tommi Maekitalo

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

#ifndef CXXTOOLS_NET_H
#define CXXTOOLS_NET_H

#include <iostream>
#include <string>
#include <stdexcept>
#include <netinet/in.h>
#include <sys/socket.h>

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

      /// wrapper um ::getsocketname
      struct sockaddr getSockAddr() const throw (Exception);

      /// timeout in milliseconds
      void setTimeout(int t);
      int getTimeout() const  { return m_timeout; }

    protected:
      void setFd(int sockFd)
      {
        close();
        m_sockFd = sockFd;
      }

    private:
      int m_sockFd;
      int m_timeout;

      Socket(const Socket&);               // no implementation
      Socket& operator= (const Socket&);   // no implementation
  };

} // namespace net

} // namespace cxxtools

#endif // CXXTOOLS_NET_H
