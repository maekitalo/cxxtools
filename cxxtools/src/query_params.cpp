/* query_params.cpp
   Copyright (C) 2003,2004 Tommi Mäkitalo

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

#include "cxxtools/query_params.h"
#include <iterator>
#include <iostream>
#include <stdlib.h>

namespace cxxtools
{

template <class iterator_type>
void _parse_url(
  iterator_type begin,
  iterator_type end,
  query_params::unnamed_params_type& unnamed_params,
  query_params::named_params_type& named_params)
{
  enum state_type {
    state_key,
    state_value,
    state_keyesc,
    state_valueesc,
    state_end
  };

  state_type state = state_key;
  std::string s1, s2;
  int cnt = -1;
  int v = 0;

  for(iterator_type charp = begin;
      state != state_end && charp != end;
      ++charp)
  {
    char ch = *charp;
    switch(state)
    {
      case state_key:
        switch(ch)
        {
          case '=':
            state = state_value;
            break;

          case '&':
            unnamed_params.push_back(s1);
            s1.clear();
            break;

          case '%':
            cnt = 0;
            v = 0;
            state = state_keyesc;
            break;

          case ' ':
          case '\t':
            state = state_end;
            break;

          default:
            s1.push_back(ch);
        }
        break;

      case state_value:
        switch(ch)
        {
          case '%':
            cnt = 0;
            v = 0;
            state = state_valueesc;
            break;

          case '&':
            named_params[s1].push_back(s2);
            s1.clear();
            s2.clear();
            state = state_key;
            break;

          case '+':
            s2.push_back(' ');
            break;

          default:
            s2.push_back(ch);
        }
        break;

      case state_keyesc:
      case state_valueesc:
        v = (v << 4)
          + ((ch >= 'a' && ch <= 'f') ? (ch - 'a' + 10)
           : (ch >= 'A' && ch <= 'F') ? (ch - 'A' + 10)
           : (ch - '0'));
        if (++cnt >= 2)
        {
          switch(state)
          {
            case state_keyesc:
              s1.push_back((char)v);
              state = state_key;
              break;

            case state_valueesc:
              s2.push_back((char)v);
              state = state_value;
              break;

            default:
              break;
          }
          cnt = -1;
        }
        break;

      case state_end:
        break;
    }
  }

  switch(state)
  {
    case state_key:
    case state_keyesc:
      if (!s1.empty())
        unnamed_params.push_back(s1);
      break;

    case state_value:
    case state_valueesc:
      named_params[s1].push_back(s2);
      break;

    case state_end:
      break;
  }
}

void query_params::parse_url(const std::string& url)
{
  _parse_url(url.begin(), url.end(), unnamed_params, named_params);
}

void query_params::parse_url(const char* url)
{
  const char* end = url;
  while (*end)
    ++end;
  _parse_url(url, end, unnamed_params, named_params);
}

void query_params::parse_url(std::istream& url_stream)
{
  _parse_url(
    std::istream_iterator<char>(url_stream),
    std::istream_iterator<char>(),
    unnamed_params, named_params);
}

static void appendUrl(std::string& url, char ch)
{
  static char hex[] = "0123456789ABCDEF";
  if (ch > 32 && ch < 127 && ch != '%' && ch != '+' && ch != '=')
    url += ch;
  else if (ch == ' ')
    url += '+';
  else
  {
    url += '%';
    char hi = (ch >> 4) & 0x0f;
    char lo = ch & 0x0f;
    url += hex[hi];
    url += hex[lo];
  }
}

static void appendUrl(std::string& url, const std::string& str)
{
  for (std::string::const_iterator i = str.begin();
       i != str.end(); ++i)
    appendUrl(url, *i);
}

std::string query_params::getUrl() const
{
  std::string ret;

  if (useParentValues())
  {
    ret = parent->getUrl();
    if (!ret.empty())
      ret += '&';
  }

  for (unnamed_params_type::const_iterator u = unnamed_params.begin();
       u != unnamed_params.end(); ++u)
  {
    appendUrl(ret, *u);
    ret += '&';
  }

  for (named_params_type::const_iterator n = named_params.begin();
       n != named_params.end(); ++n)
  {
    for (unnamed_params_type::const_iterator u = n->second.begin();
         u != n->second.end(); ++u)
    {
      ret += n->first;
      ret += '=';
      appendUrl(ret, *u);
      ret += '&';
    }
  }

  if (!ret.empty())
    ret.erase(ret.end() - 1);

  return ret;
}

std::string query_params::dump() const
{
  std::string ret;

  for (unnamed_params_type::const_iterator u = unnamed_params.begin();
       u != unnamed_params.end(); ++u)
  {
    ret += '"';
    ret += *u;
    ret += "\" ";
  }

  for (named_params_type::const_iterator n = named_params.begin();
       n != named_params.end(); ++n)
  {
    for (unnamed_params_type::const_iterator u = n->second.begin();
         u != n->second.end(); ++u)
    {
      ret += n->first;
      ret += "=\"";
      ret += *u;
      ret += "\" ";
    }
  }

  if (parent)
  {
    ret += "{ ";
    if (!use_parent_values)
      ret += '(';
    ret += parent->dump();
    if (!use_parent_values)
      ret += ')';
    ret += " }";
  }

  return ret;
}

}
