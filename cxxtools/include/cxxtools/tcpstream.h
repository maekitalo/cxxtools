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

#ifndef CXXTOOLS_TCPSTREAM_H
#define CXXTOOLS_TCPSTREAM_H

#include <iostream>
#include <string>
#include <stdexcept>
#include <netinet/in.h>
#include <sys/socket.h>

namespace cxxtools
{

namespace tcp
{
  //////////////////////////////////////////////////////////////////////
  /**
   * tcp::Exception wird bei Fehlern ausgelöst.
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
   * tcp::Timeout wird bei Zeitüberschreitung ausgelöst.
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
   * Die Klasse wird geworfen, wenn keine Zeichen gelesen werden konnten.
   */
  class eof : public std::exception
  {
    public:
      /// erzeugt eine Instanz
      eof()  { }
      /// Liefert einfache Meldung zurück
      const char* what() const throw()
      { return "EOF"; }
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

  //////////////////////////////////////////////////////////////////////
  /**
   * Serversocket
   */
  class Server : public Socket
  {
      union {
        struct sockaddr sockaddr;
        struct sockaddr_in sockaddr_in;
      } servaddr;

    public:
      Server();

      /// erzeugt einen Serversocket und hört auf die angegebene Adresse
      Server(const std::string& ipaddr, unsigned short int port, int backlog = 5)
             throw (Exception);
      /// erzeugt einen Serversocket und hört auf die angegebene Adresse
      Server(const char* ipaddr, unsigned short int port, int backlog = 5) throw (Exception);

      void Listen(const char* ipaddr, unsigned short int port, int backlog = 5) throw (Exception);
      void Listen(const std::string& ipaddr, unsigned short int port, int backlog = 5) throw (Exception)
        { Listen(ipaddr.c_str(), port, backlog); }

      const struct sockaddr& getAddr() const
        { return servaddr.sockaddr; }
      const struct sockaddr_in& getAddr_in() const
        { return servaddr.sockaddr_in; }

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
      union {
        struct sockaddr sockaddr;
        struct sockaddr_in sockaddr_in;
      } peeraddr;

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
      Stream(const char* ipaddr, unsigned short int port);
      /// Intantiiert ein Stream mit einem exisitierenden Filedeskriptor.
      /// Die Klasse übernimmt den Besitz des Deskriptors und schliesst diesen
      /// im Destruktor, wenn er >= 0 ist.
      explicit Stream(int fd)
        : Socket(fd)
        { }

      /// akzeptiert eine Verbindung von einem Server. Kommt keine
      /// Verbindung zustande, wird eine Exception ausgelöst.
      void Accept(const Server& server);
      /// nimmt eine Verbindung mit der angegebenen Adresse auf. Kommt keine
      /// Verbindung zustande, wird eine Exception ausgelöst.
      void Connect(const char* ipaddr, unsigned short int port);
      /// nimmt eine Verbindung mit der angegebenen Adresse auf. Kommt keine
      /// Verbindung zustande, wird eine Exception ausgelöst.
      void Connect(const std::string& ipaddr, unsigned short int port)
      { Connect(ipaddr.c_str(), port); }

      /// Liest vom Socket maximal 'bufsize' Zeichen in 'buffer'.
      /// Liefert die Anzahl der gelesenen Zeichen zurück.
      /// Bei Fehler wird eine tcp::Exception geworfen.
      size_type Read(char* buffer, size_type bufsize) const;
      /// Schreibt bis zu 'bufsize' Zeichen aus 'buffer' auf den Socket.
      /// Liefert die Anzahl der geschriebenen Zeichen zurück.
      size_type Write(const char* buffer, size_type bufsize) const;

      /// gibt die aktuelle Peer-Adresse zurück
      const struct sockaddr& getPeeraddr() const
        { return peeraddr.sockaddr; }
      const struct sockaddr_in& getPeeraddr_in() const
        { return peeraddr.sockaddr_in; }
  };

  //////////////////////////////////////////////////////////////////////
  /**
   * streambuf ist ein std::streambuf für Socket-Verbindungen.
   *
   * Die Klasse wird von tcp::iostream verwendet, um die Pufferung
   * zu realisieren.
   */
  class streambuf : public std::streambuf
  {
#if __GNUC__ <= 2
    typedef int int_type;
    typedef char char_type;

    class traits_type
    {
      public:
        static int eof()
        { return EOF; }
    };

#endif

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

} // namespace tcp

} // namespace cxxtools

#endif // CXXTOOLS_TCPSTREAM_H
