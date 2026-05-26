/*
 * Copyright (C) 2011 Tommi Maekitalo
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

#include <cxxtools/csvdeserializer.h>
#include <cxxtools/serializationerror.h>
#include <stdexcept>

namespace cxxtools
{
const Char CsvDeserializer::autoDelimiter = CsvParser::autoDelimiter;

void CsvDeserializer::read(std::istream& in, TextCodec<Char, char>* codec)
{
    TextIStream s(in, codec);
    doDeserialize(s);
}

void CsvDeserializer::read(std::basic_istream<Char>& in)
{
    doDeserialize(in);
}

void CsvDeserializer::begin()
{
    cxxtools::Deserializer::begin();
    _parser.begin(*this);
}

void CsvDeserializer::doDeserialize(std::basic_istream<Char>& in)
{
    begin();

    try
    {
        while (true)
        {
            auto c = in.rdbuf()->sbumpc();
            if (!std::basic_streambuf<Char>::traits_type::not_eof(c))
            {
                in.setstate(std::ios::eofbit);
                break;
            }
            advance(std::basic_streambuf<Char>::traits_type::to_char_type(c));
        }
    }
    catch (const std::exception& e)
    {
        SerializationError::doThrow("parsing csv data failed in line " + std::to_string(_parser.lineNo()) + " column " + std::to_string(_parser.colNo()) + ": " + e.what());
    }

    if (in.rdstate() & std::ios::badbit)
        SerializationError::doThrow("reading csv data failed in line " + std::to_string(_parser.lineNo()) + " column " + std::to_string(_parser.colNo()));

    finish();
}

}
