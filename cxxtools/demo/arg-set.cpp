////////////////////////////////////////////////////////////////////////
// arg2.cpp
//

#include <cxxtools/arg.h>
#include <iostream>

using cxxtools::arg;

void print_int(const char* txt, int i)
{
  std::cout << txt << ": " << i << std::endl;
}

void print_charp(const char* txt, const char* p)
{
  std::cout << txt << ": " << (p ? p : "NULL") << std::endl;
}

void print_string(const char* txt, const std::string& s)
{
  std::cout << txt << ": " << s << std::endl;
}

int main(int argc, char* argv[])
{
  try
  {
    arg<int> int_param(5); // default-value 5
    int_param.set(argc, argv, 'i');
    int_param.set(argc, argv, "--int");

    print_int("option -i|--int", int_param);

    arg<const char*> charp_param("hi"); // default-value
    charp_param.set(argc, argv, 'p');
    charp_param.set(argc, argv, "--charp");

    print_charp("option -p|--charp", charp_param);

    arg<std::string> string_param("hi"); // default-value
    string_param.set(argc, argv, 's');
    string_param.set(argc, argv, "--string");

    arg<std::string> string_arg;
    string_arg.setNoOpt(argc, argv);

    print_string("option -s|--string", string_param);
    print_string("string-arg", string_arg);

    std::cout << "unprocessed arguments:\n";
    for (int i = 1; i < argc; ++i)
      std::cout << '\t' << i << ": " << argv[i] << std::endl;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}
