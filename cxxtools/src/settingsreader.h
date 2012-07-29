/*
 * Copyright (C) 2005-2007 by Dr. Marc Boris Duerner
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
#ifndef cxxtools_SettingsReader_h
#define cxxtools_SettingsReader_h

#include "cxxtools/settings.h"
#include "cxxtools/char.h"
#include "cxxtools/serializationinfo.h"
#include <iostream>
#include <cctype>

namespace cxxtools {

class SettingsReader
{
    public:
        class State
        {
            public:
                virtual State* onChar(cxxtools::Char c, SettingsReader& reader)
                {
                    if( c == std::char_traits<cxxtools::Char>::to_char_type( std::char_traits<cxxtools::Char>::eof() ) )
                    {
                            return this->onEof(reader);
                    }

                    switch( c.value() )
                    {
                            case '\n':
                            case ' ':
                            case '\t':
                            case '\r':
                                return this->onSpace(c, reader);

                            case '"':
                                return this->onQuote(c, reader);

                            case ',':
                                return this->onComma(c, reader);

                            case '=':
                                return this->onEqual(c, reader);

                            case '#':
                            case ';':
                                return this->onHash(c, reader);

                            case '{':
                                return this->onOpenCurlyBrace(c, reader);

                            case '}':
                                return this->onCloseCurlyBrace(c, reader);

                            case '(':
                                return this->onOpenBrace(c, reader);

                            case ')':
                                return this->onCloseBrace(c, reader);

                            case '[':
                                return this->onOpenSquareBrace(c, reader);

                            case ']':
                                return this->onCloseSquareBrace(c, reader);

                            default:
                                return this->onAlpha(c, reader);
                    }

                    this->syntaxError(reader.line());
                    return 0;
                }

                virtual State* onEof(SettingsReader& reader)
                {
                    this->syntaxError(reader.line());
                    return this;
                }

                virtual ~State()
                {}

            private:
                virtual State* onSpace(cxxtools::Char c, SettingsReader& reader)
                {
                    this->syntaxError(reader.line());
                    return this;
                }

                virtual State* onQuote(cxxtools::Char c, SettingsReader& reader)
                {
                    this->syntaxError(reader.line());
                    return this;
                }

                virtual State* onComma(cxxtools::Char c, SettingsReader& reader)
                {
                    this->syntaxError(reader.line());
                    return this;
                }

                virtual State* onEqual(cxxtools::Char c, SettingsReader& reader)
                {
                    this->syntaxError(reader.line());
                    return this;
                }

                virtual State* onOpenCurlyBrace(cxxtools::Char c, SettingsReader& reader)
                {
                    this->syntaxError(reader.line());
                    return this;
                }

                virtual State* onCloseCurlyBrace(cxxtools::Char c, SettingsReader& reader)
                {
                    this->syntaxError(reader.line());
                    return this;
                }

                virtual State* onOpenBrace(cxxtools::Char c, SettingsReader& reader)
                {
                    this->syntaxError(reader.line());
                    return this;
                }

                virtual State* onCloseBrace(cxxtools::Char c, SettingsReader& reader)
                {
                    this->syntaxError(reader.line());
                    return this;
                }

                virtual State* onOpenSquareBrace(cxxtools::Char c, SettingsReader& reader)
                {
                    this->syntaxError(reader.line());
                    return this;
                }

                virtual State* onCloseSquareBrace(cxxtools::Char c, SettingsReader& reader)
                {
                    this->syntaxError(reader.line());
                    return this;
                }

                virtual State* onHash(cxxtools::Char c, SettingsReader& reader)
                {
                    reader.beginComment(); // save current state
                    return OnComment::instance();
                }

                virtual State* onAlpha(cxxtools::Char c, SettingsReader& reader)
                {
                    this->syntaxError(reader.line());
                    return this;
                }

            protected:
                void syntaxError(unsigned line);
        };


        class OnComment : public State
        {
            public:
                State* onChar(cxxtools::Char c, SettingsReader& reader)
                {
                    if( c == '\n' )
                    {
                        // restore state before comment
                        return reader.endComment();
                    }

                    return this;
                }

                static State* instance()
                {
                    static OnComment _state;
                    return &_state;
                }
        };


        class BeginStatement : public State
        {
            virtual State* onSpace(cxxtools::Char c, SettingsReader& reader)
            {
                return this;
            }

            virtual State* onQuote(cxxtools::Char c, SettingsReader& reader)
            {
                if(reader.depth() == 0)
                    this->syntaxError(reader.line());

                return OnQuotedValue::instance();
            }

            virtual State* onOpenSquareBrace(cxxtools::Char c, SettingsReader& reader)
            {
                reader.beginSection();
                return OnSection::instance();
            }

            virtual State* onOpenCurlyBrace(cxxtools::Char c, SettingsReader& reader)
            {
                reader.enterMember();
                return OnCurly::instance();
            }

            virtual State* onAlpha(cxxtools::Char c, SettingsReader& reader)
            {
                reader.buildToken(c);
                return BeginType::instance();
            }

            virtual State* onEof(SettingsReader& reader)
            {
                return this;
            }

            public:
                static State* instance()
                {
                    static BeginStatement _state;
                    return &_state;
                }
        };


        class OnSection : public State
        {
            virtual State* onSpace(cxxtools::Char c, SettingsReader& reader)
            {
                return this;
            }

            virtual State* onCloseSquareBrace(cxxtools::Char c, SettingsReader& reader)
            {
                return BeginStatement::instance();
            }

            virtual State* onAlpha(cxxtools::Char c, SettingsReader& reader)
            {
                reader.buildSection(c);
                return this;
            }

            public:
                static State* instance()
                {
                    static OnSection _state;
                    return &_state;
                }
        };


        class BeginType : public State
        {
            virtual State* onSpace(cxxtools::Char c, SettingsReader& reader)
            {
                return AfterName::instance();
            }

            virtual State* onEqual(cxxtools::Char c, SettingsReader& reader)
            {
                if(reader.depth() == 0)
                    reader.enterMember();
                else
                    reader.pushName();

                return OnEqual::instance();
            }

            virtual State* onComma(cxxtools::Char c, SettingsReader& reader)
            {
                reader.pushValue();
                reader.leaveMember();
                reader.enterMember();
                return BeginStatement::instance();
            }

            virtual State* onOpenBrace(cxxtools::Char c, SettingsReader& reader)
            {
                reader.pushTypeName();
                return BeginTypedValue::instance();
            }

            virtual State* onOpenCurlyBrace(cxxtools::Char c, SettingsReader& reader)
            {
                if(reader.depth() == 0)
                    this->syntaxError(reader.line());

                reader.pushTypeName();
                reader.enterMember();
                return OnCurly::instance();
            }

            virtual State* onCloseCurlyBrace(cxxtools::Char c, SettingsReader& reader)
            {
                reader.pushValue();
                reader.leaveMember();
                reader.leaveMember();
                return OnCloseCurly::instance();
            }

            virtual State* onAlpha(cxxtools::Char c, SettingsReader& reader)
            {
                reader.buildToken(c);
                return this;
            }

            public:
                static State* instance()
                {
                    static BeginType _state;
                    return &_state;
                }
        };


        class AfterName : public BeginType
        {
            virtual State* onSpace(cxxtools::Char c, SettingsReader& reader)
            {
                return this;
            }

            virtual State* onAlpha(cxxtools::Char c, SettingsReader& reader)
            {
                this->syntaxError(reader.line());
                return this;
            }

            public:
                static State* instance()
                {
                    static AfterName _state;
                    return &_state;
                }
        };


        class OnEqual : public State
        {
            virtual State* onSpace(cxxtools::Char c, SettingsReader& reader)
            {
                if (c.value() == '\n')
                {
                    reader.pushValue();
                    return AfterRValue::instance();
                }
                else
                    return this;
            }

            virtual State* onQuote(cxxtools::Char c, SettingsReader& reader)
            {
                return OnQuotedValue::instance();
            }

            virtual State* onOpenCurlyBrace(cxxtools::Char c, SettingsReader& reader)
            {
                reader.enterMember();
                return OnCurly::instance();
            }

            virtual State* onAlpha(cxxtools::Char c, SettingsReader& reader)
            {
                reader.buildToken(c);
                return OnRValue::instance();
            }

            public:
                static State* instance()
                {
                    static OnEqual _state;
                    return &_state;
                }
        };


        class OnQuotedValue : public State
        {
            virtual State* onSpace(cxxtools::Char c, SettingsReader& reader)
            {
                reader.buildToken(c);
                return this;
            }

            virtual State* onQuote(cxxtools::Char c, SettingsReader& reader)
            {
                reader.pushValue();
                return AfterQuotedValue::instance();
            }

            virtual State* onComma(cxxtools::Char c, SettingsReader& reader)
            {
                reader.buildToken(c);
                return this;
            }

            virtual State* onEqual(cxxtools::Char c, SettingsReader& reader)
            {
                reader.buildToken(c);
                return this;
            }

            virtual State* onHash(cxxtools::Char c, SettingsReader& reader)
            {
                reader.buildToken(c);
                return this;
            }

            virtual State* onOpenCurlyBrace(cxxtools::Char c, SettingsReader& reader)
            {
                reader.buildToken(c);
                return this;
            }

            virtual State* onCloseCurlyBrace(cxxtools::Char c, SettingsReader& reader)
            {
                reader.buildToken(c);
                return this;
            }

            virtual State* onOpenBrace(cxxtools::Char c, SettingsReader& reader)
            {
                reader.buildToken(c);
                return this;
            }

            virtual State* onCloseBrace(cxxtools::Char c, SettingsReader& reader)
            {
                reader.buildToken(c);
                return this;
            }

            virtual State* onOpenSquareBrace(cxxtools::Char c, SettingsReader& reader)
            {
                reader.buildToken(c);
                return this;
            }

            virtual State* onCloseSquareBrace(cxxtools::Char c, SettingsReader& reader)
            {
                reader.buildToken(c);
                return this;
            }

            virtual State* onAlpha(cxxtools::Char c, SettingsReader& reader)
            {
                if(c == '\\')
                    c = reader.getEscaped();

                reader.buildToken(c);
                return this;
            }

            public:
                static State* instance()
                {
                    static OnQuotedValue _state;
                    return &_state;
                }
        };


        class AfterValue : public State
        {
            virtual State* onSpace(cxxtools::Char c, SettingsReader& reader)
            {
                return this;
            }

            virtual State* onComma(cxxtools::Char c, SettingsReader& reader)
            {
                reader.leaveMember();
                reader.enterMember();
                return BeginStatement::instance();
            }

            virtual State* onCloseCurlyBrace(cxxtools::Char c, SettingsReader& reader)
            {
                reader.leaveMember();
                reader.leaveMember();
                return OnCloseCurly::instance();
            }

            virtual State* onAlpha(cxxtools::Char c, SettingsReader& reader)
            {
                reader.leaveMember();
                reader.buildToken(c);
                return BeginType::instance();
            }

            virtual State* onEof(SettingsReader& reader)
            {
                reader.leaveMember();
                if(reader.depth() > 1)
                    this->syntaxError(reader.line());

                return this;
            }

            public:
                static State* instance()
                {
                    static AfterValue _state;
                    return &_state;
                }
        };


        class AfterQuotedValue : public AfterValue
        {
            virtual State* onQuote(cxxtools::Char c, SettingsReader& reader)
            {
                /// TODO: multi-line strings
                return this;
            }

            virtual State* onOpenSquareBrace(cxxtools::Char c, SettingsReader& reader)
            {
                reader.leaveMember();
                reader.beginSection();
                return OnSection::instance();
            }

            public:
                static State* instance()
                {
                    static AfterQuotedValue _state;
                    return &_state;
                }
        };


        class OnRValue : public State
        {
            virtual State* onSpace(cxxtools::Char c, SettingsReader& reader)
            {
                return AfterRValue::instance();
            }

            virtual State* onOpenSquareBrace(cxxtools::Char c, SettingsReader& reader)
            {
                reader.pushValue();
                reader.leaveMember();

                reader.beginSection();
                return OnSection::instance();
            }

            virtual State* onOpenCurlyBrace(cxxtools::Char c, SettingsReader& reader)
            {
                reader.pushTypeName();
                reader.enterMember();
                return OnCurly::instance();
            }

            virtual State* onOpenBrace(cxxtools::Char c, SettingsReader& reader)
            {
                reader.pushTypeName();
                return BeginTypedValue::instance();
            }

            virtual State* onCloseCurlyBrace(cxxtools::Char c, SettingsReader& reader)
            {
                reader.pushValue();
                reader.leaveMember();
                reader.leaveMember();
                return OnCloseCurly::instance();
            }

            virtual State* onComma(cxxtools::Char c, SettingsReader& reader)
            {
                reader.pushValue();
                reader.leaveMember();
                reader.enterMember();
                return BeginStatement::instance();
            }

            virtual State* onAlpha(cxxtools::Char c, SettingsReader& reader)
            {
                reader.buildToken(c);
                return this;
            }

            virtual State* onEof(SettingsReader& reader)
            {
                reader.pushValue();
                reader.leaveMember();
                return BeginStatement::instance();
            }

            public:
                static State* instance()
                {
                    static OnRValue _state;
                    return &_state;
                }
        };


        class AfterRValue : public OnRValue
        {
            virtual State* onSpace(cxxtools::Char c, SettingsReader& reader)
            {
                return this;
            }

            virtual State* onAlpha(cxxtools::Char c, SettingsReader& reader)
            {
                reader.pushValue();
                reader.leaveMember();
                reader.buildToken(c);
                return BeginType::instance();
            }

            public:
                static State* instance()
                {
                    static AfterRValue _state;
                    return &_state;
                }
        };


        class OnCurly : public State
        {
            virtual State* onSpace(cxxtools::Char c, SettingsReader& reader)
            {
                return this;
            }

            virtual State* onOpenCurlyBrace(cxxtools::Char c, SettingsReader& reader)
            {
                reader.enterMember();
                return OnCurly::instance();
            }

            virtual State* onCloseCurlyBrace(cxxtools::Char c, SettingsReader& reader)
            {
                reader.leaveMember();
                return OnCloseCurly::instance();
            }

            virtual State* onQuote(cxxtools::Char c, SettingsReader& reader)
            {
                return OnQuotedValue::instance();
            }

            virtual State* onAlpha(cxxtools::Char c, SettingsReader& reader)
            {
                reader.buildToken(c);
                return BeginType::instance();
            }

            public:
                static State* instance()
                {
                    static OnCurly _state;
                    return &_state;
                }
        };


        class OnCloseCurly : public State
        {
            virtual State* onSpace(cxxtools::Char c, SettingsReader& reader)
            {
                return this;
            }

            virtual State* onOpenSquareBrace(cxxtools::Char c, SettingsReader& reader)
            {
                reader.beginSection();
                return OnSection::instance();
            }

            virtual State* onCloseCurlyBrace(cxxtools::Char c, SettingsReader& reader)
            {
                reader.leaveMember();
                return this;
            }

            virtual State* onComma(cxxtools::Char c, SettingsReader& reader)
            {
                if(reader.depth() == 0)
                {
                    this->syntaxError(reader.line());
                }

                reader.enterMember();
                return BeginStatement::instance();
            }

            virtual State* onAlpha(cxxtools::Char c, SettingsReader& reader)
            {
                reader.buildToken(c);
                return BeginType::instance();
            }

            virtual State* onEof(SettingsReader& reader)
            {
                if(reader.depth() != 0)
                    this->syntaxError(reader.line());

                return this;
            }

            public:
                static State* instance()
                {
                    static OnCloseCurly _state;
                    return &_state;
                }
        };


        class BeginTypedValue : public State
        {
            virtual State* onSpace(cxxtools::Char c, SettingsReader& reader)
            {
                return this;
            }

            virtual State* onQuote(cxxtools::Char c, SettingsReader& reader)
            {
                return OnQuotedTypedValue::instance();
            }

            virtual State* onAlpha(cxxtools::Char c, SettingsReader& reader)
            {
                reader.buildToken(c);
                return OnTypedValue::instance();
            }

            public:
                static State* instance()
                {
                    static BeginTypedValue _state;
                    return &_state;
                }
        };


        class OnTypedValue : public State
        {
            virtual State* onSpace(cxxtools::Char c, SettingsReader& reader)
            {
                return EndTypedValue::instance();
            }

            virtual State* onCloseBrace(cxxtools::Char c, SettingsReader& reader)
            {
                reader.pushValue();
                return AfterValue::instance();
            }

            virtual State* onAlpha(cxxtools::Char c, SettingsReader& reader)
            {
                reader.buildToken(c);
                return this;
            }

            public:
                static State* instance()
                {
                    static OnTypedValue _state;
                    return &_state;
                }
        };


        class OnQuotedTypedValue : public OnQuotedValue
        {
            virtual State* onQuote(cxxtools::Char c, SettingsReader& reader)
            {
                return EndTypedValue::instance();
            }

            public:
                static State* instance()
                {
                    static OnQuotedTypedValue _state;
                    return &_state;
                }
        };


        class EndTypedValue : public OnTypedValue
        {
            virtual State* onAlpha(cxxtools::Char c, SettingsReader& reader)
            {
                this->syntaxError(reader.line());
                return this;
            }

            public:
                static State* instance()
                {
                    static EndTypedValue _state;
                    return &_state;
                }
        };

    public:
        SettingsReader(std::basic_istream<cxxtools::Char>& is)
        : state(0)
        , _beforeComment(0)
        , _current(0)
        , _is(&is)
        , _line(1)
        , _depth(0)
        , _isDotted(false)
        { }

        void parse(cxxtools::SerializationInfo& si);

    protected:
        void buildToken(cxxtools::Char c)
        { _token += c; }

        void beginSection()
        { _section.clear(); }

        void buildSection(cxxtools::Char c)
        { _section += c; }

        size_t depth() const
        { return _depth; }

        size_t line() const
        { return _line; }

        void enterMember();

        void leaveMember();

        void pushValue();

        void pushTypeName();

        void pushName();

        void beginComment()
        { _beforeComment = state; }

        State* endComment() const
        { return _beforeComment; }

        cxxtools::Char getEscaped();

    private:
        State* state;

        State* _beforeComment;

        cxxtools::SerializationInfo* _current;

        std::basic_istream<cxxtools::Char>* _is;

        size_t _line;

        size_t _depth;

        bool _isDotted;

        cxxtools::String _token;

        cxxtools::String _section;
};


static struct SettingsReaderInit
{
    SettingsReaderInit()
    {
        SettingsReader::OnComment::instance();
        SettingsReader::BeginStatement::instance();
        SettingsReader::OnSection::instance();
        SettingsReader::BeginType::instance();
        SettingsReader::AfterName::instance();
        SettingsReader::OnEqual::instance();
        SettingsReader::OnCurly::instance();
        SettingsReader::OnCloseCurly::instance();
        SettingsReader::OnQuotedValue::instance();
        SettingsReader::AfterQuotedValue::instance();
        SettingsReader::OnRValue::instance();
        SettingsReader::AfterRValue::instance();
        SettingsReader::BeginTypedValue::instance();
        SettingsReader::OnTypedValue::instance();
        SettingsReader::OnQuotedTypedValue::instance();
        SettingsReader::EndTypedValue::instance();
        SettingsReader::AfterValue::instance();
    }
} settingsReaderInit;

} // namespace cxxtools

#endif
