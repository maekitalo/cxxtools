////////////////////////////////////////////////////////////////////////
// dynbuffer.h
//
// Author: Tommi Mäkitalo, Dr. Eckhardt + Partner GmbH
// Date:   22.5.2002
//
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
