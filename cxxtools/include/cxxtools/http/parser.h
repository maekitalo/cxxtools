/*
 * Copyright (C) 2009 by Marc Boris Duerner, Tommi Maekitalo
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

#ifndef cxxtools_Http_Parser_h
#define cxxtools_Http_Parser_h

#include <cxxtools/http/api.h>
#include <string>
#include <iostream>

namespace cxxtools {

namespace http {

class MessageHeader;

class CXXTOOLS_HTTP_API HeaderParser
{
    public:
        class CXXTOOLS_HTTP_API Event
        {
            public:
                virtual ~Event() {}
                virtual void onMethod(const std::string& method);
                virtual void onUrl(const std::string& url);
                virtual void onUrlParam(const std::string& q);
                virtual void onHttpVersion(unsigned major, unsigned minor);
                virtual void onKey(const std::string& key);
                virtual void onValue(const std::string& value);
                virtual void onHttpReturn(unsigned ret, const std::string& text);
                virtual void onEnd();
        };

        class CXXTOOLS_HTTP_API MessageHeaderEvent : public Event
        {
                MessageHeader& _header;
                std::string _key;

            public:
                explicit MessageHeaderEvent(MessageHeader& header)
                    : _header(header)
                    { }

                virtual void onHttpVersion(unsigned major, unsigned minor);
                virtual void onKey(const std::string& key);
                virtual void onValue(const std::string& value);
        };

    private:
        typedef void (HeaderParser::*state_type)(char);

        void state_cmd0(char ch);
        void state_cmd(char ch);
        void state_url0(char ch);
        void state_url(char ch);
        void state_urlesc(char ch);
        void state_qparam(char ch);
        void state_protocol0(char ch);
        void state_protocol(char ch);
        void state_version0(char ch);
        void state_version_major(char ch);
        void state_version_major_e(char ch);
        void state_version_minor(char ch);
        void state_end0(char ch);
        void state_h0(char ch);
        void state_hcr(char ch);
        void state_hfieldname(char ch);
        void state_hfieldnamespace(char ch);
        void state_hfieldbody0(char ch);
        void state_hfieldbody(char ch);
        void state_hfieldbody_cr(char ch);
        void state_hfieldbody_crlf(char ch);
        void state_hend_cr(char ch);

        void state_cl_protocol0(char ch);
        void state_cl_protocol(char ch);
        void state_cl_version0(char ch);
        void state_cl_version_major(char ch);
        void state_cl_version_major_e(char ch);
        void state_cl_version_minor(char ch);
        void state_cl_httpresult0(char ch);
        void state_cl_httpresult(char ch);
        void state_cl_httpresulttext(char ch);
        void state_cl_httpresult_cr(char ch);

        void state_end(char ch);
        void state_error(char ch);

        state_type state;
        Event& ev;

        std::string token;
        unsigned value;

    public:
        HeaderParser(Event& ev_, bool client)
            : state(client ? &HeaderParser::state_cl_protocol0 : &HeaderParser::state_cmd0),
              ev(ev_)
            { }

        /// parse as many characters as available in buffer without blocking
        std::size_t advance(std::streambuf& sb);

        std::size_t advance(std::istream& is)
        { return advance(*is.rdbuf()); }

        /// parses a single character and returns true, if message is finished
        bool parse(char ch)
        {
            (this->*state)(ch);
            return state == &HeaderParser::state_end || state == &HeaderParser::state_error;
        }

        bool end() const    { return state == &HeaderParser::state_end
                                || state == &HeaderParser::state_error; }
        bool fail() const   { return state == &HeaderParser::state_error; }

        void reset(bool client)
        {
            state = (client ? &HeaderParser::state_cl_protocol0 : &HeaderParser::state_cmd0);
        }
};

} // namespace http

} // namespace cxxtools

#endif
