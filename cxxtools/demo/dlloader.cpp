#include <exception>
#include <iostream>
#include <cxxtools/dlloader.h>

typedef double (*function_type)(double);

// to run the program you may have to set LTDL_LIBRARY_PATH
// to the path of libm.so

int main(int argc, char* argv[])
{
  try
  {
    std::cout << "load libm.so" << std::endl;
    cxxtools::dl::library lib("libm");

    std::cout << "sym cos" << std::endl;
    function_type cosine = (function_type)(lib["cos"]);

    std::cout << "call cos" << std::endl;
    std::cout << "cos(2.0) = " << cosine(2.0) << std::endl;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

