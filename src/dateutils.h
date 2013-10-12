/*
 * Copyright (C) 2013 Tommi Maekitalo
 *
 */

#ifndef CXXTOOLS_DATEUTILS_H
#define CXXTOOLS_DATEUTILS_H

#include <string>

namespace cxxtools
{
  unsigned getUnsigned(std::string::const_iterator& b, std::string::const_iterator e, unsigned digits);
  unsigned getInt(std::string::const_iterator& b, std::string::const_iterator e, unsigned digits);
  unsigned getMilliseconds(std::string::const_iterator& b, std::string::const_iterator e);
  void appendD4(std::string& s, unsigned v);
  void appendD3(std::string& s, unsigned v);
  void appendD2(std::string& s, unsigned v);
  void appendD1(std::string& s, unsigned short v);
}

#endif // CXXTOOLS_DATEUTILS_H

