////////////////////////////////////////////////////////////////////////
// md5stream
//

#include "cxxtools/md5stream.h"

////////////////////////////////////////////////////////////////////////
// md5streambuf
//
std::streambuf::int_type md5streambuf::overflow(
  std::streambuf::int_type ch)
{
  if (pptr() == 0)
  {
    // Ausgabepuffer ist leer - initialisieren
    MD5Init(&context);
  }
  else
  {
    // konsumiere Zeichen aus dem Puffer
    MD5Update(&context,
              (const unsigned char*)pbase(),
              pptr() - pbase());
  }

  // setze Ausgabepuffer
  setp(buffer, buffer + bufsize);

  if (ch != traits_type::eof())
  {
    // das Zeichen, welches den overflow ausgelöst hat, stecken
    // wir in den Puffer.
    *pptr() = (char_type)ch;
    pbump(1);
  }

  return 0;
}

std::streambuf::int_type md5streambuf::underflow()
{
  // nur Ausgabestrom
  return traits_type::eof();
}

std::streambuf::int_type md5streambuf::sync()
{
  if (pptr() != pbase())
  {
    // konsumiere Zeichen aus dem Puffer
    MD5Update(&context, (const unsigned char*)pbase(), pptr() - pbase());

    // leere Ausgabepuffer
    setp(buffer, buffer + bufsize);
  }

  return 0;
}

void md5streambuf::getDigest(unsigned char digest[16])
{
  if (pptr() != pbase())
  {
    // konsumiere Zeichen aus dem Puffer
    MD5Update(&context, (const unsigned char*)pbase(), pptr() - pbase());
  }

  // deinitialisiere Ausgabepuffer
  setp(0, 0);

  MD5Final(digest, &context);
}

////////////////////////////////////////////////////////////////////////
// md5stream
//
const char* md5stream::getHexDigest()
{
  static const char hex[] = "0123456789abcdef";
  unsigned char md5[16];
  getDigest(md5);
  int i;
  char* p = hexdigest;
  for (i = 0; i < 16; ++i)
  {
    *p++ = hex[md5[i] >> 4];
    *p++ = hex[md5[i] & 0xf];
  }
  *p = '\0';
  return hexdigest;
}
