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

class DynBuffer
{
    // noncopyable:
    DynBuffer(const DynBuffer&);
    DynBuffer& operator= (const DynBuffer&);

  public:
    DynBuffer()
      : m_data(0),
        m_size(0),
        m_pos(0)
      { }
    ~DynBuffer()
    { delete[] m_data; }

    char*    data() const     { return m_data; }
    unsigned size() const     { return m_pos; }
    unsigned capacity() const { return m_size; }
    void     reserve(unsigned size)
    { if (m_size < size) reserve_grow(size - m_pos); }

    void     append(char ch)
    { if (m_pos < m_size) m_data[m_pos++] = ch; else append_grow(ch); }
    void     append(unsigned n, char ch);
    void     append(const char* data, unsigned size);
    void     assign(const char* data, unsigned size);
    void     assign(unsigned n, char ch);
    void     clear()        { m_pos = 0; }
    bool     empty() const  { return m_pos == 0; }

  private:
    void append_grow(char ch);
    void reserve_grow(unsigned size);

    char*    m_data;
    unsigned m_size;
    unsigned m_pos;
    static const unsigned m_initial_size = 512;
};
#endif // DYNBUFFER_H
