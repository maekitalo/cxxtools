/* cxxtools/dynbuffer.h
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
    typedef T& reference;
    typedef const T& const_reference;
    typedef T* iterator;
    typedef const T* const_iterator;
    typedef unsigned size_type;

  private:
    // noncopyable:
    dynbuffer(const dynbuffer&);
    dynbuffer& operator= (const dynbuffer&);

  public:
    /// initializes empty buffer
    dynbuffer()
      : m_data(0),
        m_size(0)
      { }

    /// allocates array of 'size' elements
    explicit dynbuffer(unsigned size)
      : m_data(0),
        m_size(0)
      { reserve(size); }

    ~dynbuffer()
    { delete[] m_data; }

    /// returns pointer to the buffer
    T* data() const            { return m_data; }
    /// returns size of buffer
    size_type size() const     { return m_size; }
    /// ensures, that buffer has at least 'size' elements
    void reserve(size_type size, bool keep_data = true)
    {
      if (size > 0 && m_size < size)
      {
        T* data = new T[size];
        if (m_data && keep_data)
          std::copy(m_data, m_data + size, data);
        delete[] m_data;
        m_data = data;
        m_size = size;
      }
    }

    /// delete buffer
    void clear()
    {
      delete[] m_data;
      m_data = 0;
      m_size = 0;
    }

    /// returns a iterator to first element
    iterator begin()               { return m_data; }
    /// returns a iterator after last element
    iterator end()                 { return m_data + m_size; }
    /// returns a iterator to first element
    const_iterator begin() const   { return m_data; }
    /// returns a iterator after last element
    const_iterator end() const     { return m_data + m_size; }

    reference operator[] (size_type n)              { return m_data[n]; }
    const_reference operator[] (size_type n) const  { return m_data[n]; }

    void spap(dynbuffer<T>& d)
    {
      std::swap(m_data, d.m_data);
      std::swap(m_size, d.m_size);
      std::swap(m_size, d.m_size);
    }

  private:
    T*    m_data;
    unsigned m_size;
};

typedef dynbuffer<char> DynBuffer;

}

#endif // CXXTOOLS_DYNBUFFER_H
