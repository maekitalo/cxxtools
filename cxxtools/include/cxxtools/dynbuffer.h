/* cxxtools/dynbuffer.h
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
along with cxxtools; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#ifndef DYNBUFFER_H
#define DYNBUFFER_H

#include <algorithm>

template <typename T = char>
class dynbuffer
{
  public:
    typedef T* iterator;
    typedef const T* const_iterator;

  private:
    // noncopyable:
    dynbuffer(const dynbuffer&);
    dynbuffer& operator= (const dynbuffer&);

  public:
    dynbuffer()
      : m_data(0),
        m_size(0),
        m_pos(0)
      { }

    explicit dynbuffer(unsigned size)
      : m_data(0),
        m_size(0),
        m_pos(0)
      { reserve(size); }

    ~dynbuffer()
    { delete[] m_data; }

    T*       data() const     { return m_data; }
    unsigned size() const     { return m_pos; }
    unsigned capacity() const { return m_size; }
    void     reserve(unsigned size)
    { if (m_size < size) reserve_grow(size - m_pos); }

    void     append(T ch)
    { if (m_pos < m_size) m_data[m_pos++] = ch; else append_grow(ch); }
    void     append(unsigned n, T ch);
    void     append(const T* data, unsigned size);
    void     assign(const T* data, unsigned size);
    void     assign(unsigned n, T ch);
    void     clear()        { m_pos = 0; }
    bool     empty() const  { return m_pos == 0; }

    iterator begin()               { return m_data; }
    iterator end()                 { return m_data + m_size; }
    const_iterator begin() const   { return m_data; }
    const_iterator end() const     { return m_data + m_size; }

  private:
    void append_grow(T ch);
    void reserve_grow(unsigned size);

    T*    m_data;
    unsigned m_size;
    unsigned m_pos;
    static const unsigned m_initial_size = 512;
};

template <typename T>
void dynbuffer<T>::reserve_grow(unsigned size)
{
  unsigned nsize = m_size > 0 ? m_size : m_initial_size;
  while (nsize < m_pos + size)
    nsize <<= 1;
  T* data = new T[nsize];
  std::copy(m_data, m_data + m_pos, data);
  delete[] m_data;
  m_data = data;
  m_size = nsize;
}

template <typename T>
void dynbuffer<T>::append_grow(T ch)
{
  reserve_grow(1);
  m_data[m_pos++] = ch;
}

template <typename T>
void dynbuffer<T>::append(unsigned n, T ch)
{
  reserve(m_pos + n);
  std::fill(m_data + m_pos, m_data + m_pos + n, ch);
  m_pos += n;
}

template <typename T>
void dynbuffer<T>::assign(unsigned n, T ch)
{
  reserve(n);
  std::fill(m_data, m_data + n, ch);
  m_pos = n;
}

template <typename T>
void dynbuffer<T>::append(const T* data, unsigned size)
{
  reserve(m_pos + size);

  std::copy(data, data + size, m_data + m_pos);
  m_pos += size;
}

template <typename T>
void dynbuffer<T>::assign(const T* data, unsigned size)
{
  reserve(size);
  std::copy(data, data + size, m_data);
  m_pos = size;
}

typedef dynbuffer<char> DynBuffer;

#endif // DYNBUFFER_H
