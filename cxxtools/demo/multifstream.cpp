////////////////////////////////////////////////////////////////////////
// multifstream
//

#include <cxxtools/multifstream.h>
#include <iostream>
#include <algorithm>

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::multi_ifstream in;
    std::copy(argv + 1,
              argv + argc,
              in.back_inserter());

    std::cout << in.rdbuf();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}
