/* iniclass.cpp
   Copyright (C) 2003 Tommi Mäkitalo

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

#include "cxxtools/iniclass.h"
#include <fstream>

namespace cxxtools
{

using namespace std;

void ini_file::read(const char* filename)
{
  std::ifstream ini(filename);
  read(ini);
}

void ini_file::read(std::istream& in)
{
  string section_name;

  char linebuffer[512];   // getline(istream, string) is broken on Solaris 5.7
  while ( in.getline(linebuffer, sizeof(linebuffer)) )
  {
    // skip whitespace
    const char* p = linebuffer;
    while (*p == ' ' || *p == '\t')
      ++p;

    if (*p == '\0' || *p == ';' || *p == '#')
      continue;

    // check for section
    if (*p == '[')
    {
      // section found
      ++p;
      const char* e = p;
      while (*e != '\0' && *e != ']')
        ++e;

      if (*e != ']' || e == p)
        continue;  // no valid sectionheader

      section_name = string(p, e);
    }
    else if (!section_name.empty())
    {
      // check for token

      const char* s = p;
      while (*p != '\0' && *p != '=')
        ++p;
      if (*p != '=')
        continue;
      const char* pp = p + 1;

      // Leerzeichen zwischen Token und '=' entfernen
      --p;
      while (p > linebuffer && (*p == ' ' || *p == '\t'))
        --p;

      if (p == linebuffer)
        continue;

      string token_name(s, p + 1);

      // skip whitespace after '='
      while (*pp == ' ' || *pp == '\t')
        ++pp;

      if (*pp == '\0')
        continue;

      const char* e = pp + 1;
      while (*e != '\0')
        ++e;

      // ignore whitespace at end of value
      while (*(e-1) == ' ' || *(e-1) == '\t')
        --e;

      if (e > pp)
      {
        std::string token(pp, e);
        (*this)[section_name][token_name] = token;
      }
    }
  }
}

ostream& operator << (ostream& out, const ini_file& ini)
{
  for (ini_file::const_iterator section_it = ini.begin();
       section_it != ini.end(); ++section_it)
  {
    out << '[' << (*section_it).first << "]\n";
    for (ini_file::const_section_iterator token_it = (*section_it).second.begin();
         token_it != (*section_it).second.end(); ++token_it)
    {
      out << (*token_it).first << '=' << (*token_it).second << '\n';
    }
  }

  return out;
}

}

#ifdef TEST

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    cerr << "Aufruf: " << argv[0] << " inifile" << endl;
    return -1;
  }

  ini_file ini(argv[1]);
  (cout << ini).flush();

  return 0;
}

#endif
