// aufruf: query_params_it "a&a=4&a=blah&b=uhu&a=ttt"

#include <query_params.h>
#include <exception>
#include <iostream>
#include <set>
#include <algorithm>
#include <iterator>

int main(int argc, char* argv[])
{
  try
  {
    query_params p;
    p.parse_url("a=abc");
    std::cout << "p=\"" << p.getUrl() << '"' << std::endl;
    for (int i = 1; i < argc; ++i)
    {
      query_params q(p);
      q.parse_url(argv[i]);

      std::cout << "q=\"" << q.getUrl() << '"' << std::endl;

      std::cout << "unnamed parameters: ";
      std::copy(q.begin(), q.end(), std::ostream_iterator<std::string>(std::cout, "\t"));
      std::cout << std::endl;

      std::cout << "names: ";
      std::set<std::string> names;
      q.getNames(std::inserter(names, names.begin()));
      std::copy(names.begin(), names.end(),
        std::ostream_iterator<std::string>(std::cout, "\t"));
      std::cout << std::endl;

      for (std::set<std::string>::const_iterator it = names.begin();
           it != names.end(); ++it)
      {
        std::cout << "count " << *it << ": " << q.paramcount(*it) << std::endl;

        std::cout << "values of " << *it << ": ";
        std::copy(q.begin(*it), q.end(),
          std::ostream_iterator<std::string>(std::cout, "\t"));
        std::cout << std::endl;
      }
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

