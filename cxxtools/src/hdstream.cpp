////////////////////////////////////////////////////////////////////////
// hdstream.cpp
//
// Copyright (c) 1997, 2003
// Dr. Eckhardt & Partner GmbH
// Kronberger Strasse 2
// 65812 Bad Soden
// 
/////////////////////////////////////////////////////////////////////////////
//
// $Workfile:$
// $Header$
// $Nokeywords:$
// 
/////////////////////////////////////////////////////////////////////////////

#include "hdstream.h"
#include <iomanip>

int hdstreambuf::overflow(int ch)
{
  using namespace std;

  ostream out(Dest);
  out.fill('0');
  size_t count = pptr() - pbase();
  if(count > 0)
  {
    out << setw(7) << hex << offset << '|';
    offset += count;
    size_t i;
    for(i = 0; i < count; ++i)
      out << setw(2) << hex
        << (unsigned int)(unsigned char)pbase()[i] << (i == 7 ? ':' : ' ');
    for( ; i < BUFFERSIZE; ++i)
      out << "   ";
    out << '|';
    for(i = 0; i < count; ++i)
      out << (char)(isprint(pbase()[i]) ? pbase()[i] : '.');
    for( ; i < BUFFERSIZE; ++i)
      out << ' ';
    out << endl;
  }

  setp(pbase(), epptr());
  if (ch != -1)
    sputc(ch);
  return 0;
}

int hdstreambuf::underflow()
{
  return EOF;
}

int hdstreambuf::sync()
{
  return overflow(EOF);
}
