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
#include <cxxtools/textstream.h>
#include <cxxtools/utf8codec.h>
#include <cxxtools/propertiesparser.h>
#include <cxxtools/envsubst.h>
#include <cxxtools/trim.h>
#include <cxxtools/join.h>
#include <cxxtools/log.h>

log_define("cxxtools.properties.deserializer")

namespace cxxtools
{
    PropertiesDeserializer::PropertiesDeserializer(std::istream& in, TextCodec<Char, char>* codec)
        : _envSubst(false),
          _trim(false)
    {
        TextIStream s(in, codec);
        doDeserialize(s);
    }

    PropertiesDeserializer::PropertiesDeserializer(std::basic_istream<Char>& in)
        : _envSubst(false),
          _trim(false)
    {
        doDeserialize(in);
    }

    PropertiesDeserializer::PropertiesDeserializer()
        : _envSubst(false),
          _trim(false)
    {
    }

    void PropertiesDeserializer::read(std::istream& in, TextCodec<Char, char>* codec)
    {
        TextIStream s(in, codec);
        doDeserialize(s);
    }

    void PropertiesDeserializer::read(std::basic_istream<Char>& in)
    {
        doDeserialize(in);
    }

    class PropertiesDeserializer::Ev : public PropertiesParser::Event
    {
            PropertiesDeserializer& _deserializer;
            std::vector<std::string> _keys;

        public:
            Ev(PropertiesDeserializer& deserializer)
                : _deserializer(deserializer)
                { }

            bool onKeyPart(const String& key) override;
            bool onKey(const String& key) override;
            bool onValue(String&& value) override;
    };

    bool PropertiesDeserializer::Ev::onKeyPart(const String& key)
    {
        log_finer("onKeyPart(" << key << ')');
        _keys.push_back(Utf8Codec::encode(key));
        return false;
    }

    bool PropertiesDeserializer::Ev::onKey(const String& key)
    {
        log_finer("onKey(" << key << ')');
        return false;
    }

    namespace
    {
        void addValue(SerializationInfo& si, const std::string& key, const std::vector<std::string>& keys, unsigned idx, String&& value)
        {
            log_trace("val key=" << key << " keys.size=" << keys.size() << " idx=" << idx << " value=" << value);

            std::string k = join(keys.begin() + idx, keys.end(), '.');
            log_debug("key " << k << " value " << value);
            si.getAddMember(k).setValue(String(value));

            SerializationInfo& sia = si.getAddMember(key);

            for (unsigned n = idx + 1; n < keys.size(); ++n)
            {
                addValue(sia, join(keys.begin() + idx + 1, keys.begin() + n + 1, '.'), keys, n, String(value));
                addValue(si, join(keys.begin(), keys.begin() + n + 1, '.'), keys, n, std::move(value));
            }
        }

        void addValue(SerializationInfo& si, const std::vector<std::string>& keys, String&& value)
        {
            if (keys.empty())
                return;

            SerializationInfo& sia = si.getAddMember(keys[0]);

            if (keys.size() == 1)
            {
                log_debug("key " << keys[0] << " value " << value << "(1)");
                sia.setValue(std::move(value));
            }
            else if (keys.size() > 1)
                addValue(si, keys[0], keys, 0, std::move(value));
        }
    }

    bool PropertiesDeserializer::Ev::onValue(String&& value)
    {
        log_finer("onValue(" << value << ')');

        if (_deserializer._envSubst)
        {
            value = Utf8Codec::decode(cxxtools::envSubst(Utf8Codec::encode(value)));
        }

        addValue(*_deserializer.current(), _keys, std::move(value));

        _keys.clear();

        return false;
    }

    void PropertiesDeserializer::doDeserialize(std::basic_istream<Char>& in)
    {
        begin();
        Ev ev(*this);
        PropertiesParser parser(ev, _trim);
        parser.parse(in);

        if (in.rdstate() & std::ios::badbit)
            SerializationError::doThrow("propertiesdeserialization failed");
    }
}
