#include <exception>
#include <iostream>
#include <cxxtools/arg.h>

using cxxtools::arg;

void process(const char* infile, const char* outfile, unsigned number)
{
  std::cout << "infile=" << infile << '\n'
            << "outfile=" << outfile << '\n'
            << "number=" << number << std::endl;
}

int main(int argc, char* argv[])
{
  try
  {
    arg<bool> verbose(argc, argv, 'v');
    arg<bool> debug(argc, argv, 'd');
    arg<const char*> infile(argc, argv, 'i', "");
    arg<const char*> outfile(argc, argv, 'o', "");
    arg<unsigned> num(argc, argv, 'n');

    std::cout << "verbose: " << (verbose ? "yes" : "no") << std::endl;
    std::cout << "debug: " << (debug ? "yes" : "no") << std::endl;
    process(infile, outfile, num);

    std::cout << "unprocessed arguments:\n";
    for (int i = 1; i < argc; ++i)
      std::cout << '\t' << i << ": " << argv[i] << std::endl;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

