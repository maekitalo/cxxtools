////////////////////////////////////////////////////////////////////////
// hdstream.h
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

#ifndef HDSTREAM_H
#define HDSTREAM_H

#include <iostream>

class hdstreambuf : public std::streambuf
{
  public:
    hdstreambuf(std::streambuf* dest)
      : Dest(dest),
        offset(0)
    {
      setp(Buffer, Buffer + BUFFERSIZE);
    }

  private:
    static const unsigned BUFFERSIZE = 16;
    
    int overflow(int ch);
    int underflow();
    int sync();

    char Buffer[BUFFERSIZE];
    std::streambuf* Dest;
    unsigned offset;
};

class hdostream : public std::ostream
{
    typedef std::ostream base_class;

  public:
    hdostream()
      : base_class(new hdstreambuf(std::cout.rdbuf()))
    { }
    hdostream(std::ostream& out)
      : base_class(new hdstreambuf(out.rdbuf()))
    { }

    ~hdostream()
    { delete rdbuf(); }
};

#endif  // HDSTREAM_H
