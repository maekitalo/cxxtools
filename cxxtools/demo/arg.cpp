////////////////////////////////////////////////////////////////////////
// arg.cpp
//

#include <cxxtools/arg.h>
#include <iostream>

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
    using cxxtools::arg;

    arg<int> int_param(argc, argv, 'i');
    arg<int> long_int_param(argc, argv, "--int");
    arg<int> int_param_def(argc, argv, 'I', 5);

    print_int("option -i", int_param);
    print_int("option --int", long_int_param);
    print_int("option -I", int_param_def);

    arg<const char*> charp_param(argc, argv, 'p');
    arg<const char*> long_charp_param(argc, argv, "--charp");
    arg<const char*> charp_param_def(argc, argv, 'P', "hi");

    print_charp("option -p", charp_param);
    print_charp("option --charp", long_charp_param);
    print_charp("option -P", charp_param_def);

    arg<std::string> string_param(argc, argv, 's');
    arg<std::string> long_string_param(argc, argv, "--string");
    arg<std::string> string_param_def(argc, argv, 'S', "hi");
    arg<std::string> string_arg(argc, argv);

    print_string("option -s", string_param);
    print_string("option --string", long_string_param);
    print_string("option -S", string_param_def);
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
