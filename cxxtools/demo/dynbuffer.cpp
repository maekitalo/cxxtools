#include <exception>
#include <iostream>
#include <cxxtools/dynbuffer.h>

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::dynbuffer<> buffer(512);
    char* data = buffer.data();
    // data points to a 512 bytes buffer, which is freed, when the
    // current scope is left either normally or through a exception.
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

