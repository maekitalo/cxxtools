////////////////////////////////////////////////////////////////////////
// multifstream
//

#include <cxxtools/multifstream.h>
#include <iostream>

int main(int argc, char* argv[])
{
  try
  {
    for (int a = 1; argv[a]; ++a)
    {
      multi_ifstream in(argv[a]);
      char ch;
      while (in.get(ch))
        std::cout << ch;
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}
