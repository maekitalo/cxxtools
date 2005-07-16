/* cxxtools/cgi.h
   Copyright (C) 2004 Tommi Maekitalo

This file is part of cxxtools.

Cxxtools is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Cxxtools is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with cxxtools; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#ifndef CXXTOOLS_CGI_H
#define CXXTOOLS_CGI_H

#include <cxxtools/query_params.h>

namespace cxxtools
{

/**
 Class for easy extraction of CGI-parameters.

 This class reads automatically GET- and POST-parameters from stdin
 and the environvariable QUERY_STRING, like CGI-programs do. This eases
 writing CGI-programs in C++.

 example:
 \code
   int main()
   {
     cxxtools::Cgi q;  // this parses all input-parameters
     std::cout << q.header
               << "<html>\n"
               << "<body>\n"
               << "<form>\n"
               << "<input type=\"text\" name=\"v\"><br>\n"
               << "<input type=\"submit\"><br>\n"
               << "<hr>\n"
               << "</form>\n"
               << "you entered: <b>" << q["v"] << "</b>\n"
               << "</body>"
               << "</html>";
   }
 \endcode
 */
class Cgi : public QueryParams
{
  public:
    /// constructor reads parameters from server
    Cgi();

    static std::string header()  { return "Content-Type: text/html\r\n\r\n"; }
    static std::string header(const std::string& type)
      { return "Content-Type: " + type + "\r\n\r\n"; }
};

}

#endif // CXXTOOLS_CGI_H

