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

#include <cxxtools/propertiesdeserializer.h>
#include <cxxtools/utf8codec.h>
#include <cxxtools/propertiesparser.h>
#include <cxxtools/log.h>

log_define("cxxtools.properties.deserializer")

namespace cxxtools
{
    PropertiesDeserializer::PropertiesDeserializer(std::istream& in, TextCodec<Char, char>* codec)
    {
        TextIStream s(in, codec);
        doDeserialize(s);
    }

    PropertiesDeserializer::PropertiesDeserializer(std::basic_istream<Char>& in)
    {
        doDeserialize(in);
    }

    class PropertiesDeserializer::Ev : public PropertiesParser::Event
    {
            PropertiesDeserializer& _deserializer;
            std::vector<std::string> _keys;
            std::string _longkey;

        public:
            Ev(PropertiesDeserializer& deserializer)
                : _deserializer(deserializer)
                { }

            virtual bool onKeyPart(const String& key);
            virtual bool onKey(const String& key);
            virtual bool onValue(const String& value);
    };

    bool PropertiesDeserializer::Ev::onKeyPart(const String& key)
    {
        _keys.push_back(Utf8Codec::encode(key));
        return false;
    }

    bool PropertiesDeserializer::Ev::onKey(const String& key)
    {
        _longkey = Utf8Codec::encode(key);
        return false;
    }

    bool PropertiesDeserializer::Ev::onValue(const String& value)
    {
        SerializationInfo* p = _deserializer.current()->findMember(_longkey);
        if (p == 0)
            p = &_deserializer.current()->addMember(_longkey);

        *p <<= value;
        p->addMember(std::string()) <<= value;

        if (_keys.size() > 1)
        {
            std::string key = _keys[0];          // foo.bar.baz => foo
            std::string member = _longkey.substr(key.size() + 1);   // foo.bar.baz => bar.baz
            for (unsigned n = 1; n < _keys.size(); ++n )
            {
                log_debug("add key " << key << " member " << member << " value " << value);
                SerializationInfo* p = _deserializer.current()->findMember(key);
                if (p == 0)
                    p = &_deserializer.current()->addMember(key);
                p->addMember(member) <<= value;

                key += '.';  // foo => foo.bar; foo.bar => foo.bar.baz
                key += _keys[n];

                member.erase(0, _keys[n].size() + 1);   // bar.baz => baz
            }
        }

        _keys.clear();
        _longkey.clear();

        return false;
    }

    void PropertiesDeserializer::doDeserialize(std::basic_istream<Char>& in)
    {
        begin();
        Ev ev(*this);
        PropertiesParser parser(ev);
        parser.parse(in);

        if (in.rdstate() & std::ios::badbit)
            SerializationError::doThrow("propertiesdeserialization failed");
    }
}
