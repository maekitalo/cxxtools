/* cxxtools/xmltag.h
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

#ifndef CXXTOOLS_XMLTAG_H
#define CXXTOOLS_XMLTAG_H

#include <string>
#include <iosfwd>

namespace cxxtools
{

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
  class Xmltag
  {
      std::string tag;
      std::ostream& out;
      static std::ostream* default_out;

    public:
      /// prints start-tag and remember tag
      Xmltag(const std::string& tag, std::ostream& out = *default_out);
      /// prints start-tag with parameters and remember tag
      Xmltag(const std::string& tag, const std::string& parameter,
        std::ostream& out = *default_out);
      /// closes tag
      ~Xmltag()   { close(); }

      static void setDefaultOutstream(std::ostream& out)
        { default_out = &out; }
      static std::ostream& getDefaultOutstream()
        { return *default_out; }

      const std::string& get() const   { return tag; }
      void close();
      void clear()                     { tag.clear(); }
  };

}

#endif // CXXTOOLS_XMLTAG_H

