////////////////////////////////////////////////////////////////////////
// read value from ini-file
//

#include <exception>
#include <iostream>
#include "cxxtools/iniclass.h"

int main(int argc, char* argv[])
{
  try
  {
    if (argc < 4)
    {
      std::cerr << "usage: " << argv[0]
        << " inifile section key [default]" << std::endl;
      return -1;
    }

    const char* fname = argv[1];
    const char* section = argv[2];
    const char* key = argv[3];
    const char* defvalue = argv[4] ? argv[4] : "";

    cxxtools::ini_file iniFile(fname);
    std::cout << iniFile.get_value(section, key, defvalue) << std::endl;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

