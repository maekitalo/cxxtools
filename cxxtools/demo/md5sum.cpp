#include <fstream>
#include <iostream>
#include <algorithm>
#include <cxxtools/md5stream.h>

int main(int argc, char* argv[])
{
  cxxtools::md5stream s;
  for (int i = 1; i < argc; ++i)
  {
    std::ifstream in(argv[i]);
    if (in)
    {
      // copy file to md5stream:
      s << in.rdbuf();
      std::cout << s.getHexDigest() << "  " << argv[i] << std::endl;
    }
  }
}

