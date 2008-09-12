/* cxxtools/cgi.h
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2004 Tommi Maekitalo
 *
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
 *
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

