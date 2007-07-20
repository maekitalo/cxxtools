/* cxxtools/iconverter.h
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2006 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
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

#ifndef CXXTOOLS_ICONVERTER_H
#define CXXTOOLS_ICONVERTER_H

#include <cxxtools/iconvstream.h>
#include <string>
#include <sstream>

namespace cxxtools
{
  /**
    IConverter is a simple wrapper around cxxtools::iconvstream.

    Often the result of a character-conversion is needed as a std::string.
    This can be achived to let iconvstream to a ostringstream and get
    the string from the ostringstream. IConverter simplify this.

    example: convert a UTF8-encoded string to ISO8859-1:
    \code
     cxxtools::IConverter conv("ISO8859-1", "UTF8");
     std::string utf8string = getUtf8String();
     std::string iso8895_1 = conv.convert(utf8string);
     // or functor-style:
     std::string iso8895_1 = conv(utf8string);
    \endcode
   */
  class IConverter
  {
      std::string tocode;
      std::string fromcode;

    public:
      IConverter()  { }
      IConverter(const std::string& tocode_, const std::string& fromcode_)
        : tocode(tocode_),
          fromcode(fromcode_)
          { }

      void setToCode(const std::string& tocode_)      { tocode = tocode_; }
      void setFromCode(const std::string& fromcode_)  { fromcode = fromcode_; }
      const std::string& getToCode() const            { return tocode; }
      const std::string& getFromCode() const          { return fromcode; }

      template <typename objT>
      std::string convert(objT s) const
      {
        std::ostringstream o;

        iconvstream conv;
        conv.exceptions(std::ios::failbit | std::ios::badbit);
        conv.open(o, tocode.c_str(), fromcode.c_str());
        conv << s << std::flush;

        return o.str();
      }

      std::string convert(const char* data, unsigned size) const
      {
        std::ostringstream o;

        iconvstream conv;
        conv.exceptions(std::ios::failbit | std::ios::badbit);
        conv.open(o, tocode.c_str(), fromcode.c_str());
        conv.write(data, size);
        conv.flush();

        return o.str();
      }

      template <typename iteratorT>
      std::string convertRange(iteratorT begin, iteratorT end) const
      {
        std::ostringstream o;

        iconvstream conv;
        conv.exceptions(std::ios::failbit | std::ios::badbit);
        conv.open(o, tocode.c_str(), fromcode.c_str());
        for (iteratorT it = begin; it != end; ++it)
          conv << *it;

        conv.flush();

        return o.str();
      }

      template <typename objT>
      std::string operator() (objT s) const
        { return convert(s); }

      std::string operator() (const char* data, unsigned size) const
        { return convert(data, size); }
  };

}

#endif // CXXTOOLS_ICONVERTER_H

