#include <exception>
#include <iostream>
#include "cxxtools/iniclass.h"

int main(int argc, char* argv[])
{
  try
  {
    if (argc < 2)
    {
      std::cerr << "usage: " << argv[0] << " inifile" << std::endl;
      return -1;
    }

    cxxtools::ini_file iniFile(argv[1]);
    std::cout << iniFile.get_value("section", "value") << std::endl;
    std::cout << iniFile.get_value("section", "int-value", 5) << std::endl;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

