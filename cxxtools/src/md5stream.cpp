/* md5stream.cpp
   Copyright (C) 2003 Tommi MÃ¤kitalo

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

void md5streambuf::getDigest(unsigned char digest_[16])
{
  if (pptr())
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

  std::memcpy(digest_, digest, 16);
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
