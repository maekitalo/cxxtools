/*
 * Copyright (C) 2015 Tommi Maekitalo
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


#include <cxxtools/propertiesserializer.h>
#include <cxxtools/utf8codec.h>
#include <cxxtools/convert.h>
#include <cxxtools/log.h>

log_define("cxxtools.propertiesserializer")

namespace cxxtools
{

namespace
{

    void checkTs(std::basic_ostream<Char>* _ts)
    {
        if (_ts == 0)
            throw std::logic_error("outputstream is not set in PropertiesSerializer");

    }

    void stringOut(std::basic_ostream<cxxtools::Char>& ts, const cxxtools::String& str)
    {
        for (cxxtools::String::const_iterator it = str.begin(); it != str.end(); ++it)
        {
            if (*it == L'"')
                ts << L'\\'
                    << L'\"';
            else if (*it == L'\\')
                ts << L'\\'
                    << L'\\';
            else if (*it == L'\b')
                ts << L'\\'
                    << L'b';
            else if (*it == L'\f')
                ts << L'\\'
                    << L'f';
            else if (*it == L'\n')
                ts << L'\\'
                    << L'n';
            else if (*it == L'\r')
                ts << L'\\'
                    << L'r';
            else if (*it == L'\t')
                ts << L'\\'
                    << L't';
            else if (it->value() >= 0x80 || it->value() < 0x20)
            {
                ts << L'\\'
                     << L'u';
                static const char hex[] = "0123456789abcdef";
                uint32_t v = it->value();
                for (uint32_t s = 16; s > 0; s -= 4)
                {
                    ts << Char(hex[(v >> (s - 4)) & 0xf]);
                }
            }
            else
                ts << *it;
        }
    }

}

PropertiesSerializer& PropertiesSerializer::begin(std::basic_ostream<Char>& ts)
{
    finish();

    _ts = &ts;

    delete _myts;
    _myts = 0;

    return *this;
}

PropertiesSerializer& PropertiesSerializer::begin(std::ostream& os,
    TextCodec<Char, char>* codec)
{
    finish();
    _ts = _myts = new TextOStream(os, codec ? codec : new Utf8Codec());
    return *this;
}

void PropertiesSerializer::finish()
{
    if (_ts)
        _ts->flush();

    _ts = 0;

    delete _myts;
    _myts = 0;

}

void PropertiesSerializer::format(const SerializationInfo& si, const std::string& key)
{
    checkTs(_ts);
    doFormat(si, key);
}

void PropertiesSerializer::doFormat(const SerializationInfo& si, const std::string& key)
{
    switch (si.category())
    {
        case SerializationInfo::Void:
            if (key.empty())
            {
                if (!si.name().empty())
                {
                    *_ts << String(si.name()) << L" =\n";
                }
            }
            else
            {
                *_ts << String(key);
                if (!si.name().empty())
                    *_ts << L'.' << String(si.name());
                *_ts << L" =\n";
            }

            break;

        case SerializationInfo::Value:
        {
            String value;
            si.getValue(value);

            log_debug("format value; key=\"" << key << "\" name=\"" << si.name() << "\" value=\"" << value);

            if (key.empty())
            {
                if (!si.name().empty())
                {
                    *_ts << String(si.name()) << L" = ";
                }
            }
            else
            {
                *_ts << String(key);
                if (!si.name().empty())
                    *_ts << L'.' << String(si.name());
                *_ts << L" = ";
            }

            stringOut(*_ts, value);
            *_ts << L'\n';

            break;
        }

        case SerializationInfo::Object:
        {
            std::string prefix = key;
            if (prefix.empty())
                prefix = si.name();

            log_debug("format object; prefix=\"" << prefix);

            for (SerializationInfo::const_iterator it = si.begin(); it != si.end(); ++it)
                format(*it, prefix);

            break;
        }

        case SerializationInfo::Array:
        {
            std::string prefix = key;
            if (!prefix.empty())
                prefix += '.';

            log_debug("format array; size=" << si.memberCount() << "; prefix=\"" << prefix);

            for (unsigned n = 0; n < si.memberCount(); ++n)
            {
                format(si.getMember(n), prefix + convert<std::string>(n));
            }

            break;
        }
    }
}

}
