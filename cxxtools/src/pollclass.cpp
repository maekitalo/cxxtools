/* pollclass.cpp
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2003 Tommi Maekitalo
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

#include "cxxtools/pollclass.h"
#include <stdexcept>
#include <iostream>

namespace cxxtools
{

////////////////////////////////////////////////////////////////////////
// Wrapper für poll-Systemfunktion
//
int Poller::Poll(int timeout)
{
  map_type::size_type size = m_clientmap.size();

  // baue pollfd-Struktur
  if (m_fdsptr_size < size)
  {
    delete[] m_fdsptr;
    m_fdsptr      = new pollfd[size];
    m_fdsptr_size = size;
  }

  int i = 0;
  for (map_type::iterator it = m_clientmap.begin();
       it != m_clientmap.end();
       ++it)
  {
    m_fdsptr[i].fd      = (*it).first;
    m_fdsptr[i].events  = (*it).second->GetEventMask();
    m_fdsptr[i].revents = 0;
    ++i;
  }

  // Aufruf der Systemfunktion
  int ret = ::poll(m_fdsptr, size, timeout);

  if (ret > 0)
  {
    // Delegation der Events
    for (map_type::size_type i = 0; i < size; ++i)
    {
      if (m_fdsptr[i].revents != 0)
      {
        map_type::iterator it = m_clientmap.find(m_fdsptr[i].fd);

        // Kann eigentlich nicht passieren, da die m_fdsptr aus der m_clientmap
        // aufgebaut wird:
        if (it == m_clientmap.end())
          throw std::runtime_error("Pollclient not found");

        (*it).second->PollEvent(m_fdsptr[i].revents);
      }
    }
  }
  else if (ret < 0)
  {
    throw std::runtime_error("error in poll");
  }

  return ret;
}

void Poller::add(ClientIf* pc, int fd)
{
  if (m_clientmap.find(fd) != m_clientmap.end())
    throw std::runtime_error("Poller::add: fd already listening");

  m_clientmap[fd] = pc;
}

void Poller::forget(ClientIf* pc, int fd)
{
  map_type::iterator it = m_clientmap.find(fd);

  if (it == m_clientmap.end()
    || (*it).second != pc)
    throw std::runtime_error("Poller::forget: client not found");

  m_clientmap.erase(it);
}

////////////////////////////////////////////////////////////////////////
// PollClient-Interface
//
PollClient::PollClient(Poller& p, int fd, short events)
  : m_poller(p)
{
  m_fd = fd;
  m_events = events;
  m_poller.add(this, fd);
}

PollClient::~PollClient()
{
  try
  {
    m_poller.forget(this, m_fd);
  }
  catch (const std::runtime_error& e)
  {
    // forget könnte sagen, daß der PollClient nicht gefunden wurde.
    // Sollte eigentlich nicht passieren, aber wenn doch:
    std::cerr << e.what() << std::endl;
  }
}

short PollClient::GetEventMask() const
{
  return m_events;
}

}
