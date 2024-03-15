/*
 * Copyright (C) 2019 Tommi Maekitalo
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

#include <cxxtools/inideserializer.h>

namespace cxxtools
{

namespace
{
    struct CodecReleaser
    {
        TextCodec<Char, char>* _codec;
        CodecReleaser(TextCodec<Char, char>* codec)
            : _codec(codec)
        { }

        ~CodecReleaser()
        {
            if (_codec->refs() == 0)
                delete _codec;
        }

    private:
        CodecReleaser(const CodecReleaser&)  { }
        void operator=(const CodecReleaser&)  { }
    };

    class Ev : public IniParser::Event
    {
    public:
        explicit Ev(SerializationInfo& si)
            : _si(si),
              _section(&_si)
            { }

        void parser(const IniParser& p) { _iniParser = &p; }
    private:
        const IniParser* _iniParser;
        SerializationInfo& _si;
        SerializationInfo* _section;
        String _key;

        virtual bool onSection(const String& section);
        virtual bool onKey(const String& key);
        virtual bool onValue(const String& key);
        virtual bool onError();
    };

    bool Ev::onSection(const String& section)
    {
        _section = &_si.addMember(Utf8Codec::encode(section));
        return false;
    }

    bool Ev::onKey(const String& key)
    {
        _key = key;
        return false;
    }

    bool Ev::onValue(const String& value)
    {
        _section->addMember(Utf8Codec::encode(_key)) <<= value;
        return false;
    }

    bool Ev::onError()
    {
        SerializationError::doThrow("parsing ini failed in line " + std::to_string(_iniParser->linenumber()) + " expected: " + _iniParser->expected());
        return true;
    }

}

IniDeserializer::IniDeserializer(std::istream& in, TextCodec<Char, char>* codec)
{
    Ev ev(si());
    IniParser parser(ev);
    ev.parser(parser);

    CodecReleaser r(codec);

    char ibuf;
    Char obuf;

    const char* fromBegin = &ibuf;
    const char* fromEnd = (&ibuf) + 1;
    const char* fromNext = fromEnd;
    Char* toBegin = &obuf;
    Char* toEnd = (&obuf) + 1;
    Char* toNext = &obuf;
    MBState mbstate;

    begin();

    while (true)
    {
        if (fromNext > fromBegin)
        {
            if (!in.get(ibuf))
                break;
            fromNext = fromBegin;
        }

        if (fromNext < fromEnd || toNext < toEnd)
        {
            std::codecvt_base::result r = codec->in(mbstate, fromBegin, fromEnd, fromNext, toBegin, toEnd, toNext);
            if (r == std::codecvt_base::error)
            {
                in.setstate(std::ios::failbit);
                return;
            }
        }

        if (toNext > toBegin)
        {
            int ret = parser.parse(obuf);
            if (ret == -1)
                in.putback(ibuf);
            if (ret != 0)
                break;
            toNext = &obuf;
        }
    }

    if (in.rdstate() & std::ios::badbit)
        SerializationError::doThrow("parsing ini failed in line " + std::to_string(parser.linenumber()) + " expected: " + parser.expected());

    parser.end();
}

IniDeserializer::IniDeserializer(std::basic_istream<Char>& in)
{
    Ev ev(si());
    IniParser parser(ev);
    ev.parser(parser);

    begin();
    Char ch;
    int ret;
    while (in.get(ch))
    {
        ret = parser.parse(ch);
        if (ret == -1)
            in.putback(ch);
        if (ret != 0)
            break;
    }

    if (in.rdstate() & std::ios::badbit)
        SerializationError::doThrow("parsing ini failed in line " + std::to_string(parser.linenumber()) + " expected: " + parser.expected());

    parser.end();
}

}
