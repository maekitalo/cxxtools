/////////////////////////////////////////////////////////////////////////////
// tee.cpp
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

#include "cxxtools/tee.h"

std::streambuf::int_type teestreambuf::overflow(std::streambuf::int_type ch)
{
  if(ch != traits_type::eof())
  {
    if(streambuf1 && streambuf1->sputc(ch) == traits_type::eof())
      return traits_type::eof();

    if(streambuf2 && streambuf2->sputc(ch) == traits_type::eof())
      return traits_type::eof();
  }

  return 0;
}

std::streambuf::int_type teestreambuf::underflow()
{
  return traits_type::eof();
}

std::streambuf::int_type teestreambuf::sync()
{
  if(streambuf1 && streambuf1->pubsync() == traits_type::eof())
    return traits_type::eof();

  if(streambuf2 && streambuf2->pubsync() == traits_type::eof())
    return traits_type::eof();

  return 0;
}

/////////////////////////////////////////////////////////////////////////////
void tee::assign(std::ostream& s1, std::ostream& s2)
{
  teestreambuf* buf = dynamic_cast<teestreambuf*>(rdbuf());
  if(buf)
    buf->tie(s1.rdbuf(), s2.rdbuf());
};

void tee::assign_single(std::ostream& s)
{
  teestreambuf* buf = dynamic_cast<teestreambuf*>(rdbuf());
  if(buf)
    buf->tie(s.rdbuf());
}

