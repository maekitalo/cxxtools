/* cxxtools/cgi.h
   Copyright (C) 2004 Tommi Mäkitalo

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
     cgi q;  // this parses all input-parameters
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
class cgi : public query_params
{
  public:
    /// constructor reads parameters from server
    cgi();

    static std::string header()  { return "Content-Type: text/html\r\n\r\n"; }
    static std::string header(const std::string& type)
      { return "Content-Type: " + type + "\r\n\r\n"; }
};

/**
 This class outputs xml-tags and end-tags.

 When the class is put on the stack it prints the start-tag to the
 outputstream and when leaving scope prints end-tag. The end-tag
 is the start tag with '/' inserted at the second position.

 Tags are specified by text including or not including '<' and '>'.
 These brackets are recognized and printed also when not passed.
 Parameters in the tag can be passed as a separate parameter
 or delimited by space. After the first space everything is parameter
 and not repeated in the closing tag.

 Tags are closed in the destructor, if not already closed explicitely
 by calling the close()-method.
 */
class xmltag
{
    std::string tag;
    std::ostream& out;
    static std::ostream* default_out;

  public:
    /// prints start-tag and remember tag
    xmltag(const std::string& tag, std::ostream& out = *default_out);
    /// prints start-tag with parameters and remember tag
    xmltag(const std::string& tag, const std::string& parameter,
      std::ostream& out = *default_out);
    /// closes tag
    ~xmltag()   { close(); }

    static void setDefaultOutstream(std::ostream& out)
      { default_out = &out; }
    static std::ostream& getDefaultOutstream()
      { return *default_out; }

    const std::string& get() const   { return tag; }
    void close();
    void clear()                     { tag.clear(); }
};

}

#endif // CXXTOOLS_CGI_H

