#include <exception>
#include <iostream>
#include <cxxtools/dir.h>

int main(int argc, char* argv[])
{
  try
  {
    for (int a = 1; a < argc; ++a)
    {
      cxxtools::dir d(argv[a]);
      std::copy(d.begin(),
                d.end(),
                std::ostream_iterator<std::string>(std::cout, "\n"));
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

