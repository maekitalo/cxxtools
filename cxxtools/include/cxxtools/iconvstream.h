////////////////////////////////////////////////////////////////////////
// iconvstream.h
//

#ifndef ICONVSTREAM_H
#define ICONVSTREAM_H

#include <iostream>
#include <iconv.h>

////////////////////////////////////////////////////////////////////////
/// std::streambuf-Interface für den iconv(3)-Systemaufruf.
/// iconv konvertiert Zeichensätze.
class iconvstreambuf : public std::streambuf
{
    std::ostream* sink;
    iconv_t cd;
    char buffer[16];

  public:
    iconvstreambuf()
      : sink(0),
        cd((iconv_t)-1)
      { }
    ~iconvstreambuf()
      { close(); }

    iconvstreambuf* open(std::ostream& sink_,
      const char* tocode, const char* fromcode);
    iconvstreambuf* close() throw();

    /// überladen aus std::streambuf
    int_type overflow(int_type c);
    /// überladen aus std::streambuf
    int_type underflow();
    /// überladen aus std::streambuf
    int sync();

};

////////////////////////////////////////////////////////////////////////
/// std::ostream-Interface für den iconv(3)-Systemaufruf.
/// iconv konvertiert Zeichensätze.
class iconvstream : public std::ostream
{
    iconvstreambuf streambuf;

  public:
    iconvstream(std::ostream& sink, const char* tocode, const char* fromcode)
      : std::ostream(&streambuf)
    {
      streambuf.open(sink, tocode, fromcode);
    }

    void open(std::ostream& sink_,
      const char* tocode, const char* fromcode);
    void close() throw()
      { streambuf.close(); }
};

#endif // ICONVSTREAM_H

