////////////////////////////////////////////////////////////////////////
// dynbuffer.h
//
// Author: Tommi Mäkitalo, Dr. Eckhardt + Partner GmbH
// Date:   22.5.2002
//

#include <dynbuffer.h>
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
