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

#ifndef CXXTOOLS_DYNBUFFER_H
#define CXXTOOLS_DYNBUFFER_H

#include <algorithm>

namespace cxxtools
{

/**
 Exceptionsafe dynamic buffer.

 Sometimes programs need a buffer for temporaryly storing data. The
 size of the buffer might be unknown at compile-time, so the program
 can't use a array. This is where dynbuffer help.

 Allocation of storage is done on request and the destructor releases
 the memory.

 example:
 \code
   some_func()
   {
     unsigned size = get_bytes_to_read();
     dynbuffer<char> buffer(size);
     read(buffer.data(), size);

     do_something_with_data(buffer.data())

   } // destructor releases the buffer even on exception here

 \endcode
 */
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
    /// initializes empty buffer
    dynbuffer()
      : m_data(0),
        m_size(0),
        m_pos(0)
      { }

    /// allocates array of 'size' elements
    explicit dynbuffer(unsigned size)
      : m_data(0),
        m_size(0),
        m_pos(0)
      { reserve(size); }

    ~dynbuffer()
    { delete[] m_data; }

    /// returns pointer to the buffer
    T*       data() const     { return m_data; }
    /// returns size of buffer
    unsigned size() const     { return m_pos; }
    /// returns capacity of buffer.
    unsigned capacity() const { return m_size; }
    /// ensures, that buffer is at least 'size' elements
    void     reserve(unsigned size)
    { if (m_size < size) reserve_grow(size - m_pos); }

    /// append one element to the buffer.
    /// Allocates new memory if capacity is too small.
    void     append(T ch)
    { if (m_pos < m_size) m_data[m_pos++] = ch; else append_grow(ch); }
    /// appends mutliple copies of one element to the buffer.
    /// Allocates new memory if capacity is too small.
    void     append(unsigned n, T ch);
    /// appends mutliple elements to the buffer.
    /// Allocates new memory if capacity is too small.
    void     append(const T* data, unsigned size);
    /// replaces content
    void     assign(const T* data, unsigned size);
    /// replaces content with multiple copies of the element.
    void     assign(unsigned n, T ch);
    /// reduces the size of the buffer to 0
    /// doew not free any memory.
    void     clear()        { m_pos = 0; }
    /// returns true, if buffer has the size 0
    bool     empty() const  { return m_pos == 0; }

    /// returns a iterator to first element
    iterator begin()               { return m_data; }
    /// returns a iterator after last element
    iterator end()                 { return m_data + m_size; }
    /// returns a iterator to first element
    const_iterator begin() const   { return m_data; }
    /// returns a iterator after last element
    const_iterator end() const     { return m_data + m_size; }

    void spap(dynbuffer<T>& d)
    {
      std::swap(m_data, d.m_data);
      std::swap(m_size, d.m_size);
      std::swap(m_pos, d.m_pos);
    }

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

}

#endif // CXXTOOLS_DYNBUFFER_H
