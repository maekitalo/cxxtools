/////////////////////////////////////////////////////////////////////////////
// tee.h
//
// Copyright (c) 1997, 2003
// Dr. Eckhardt & Partner GmbH
// Kronberger Strasse 2
// 65812 Bad Soden
// 
/////////////////////////////////////////////////////////////////////////////
//
// $Workfile$
// $Header$
// $Nokeywords$
// 
/////////////////////////////////////////////////////////////////////////////

#ifndef TEE_H
#define TEE_H

#include <iostream>

class teestreambuf : public std::streambuf
{
  public:
    teestreambuf(std::streambuf* buf1 = 0, std::streambuf* buf2 = 0)
      : streambuf1(buf1),
        streambuf2(buf2)
    { setp(0, 0); }
    
    void tie(std::streambuf* buf1, std::streambuf* buf2 = 0)
    {
      streambuf1 = buf1;
      streambuf2 = buf2;
    }

  private:
    std::streambuf::int_type overflow(std::streambuf::int_type ch);
    std::streambuf::int_type underflow();
    std::streambuf::int_type sync();

    std::streambuf* streambuf1;
    std::streambuf* streambuf2;
};

/////////////////////////////////////////////////////////////////////////////

class tee : public std::ostream
{
    typedef std::ostream base_class;

  public:
    tee()
      : base_class(new teestreambuf(std::cout.rdbuf()))
    { }
    tee(std::ostream& s1, std::ostream& s2)
      : base_class(new teestreambuf(s1.rdbuf(), s2.rdbuf()))
    { }
    tee(std::ostream& s)
      : base_class(new teestreambuf(s.rdbuf(), std::cout.rdbuf()))
    { }
    ~tee()
    {
      delete rdbuf();
    }

    void assign(std::ostream& s1, std::ostream& s2);
    void assign(std::ostream& s)
    { assign(s, std::cout); }
    void assign_single(std::ostream& s);
};

#endif  // TEE_H
