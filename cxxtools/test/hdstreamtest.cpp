#include <iostream>
#include <fstream>
#include "hdstream.h"

int main(int argc, char* argv[])
{
  for (int i = 1; i < argc; ++i)
  {
    std::ifstream in(argv[i]);
    hdostream hd;
    char ch;
    while (in.get(ch))
      hd.put(ch);
    hd.flush();
  }
}
