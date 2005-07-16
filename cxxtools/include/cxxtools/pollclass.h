/* cxxtools/pollclass.h
   Copyright (C) 2003 Tommi Maekitalo

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

#ifndef CXXTOOLS_POLLCLASS_H
#define CXXTOOLS_POLLCLASS_H

#include <map>
#include <poll.h>

namespace cxxtools
{

////////////////////////////////////////////////////////////////////////
// Wrapper for poll(2)
//
class Poller
{
  public:
    ////////////////////////////////////////////////////////////////////
    // PollClient-Interface
    //
    class ClientIf
    {
      public:
        virtual short GetEventMask() const = 0;
        virtual void  PollEvent(short events) = 0;
    };

    Poller()
      : m_fdsptr(0),
        m_fdsptr_size(0)
      { }

    virtual ~Poller()
    {
      delete[] m_fdsptr;
    }

    int Poll(int timeout = -1);

    void add(ClientIf* pc, int fd);
    void forget(ClientIf* pc, int fd);

  private:
    // noncopyable:
    Poller(const Poller&);
    Poller& operator= (const Poller&);

    typedef std::map<int, ClientIf*> map_type;
    map_type m_clientmap;

    struct pollfd*      m_fdsptr;
    map_type::size_type m_fdsptr_size;
};

////////////////////////////////////////////////////////////////////////
// PollClient-Implementation
//
class PollClient : public Poller::ClientIf
{
  public:
    PollClient(Poller& p, int fd, short events);
    virtual ~PollClient();

    short GetEventMask() const;

    void setEventMask(short events)
    { m_events = events; }

  private:
    Poller& m_poller;
    int     m_fd;
    short   m_events;
};

}

#endif // CXXTOOLS_POLLCLASS_H
