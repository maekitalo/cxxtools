////////////////////////////////////////////////////////////////////////
// iconvstreamcpp
//

#include "iconvstream.h"
#include <string.h>

iconvstreambuf* iconvstreambuf::open(std::ostream& sink_,
  const char* tocode, const char* fromcode)
{
  sink = &sink_;
  cd = iconv_open(tocode, fromcode);

  if (cd == (iconv_t)-1)
    return 0;

  setp(buffer, buffer + (sizeof(buffer) - 1));
  return this;
}

iconvstreambuf* iconvstreambuf::close() throw()
{
  if (cd != (iconv_t)-1)
  {
    int r = iconv_close(cd);
    if (r == 0)
    {
      cd = 0;
      return this;
    }
    else
      return 0;
  }
  else
    return this;
}

iconvstreambuf::int_type iconvstreambuf::overflow(int_type c)
{
  if (sink == 0)
    return traits_type::eof();
  else if (pptr() == pbase())
  {
    // 1. Aufruf: initialisiere Puffer
    setp(buffer, buffer + (sizeof(buffer) - 1));
    if (c != traits_type::eof())
    {
      *pptr() = (char_type)c;
      pbump(1);
    }
    return 0;
  }
  else
  {
    // Ausgabepuffer voll - konvertiere Zeichen
    char outbuf[32];

    size_t inbytesleft = pptr() - buffer;
    if (c != traits_type::eof())
    {
      *pptr() = (char_type)c;
      ++inbytesleft;
    }
    size_t outbytesleft = sizeof(outbuf);

    char* inbufptr = buffer;
    char* outbufptr = outbuf;

    // Konvertiere so viele Zeichen wie möglich
    iconv(cd,
          &inbufptr, &inbytesleft,
          &outbufptr, &outbytesleft);

    if (errno != 0 && errno != EINVAL && errno != E2BIG)
    {
      // Konvertierung fehlgeschlagen
      return traits_type::eof();
    }
    errno = 0;

    // Konvertierung erfolgreich

    // Schreibe outbuf in sink
    sink->write(outbuf, outbufptr - outbuf);

    if (sink->fail())
      return traits_type::eof();

    // verschiebe übrig gebliebene Zeichen an den Anfang des Puffers
    // und setze Puffer neu.
    if (inbytesleft > 0)
      memmove(buffer, inbufptr, inbytesleft);

    setp(buffer + inbytesleft, buffer + sizeof(buffer) - 1);
    return 0;
  }
}

iconvstream::int_type iconvstreambuf::underflow()
{
  return traits_type::eof();
}

int iconvstreambuf::sync()
{
  if (sink)
  {
    int_type ret = overflow(traits_type::eof());
    sink->flush();
    return ret == traits_type::eof() || sink->fail() ? -1 : 0;
  }
  else
    return -1;
}

void iconvstream::open(std::ostream& sink_,
  const char* tocode, const char* fromcode)
{
  if (!fail() && streambuf.open(sink_, tocode, fromcode) == 0)
    setstate(std::ios::failbit);
}
