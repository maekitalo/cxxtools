/* dynbuffer.cpp
   Copyright (C) 2003 Tommi Mäkitalo

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
along with Cxxtools; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#include "cxxtools/dynbuffer.h"
#include <algorithm>

void DynBuffer::reserve_grow(unsigned size)
{
  unsigned nsize = m_size > 0 ? m_size : m_initial_size;
  while (nsize < m_pos + size) nsize <<= 1;
  char* data = new char[nsize];
  std::copy(m_data, m_data + m_pos, data);
  delete[] m_data;
  m_data = data;
  m_size = nsize;
}

void DynBuffer::append_grow(char ch)
{
  reserve_grow(1);
  m_data[m_pos++] = ch;
}

void DynBuffer::append(unsigned n, char ch)
{
  reserve(m_pos + n);
  std::fill(m_data + m_pos, m_data + m_pos + n, ch);
  m_pos += n;
}

void DynBuffer::assign(unsigned n, char ch)
{
  reserve(n);
  std::fill(m_data, m_data + n, ch);
  m_pos = n;
}

void DynBuffer::append(const char* data, unsigned size)
{
  reserve(m_pos + size);

  std::copy(data, data + size, m_data + m_pos);
  m_pos += size;
}

void DynBuffer::assign(const char* data, unsigned size)
{
  reserve(size);
  std::copy(data, data + size, m_data);
  m_pos = size;
}
