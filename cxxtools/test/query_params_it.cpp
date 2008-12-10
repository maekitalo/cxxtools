/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
// aufruf: query_params_it "a&a=4&a=blah&b=uhu&a=ttt"

#include <cxxtools/query_params.h>
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

