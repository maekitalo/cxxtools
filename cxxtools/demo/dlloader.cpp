#include <exception>
#include <iostream>
#include <cxxtools/dlloader.h>

typedef double (*function_type)(double);

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::dl::library lib("libm.so");
    function_type cosine = (function_type)(lib["cos"]);
    std::cout << "cos(2.0) = " << cosine(2.0) << std::endl;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

