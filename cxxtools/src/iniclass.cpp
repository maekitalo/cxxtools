////////////////////////////////////////////////////////////////////////
// iniclass.cpp
//
// Author: Tommi Mäkitalo, Dr. Eckhardt + Partner GmbH
// Datum:  13.3.2002
//

#include "cxxtools/iniclass.h"
#include <fstream>

using namespace std;

void IniFile::Read(const char* filename)
{
  std::ifstream ini(filename);
  Read(ini);
}

void IniFile::Read(std::istream& in)
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

std::string IniFile::getStringValue(const std::string& section,
                                    const std::string& token,
                                    const std::string& def) const
{
  // find section

  const_iterator section_it = find(section);

  if (section_it != end())
  {
    // find token

    const_section_iterator token_it = (*section_it).second.find(token);

    if (token_it != (*section_it).second.end())
      return (*token_it).second;
  }

  return def;
}

ostream& operator << (ostream& out, const IniFile& ini)
{
  for (IniFile::const_iterator section_it = ini.begin();
       section_it != ini.end(); ++section_it)
  {
    out << '[' << (*section_it).first << "]\n";
    for (IniFile::const_section_iterator token_it = (*section_it).second.begin();
         token_it != (*section_it).second.end(); ++token_it)
    {
      out << (*token_it).first << '=' << (*token_it).second << '\n';
    }
  }

  return out;
}

#ifdef TEST

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    cerr << "Aufruf: " << argv[0] << " inifile" << endl;
    return -1;
  }

  IniFile ini(argv[1]);
  (cout << ini).flush();

  return 0;
}

#endif
