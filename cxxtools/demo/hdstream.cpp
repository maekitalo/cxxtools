#include <iostream>
#include <fstream>
#include "cxxtools/hdstream.h"
#include <algorithm>

void hexdump(const char* file)
{
  std::ifstream in(file);
  cxxtools::hdostream hd;
  hd << in.rdbuf() << std::flush;
}

int main(int argc, char* argv[])
{
  std::for_each(argv + 1, argv + argc, hexdump);
}
