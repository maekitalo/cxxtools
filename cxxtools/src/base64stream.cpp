////////////////////////////////////////////////////////////////////////
// base64stream.cpp
//

#include "base64stream.h"
#include <map>

static char cv[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
typedef std::map<char, unsigned> vc_type;
static vc_type vc;

void base64stream_streambuf::end()
{
  char A = obuffer[0];
  char B = obuffer[1];
  char C = obuffer[2];
  switch(pptr() - pbase())
  {
    case 1:
      B = 0;
      C = 0;
      putchar(cv[A >> 2]);
      putchar(cv[((A << 4) | ((B >> 4) & 0xF)) & 0x3F]);
      putchar('=');
      putchar('=');
      break;

    case 2:
      C = 0;
      putchar(cv[A >> 2]);
      putchar(cv[((A << 4) | ((B >> 4) & 0xF)) & 0x3F]);
      putchar(cv[((B << 2) | ((C >> 6) & 0x3)) & 0x3F]);
      putchar('=');
      break;

    case 3:
      putchar(cv[A >> 2]);
      putchar(cv[((A << 4) | ((B >> 4) & 0xF)) & 0x3F]);
      putchar(cv[((B << 2) | ((C >> 6) & 0x3)) & 0x3F]);
      putchar(cv[( C                         ) & 0x3F]);
      break;
  }

  putchar('\n');
  putchar('=');
  putchar('=');
  putchar('=');
  putchar('=');
  putchar('\n');

  setp(obuffer, obuffer + 3);
}

std::streambuf::int_type base64stream_streambuf::overflow(std::streambuf::int_type ch)
{
  if (pptr() != pbase())
  {
    char A = obuffer[0];
    char B = obuffer[1];
    char C = obuffer[2];
    putchar(cv[(A >> 2) & 0x3F]);
    putchar(cv[((A << 4) | ((B >> 4) & 0xF)) & 0x3F]);
    putchar(cv[((B << 2) | ((C >> 6) & 0x3)) & 0x3F]);
    putchar(cv[( C                         ) & 0x3F]);
  }

  setp(obuffer, obuffer + 3);

  if (ch != traits_type::eof())
  {
    *pptr() = (char_type)ch;
    pbump(1);
  }

  return 0;
}

std::streambuf::int_type base64stream_streambuf::underflow()
{
  // input:
  // |....,....|....,....|....,....|
  //  <   A   > <   B   > <   C   >
  //  < c1  ><  c2 > <  c3 ><  c4 >
  char& A = decodebuf[0];
  char& B = decodebuf[1];
  char& C = decodebuf[2];

  int c1 = getval();
  if (c1 == -1)
    return traits_type::eof();
  A =  (char)(c1 << 2);

  int c2 = getval();
  if (c2 == -1)
    return traits_type::eof();
  A |= (char)(c2 >> 4);
  B = (char)(c2 << 4);

  int c3 = getval();
  if (c3 == -1)
  {
    setg(decodebuf, decodebuf, decodebuf + 1);
    return traits_type::eof();
  }
  B |= (char)(c3 >> 2);
  C = (char)(c3 << 6);

  int c4 = getval();
  if (c4 == -1)
  {
    setg(decodebuf, decodebuf, decodebuf + 2);
    return traits_type::eof();
  }
  C |= (char)c4;

  setg(decodebuf, decodebuf, decodebuf + 3);

  return traits_type::to_int_type(A);
}

std::streambuf::int_type base64stream_streambuf::sync()
{
  return 0;
}

void base64stream_streambuf::putchar(char ch)
{
  sinksource->sputc(ch);
  if (ch == '\n')
    count = 0;
  else if (++count >= 60)
  {
    sinksource->sputc('\n');
    count = 0;
  }
}

int base64stream_streambuf::getval()
{
  if (vc.empty())
  {
    for (unsigned u = 0; u < 64; ++u)
      vc[cv[u]] = u;
  }

  unsigned count = 0;
  while (true)
  {
    int ret = sinksource->sbumpc();
    if (ret == traits_type::eof())
      return -1;

    char ch = traits_type::to_char_type(ret);
    if (ch == '=')
    {
      if (++count >= 4)
        return -1;
    }
    else
    {
      vc_type::const_iterator it = vc.find(ch);
      if (it != vc.end())
        return it->second;
      else
        count = 0;
    }
  }
}
