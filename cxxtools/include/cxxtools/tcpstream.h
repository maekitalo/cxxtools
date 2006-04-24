/* cxxtools/tcpstream.h
   Copyright (C) 2003-2005 Tommi Maekitalo

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

#ifndef CXXTOOLS_NET_TCPSTREAM_H
#define CXXTOOLS_NET_TCPSTREAM_H

#include <iostream>
#include <string>
#include <stdexcept>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cxxtools/net.h>

namespace cxxtools
{

namespace net
{
  //////////////////////////////////////////////////////////////////////
  /**
   * Serversocket
   */
  class Server : public Socket
  {
      struct sockaddr_storage servaddr;

    public:
      Server();

      /// erzeugt einen Serversocket und hört auf die angegebene Adresse
      Server(const std::string& ipaddr, unsigned short int port, int backlog = 5)
             throw (Exception);

      void listen(const std::string& ipaddr, unsigned short int port, int backlog = 5) throw (Exception);

      const struct sockaddr_storage& getAddr() const
        { return servaddr; }

    private:
  };

  //////////////////////////////////////////////////////////////////////
  /**
   * Stream repräsentiert ein Client-Socket.
   *
   * Stream ist ein ungepufferter Client-Socket zum lesen und schreiben
   * von Daten über den Socket.
   */
  class Stream : public Socket
  {
      struct sockaddr_storage peeraddr;

    public:
      typedef size_t size_type;

      /// initialisert einen leeren Socket
      Stream();
      /// nimmt eine Verbindung von einem Serversocket an.
      explicit Stream(const Server& server);
      /// Initialisert einen Socket und nimmt Verbindung mit der angegebenen
      /// Adresse auf. Kommt keine Verbindung zustande, wird eine Exception
      /// ausgelöst
      Stream(const std::string& ipaddr, unsigned short int port);
      /// Initialisert einen Socket und nimmt Verbindung mit der angegebenen
      /// Adresse auf. Kommt keine Verbindung zustande, wird eine Exception
      /// ausgelöst
      explicit Stream(int fd)
        : Socket(fd)
        { }

      /// akzeptiert eine Verbindung von einem Server. Kommt keine
      /// Verbindung zustande, wird eine Exception ausgelöst.
      void accept(const Server& server);
      /// nimmt eine Verbindung mit der angegebenen Adresse auf. Kommt keine
      /// Verbindung zustande, wird eine Exception ausgelöst.
      void connect(const std::string& ipaddr, unsigned short int port);

      /// Liest vom Socket maximal 'bufsize' Zeichen in 'buffer'.
      /// Liefert die Anzahl der gelesenen Zeichen zurück.
      /// Bei Fehler wird eine net::Exception geworfen.
      size_type read(char* buffer, size_type bufsize) const;
      /// Schreibt bis zu 'bufsize' Zeichen aus 'buffer' auf den Socket.
      /// Liefert die Anzahl der geschriebenen Zeichen zurück.
      size_type write(const char* buffer, size_type bufsize) const;

      /// gibt die aktuelle Peer-Adresse zurück
      const struct sockaddr_storage& getPeeraddr() const
        { return peeraddr; }
  };

  //////////////////////////////////////////////////////////////////////
  /**
   * streambuf ist ein std::streambuf für Socket-Verbindungen.
   *
   * Die Klasse wird von net::iostream verwendet, um die Pufferung
   * zu realisieren.
   */
  class streambuf : public std::streambuf
  {
    public:
      /// Konstuktor reserviert Speicher für den E/A-Puffer
      explicit streambuf(Stream& stream, unsigned bufsize = 256,
        int timeout = -1);

      /// Destruktor gibt Speicher frei
      ~streambuf()
      { delete[] m_buffer; }

      void setTimeout(int t)   { m_stream.setTimeout(t); }
      int getTimeout() const   { return m_stream.getTimeout(); }

      /// überladen aus std::streambuf
      int_type overflow(int_type c);
      /// überladen aus std::streambuf
      int_type underflow();
      /// überladen aus std::streambuf
      int sync();

    private:
      Stream&    m_stream;
      unsigned   m_bufsize;
      char_type* m_buffer;
  };

  //////////////////////////////////////////////////////////////////////
  /**
   * Stream-Socket erweitert um Pufferung über std::iostream-Bibliothek
   */
  class iostream : public std::iostream, public Stream
  {
    public:
      /// Erzeugt einen nicht verbundenen Stream-Socket.
      explicit iostream(unsigned bufsize = 256, int timeout = -1)
        : std::iostream(&m_buffer),
          m_buffer(*this, bufsize, timeout)
        { }

      /// Nimmt eine eingehende Verbindung vom angegebenen Server an.
      explicit iostream(const Server& server, unsigned bufsize = 256, int timeout = -1)
        : std::iostream(&m_buffer),
          Stream(server),
          m_buffer(*this, bufsize, timeout)
        { }

      /// Baut eine Verbindung zum angegebenen Server auf.
      iostream(const char* ipaddr, unsigned short int port, unsigned bufsize = 256)
        : std::iostream(&m_buffer),
          Stream(ipaddr, port),
          m_buffer(*this, bufsize)
        { }

      /// Baut eine Verbindung zum angegebenen Server auf.
      iostream(std::string ipaddr, unsigned short int port, unsigned bufsize = 256)
        : std::iostream(&m_buffer),
          Stream(ipaddr, port),
          m_buffer(*this, bufsize)
        { }

      void setTimeout(int timeout)  { m_buffer.setTimeout(timeout); }
      int getTimeout() const        { return m_buffer.getTimeout(); }

    private:
      streambuf m_buffer;
  };

} // namespace net

} // namespace cxxtools

#endif // CXXTOOLS_NET_TCPSTREAM_H
