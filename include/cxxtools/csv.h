/*
 * Copyright (C) 2013 Tommi Maekitalo
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
 */

#ifndef CXXTOOLS_CSV_H
#define CXXTOOLS_CSV_H

#include <cxxtools/csvserializer.h>
#include <cxxtools/csvdeserializer.h>
#include <iostream>

namespace cxxtools
{
    /**
       Wrapper object to easily print serializable objects as csv to a output stream.

       CsvOObject is a little wrapper which makes it easy to output serializable
       objects into s ostream. For this the CsvOObject expects a reference to the
       wrapped object and has a output operator for a std::ostream, or actually
       a std::basic_ostream, which prints the object in json format.

       Example:
       \code
        std::vector<std::vector<int> > v(2);
        v[0].push_back(4);
        v[0].push_back(17);
        v[0].push_back(12);
        v[1].push_back(45);
        v[1].push_back(-3);
        v[1].push_back(76);
        std::cout << cxxtools::Csv(v) << std::endl;
       \endcode
     */
    template <typename ObjectType>
    class CsvOObject
    {
        const ObjectType& _object;
        Char _delimiter;
        Char _quote;
        String _lineEnding;

      public:
        /// Constructor. Needs the wrapped object.
        explicit CsvOObject(const ObjectType& object)
          : _object(object),
            _delimiter(0),
            _quote(0)
        { }

        CsvOObject& delimiter(Char delimiter)
        { _delimiter = delimiter; return *this; }

        CsvOObject& quote(Char quote)
        { _quote = quote; return *this; }

        CsvOObject& lineEnding(const String& lineEnding)
        { _lineEnding = lineEnding; return *this; }

        Char delimiter() const
        { return _delimiter; }

        Char quote() const
        { return _quote; }

        const String& lineEnding() const
        { return _lineEnding; }

        const ObjectType& object() const
        { return _object; }
    };

    /// The output operator for CsvOObject. It does the actual work.
    template <typename CharType, typename ObjectType>
    std::basic_ostream<CharType>& operator<< (std::basic_ostream<CharType>& out, const CsvOObject<ObjectType>& object)
    {
      try
      {
        CsvSerializer serializer(out);

        if (object.delimiter() != CsvParser::autoDelimiter)
          serializer.delimiter(object.delimiter());
        if (object.quote() != Char(0))
          serializer.quote(object.quote());
        if (!object.lineEnding().empty())
          serializer.lineEnding(object.lineEnding());

        serializer.serialize(object.object());
      }
      catch (const std::exception&)
      {
        out.setstate(std::ios::failbit);
        throw;
      }

      return out;
    }

    /// Function, which creates a CsvOObject.
    /// This makes the syntactic sugar perfect. See the example at CsvOObject
    /// for its use.
    template <typename ObjectType>
    CsvOObject<ObjectType> Csv(const ObjectType& object)
    {
      return CsvOObject<ObjectType>(object);
    }

    template <typename ObjectType>
    class CsvIOObject : public CsvOObject<ObjectType>
    {
        ObjectType& _object;
        bool _readTitle;
        bool _skipEmptyLines;

      public:
        explicit CsvIOObject(ObjectType& object)
          : CsvOObject<ObjectType>(object),
            _object(object),
            _readTitle(true),
            _skipEmptyLines(false)
        { }

        CsvIOObject& delimiter(Char delimiter)
        { CsvOObject<ObjectType>::delimiter(delimiter); return *this; }

        CsvIOObject& readTitle(bool sw)
        { _readTitle = sw; return *this; }

        CsvIOObject& skipEmptyLines(bool sw)
        { _skipEmptyLines = sw; return *this; }

        Char delimiter() const
        { return CsvOObject<ObjectType>::delimiter(); }

        bool readTitle() const
        { return _readTitle; }

        bool skipEmptyLines() const
        { return _skipEmptyLines; }

        ObjectType& object()
        { return _object; }
    };

    /// The input operator for CsvIOObject. It does the actual work.
    template <typename CharType, typename ObjectType>
    std::basic_istream<CharType>& operator>> (std::basic_istream<CharType>& in, CsvIOObject<ObjectType> object)
    {
      try
      {
        CsvDeserializer deserializer;
        if (object.delimiter() != CsvParser::autoDelimiter)
          deserializer.delimiter(object.delimiter());
        deserializer.readTitle(object.readTitle());
        deserializer.skipEmptyLines(object.skipEmptyLines());

        deserializer.read(in);

        deserializer.deserialize(object.object());
      }
      catch (const std::exception&)
      {
        in.setstate(std::ios::failbit);
        throw;
      }

      return in;
    }

    template <typename ObjectType>
    CsvIOObject<ObjectType> Csv(ObjectType& object)
    {
      return CsvIOObject<ObjectType>(object);
    }

}

#endif // CXXTOOLS_CSV_H

