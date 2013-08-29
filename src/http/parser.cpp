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

#include "parser.h"
#include <cxxtools/http/messageheader.h>
#include <cxxtools/log.h>
#include <cctype>
#include <algorithm>
#include <string.h>

log_define("cxxtools.http.parser")

namespace cxxtools {

namespace http {

    namespace
    {
        std::string chartoprint(char ch)
        {
            const static char hex[] = "0123456789abcdef";
            if (std::isprint(ch))
                return std::string(1, '\'') + ch + '\'';
            else
                return std::string("'\\x") + hex[(ch >> 4) & 0xf] + hex[ch & 0xf] + '\'';
        }

        inline bool istokenchar(char ch)
        {
            static const char s[] = "\"(),/:;<=>?@[\\]{}";
            return std::isalpha(ch) || std::binary_search(s, s + sizeof(s) - 1, ch);
        }

        inline bool isHexDigit(char ch)
        {
            return (ch >= '0' && ch <= '9')
                || (ch >= 'A' && ch <= 'Z')
                || (ch >= 'a' && ch <= 'z');
        }

        inline unsigned valueOfHexDigit(char ch)
        {
            return ch >= '0' && ch <= '9' ? ch - '0'
                 : ch >= 'a' && ch <= 'z' ? ch - 'a' + 10
                 : ch >= 'A' && ch <= 'Z' ? ch - 'A' + 10
                 : 0;
        }
    }

    void HeaderParser::Event::onMethod(const std::string& method)
    {
    }

    void HeaderParser::Event::onUrl(const std::string& url)
    {
    }

    void HeaderParser::Event::onUrlParam(const std::string& q)
    {
    }

    void HeaderParser::Event::onHttpVersion(unsigned major, unsigned minor)
    {
    }

    void HeaderParser::Event::onKey(const std::string& key)
    {
    }

    void HeaderParser::Event::onValue(const std::string& value)
    {
    }

    void HeaderParser::Event::onHttpReturn(unsigned ret, const std::string& text)
    {
    }

    void HeaderParser::Event::onEnd()
    {
    }

    void HeaderParser::MessageHeaderEvent::onHttpVersion(unsigned major, unsigned minor)
    {
         _header.httpVersion(major, minor);
    }

    void HeaderParser::MessageHeaderEvent::onKey(const std::string& key)
    {
        strncpy(_key, key.c_str(), MessageHeader::MAXHEADERSIZE);
    }

    void HeaderParser::MessageHeaderEvent::onValue(const std::string& value)
    {
        _header.addHeader(_key, value.c_str());
    }

    std::size_t HeaderParser::advance(std::streambuf& sb)
    {
        std::size_t ret = 0;

        while (sb.in_avail() > 0)
        {
            ++ret;
            if (parse(sb.sbumpc()))
                return ret;
        }

        return ret;
    }

    void HeaderParser::state_cmd0(char ch)
    {
        if (istokenchar(ch))
        {
            token.reserve(32);
            token = ch;
            state = &HeaderParser::state_cmd;
            return;
        }
        else if (ch != ' ' && ch != '\t')
        {
            log_warn("invalid character " << chartoprint(ch) << " in method");
            state = &HeaderParser::state_error;
            return;
        }
        else
        {
            state = &HeaderParser::state_cmd;
            return;
        }
    }

    void HeaderParser::state_cmd(char ch)
    {
        if (istokenchar(ch))
        {
            token += ch;
            return;
        }
        else if (ch == ' ')
        {
            log_debug("method=" << token);
            ev.onMethod(token);
            state = &HeaderParser::state_url0;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in method");
            state = &HeaderParser::state_error;
            return;
        }
    }

    void HeaderParser::state_url0(char ch)
    {
        if (ch == ' ' || ch == '\t')
        {
            return;
        }
        else if (ch == '/' || ch == '*')
        {
            token.reserve(32);
            token = ch;
            state = &HeaderParser::state_url;
            return;
        }
        else if (std::isalpha(ch))
        {
            token.reserve(32);
            token = ch;
            state = &HeaderParser::state_uri_protocol;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in url");
            state = &HeaderParser::state_error;
            return;
        }
    }

    void HeaderParser::state_uri_protocol(char ch)
    {
        if (std::isalpha(ch))
        {
        }
        else if (ch == ':')
        {
            token.clear();
            state = &HeaderParser::state_uri_protocol_e;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in url");
            state = &HeaderParser::state_error;
        }
    }

    void HeaderParser::state_uri_protocol_e(char ch)
    {
        if (token.size() < 2 && ch == '/')
        {
            token += ch;
        }
        else if (token.size() == 2 && std::isalpha(ch))
        {
            token = ch;
            state = &HeaderParser::state_uri_host;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in url");
            state = &HeaderParser::state_error;
        }
    }

    void HeaderParser::state_uri_host(char ch)
    {
        if (std::isalnum(ch) || ch == '.' || ch == ':' || ch == '[' || ch == ']')
        {
        }
        else if (ch == '/')
        {
            token = ch;
            state = &HeaderParser::state_url;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in url");
            state = &HeaderParser::state_error;
        }
    }

    void HeaderParser::state_url(char ch)
    {
        if (ch == '?')
        {
            log_debug("url=" << token);
            ev.onUrl(token);
            token.clear();
            token.reserve(32);
            state = &HeaderParser::state_qparam;
            return;
        }
        else if (ch == ' ' || ch == '\t')
        {
            log_debug("url=" << token);
            ev.onUrl(token);
            token.clear();
            token.reserve(32);
            state = &HeaderParser::state_protocol0;
            return;
        }
        else if (ch == '+')
        {
            token += ' ';
            return;
        }
        else if (ch == '%')
        {
            token += ch;
            state = &HeaderParser::state_urlesc;
            return;
        }
        else if (ch > ' ')
        {
            token += ch;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in url");
            state = &HeaderParser::state_error;
            return;
        }
    }

    void HeaderParser::state_urlesc(char ch)
    {
        if (isHexDigit(ch))
        {
            if (token.size() >= 2 && token[token.size() - 2] == '%')
            {
                unsigned v = (valueOfHexDigit(token[token.size() - 1]) << 4) | valueOfHexDigit(ch);
                if (v == 0)
                {
                    log_warn("%00 not allowed in url");
                    state = &HeaderParser::state_error;
                    return;
                }

                token[token.size() - 2] = static_cast<char>(v);
                token.resize(token.size() - 1);
                state = &HeaderParser::state_url;
                return;
            }
            else
            {
                token += ch;
                return;
            }
        }
        else
        {
            log_warn("invalid hex digit " << chartoprint(ch) << " in url");
            state = &HeaderParser::state_error;
            return;
        }
    }

    void HeaderParser::state_qparam(char ch)
    {
        if (ch == ' ' || ch == '\t')
        {
            log_debug("queryString=" << token);
            ev.onUrlParam(token);
            token.clear();
            token.reserve(32);
            state = &HeaderParser::state_protocol0;
            return;
        }
        else
        {
            token += ch;
            return;
        }
    }

    void HeaderParser::state_protocol0(char ch)
    {
        if (ch == ' ' || ch == '\t')
        {
            return;
        }
        else if (std::isalpha(ch))
        {
            token.reserve(32);
            token = ch;
            state = &HeaderParser::state_protocol;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in http protocol field");
            state = &HeaderParser::state_error;
            return;
        }
    }

    void HeaderParser::state_protocol(char ch)
    {
        if (ch == ' ' || ch == '\t' || ch == '/')
        {
            if (token != "HTTP")
            {
                log_warn("invalid protocol " << token << " in http protocol field");
                state = &HeaderParser::state_error;
                return;
            }
            else
            {
                state = (ch == '/' ? &HeaderParser::state_version_major : &HeaderParser::state_version0);
                return;
            }
        }
        else if (std::isalpha(ch))
        {
            token += std::toupper(ch);
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in http protocol field");
            state = &HeaderParser::state_error;
            return;
        }
    }

    void HeaderParser::state_version0(char ch)
    {
        if (ch == ' ' || ch == '\t')
        {
            return;
        }
        else if (ch == '/')
        {
            state = &HeaderParser::state_version_major;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in http version field");
            state = &HeaderParser::state_error;
            return;
        }
    }

    void HeaderParser::state_version_major(char ch)
    {
        if (ch == ' ' || ch == '\t')
        {
            return;
        }
        else if (ch == '1')
        {
            state = &HeaderParser::state_version_major_e;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in http version field");
            state = &HeaderParser::state_error;
            return;
        }
    }

    void HeaderParser::state_version_major_e(char ch)
    {
        if (ch == ' ' || ch == '\t')
        {
            state = &HeaderParser::state_version_major_e;
            return;
        }
        else if (ch == '.')
        {
            state = &HeaderParser::state_version_minor;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in http version field");
            state = &HeaderParser::state_error;
            return;
        }
    }

    void HeaderParser::state_version_minor(char ch)
    {
        if (ch == ' ' || ch == '\t')
        {
            return;
        }
        else if (ch == '0' || ch == '1')
        {
            ev.onHttpVersion(1, ch - '0');
            state = &HeaderParser::state_end0;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in http version field");
            state = &HeaderParser::state_error;
            return;
        }
    }

    void HeaderParser::state_end0(char ch)
    {
        if (ch == '\n')
        {
            state = &HeaderParser::state_h0;
            return;
        }
        else if (ch == ' ' || ch == '\t' || ch == '\r')
        {
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in http request line");
            state = &HeaderParser::state_error;
            return;
        }
    }

    void HeaderParser::state_h0(char ch)
    {
        if (ch == ' ' || ch == '\t')
        {
            return;
        }
        else if (ch > 32 && ch < 127)
        {
            token.reserve(32);
            token = ch;
            state = &HeaderParser::state_hfieldname;
            return;
        }
        else if (ch == '\r')
        {
            state = &HeaderParser::state_hcr;
            return;
        }
        else if (ch == '\n')
        {
            ev.onEnd();
            state = &HeaderParser::state_end;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in http header");
            state = &HeaderParser::state_error;
            return;
        }
    }

    void HeaderParser::state_hcr(char ch)
    {
        if (ch == '\n')
        {
            ev.onEnd();
            state = &HeaderParser::state_end;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in http header");
            state = &HeaderParser::state_error;
            return;
        }
    }

    void HeaderParser::state_hfieldname(char ch)
    {
        if (ch == ':')
        {
            ev.onKey(token);
            state = &HeaderParser::state_hfieldbody0;
            return;
        }
        else if (ch == ' ' || ch == '\t')
        {
            ev.onKey(token);
            state = &HeaderParser::state_hfieldnamespace;
            return;
        }
        else if (ch > 32 && ch < 127)
        {
            token += ch;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in fieldname");
            state = &HeaderParser::state_error;
            return;
        }
    }

    void HeaderParser::state_hfieldnamespace(char ch)
    {
        if (ch == ':')
        {
            state = &HeaderParser::state_hfieldbody0;
            return;
        }
        else if (ch == ' ' || ch == '\t')
        {
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in fieldname");
            state = &HeaderParser::state_error;
            return;
        }
    }

    void HeaderParser::state_hfieldbody0(char ch)
    {
        if (ch == '\r')
        {
            state = &HeaderParser::state_hfieldbody_cr;
            return;
        }
        else if (ch == '\n')
        {
            state = &HeaderParser::state_hfieldbody_crlf;
            return;
        }
        else if (std::isspace(ch))
        {
            return;
        }
        else if (!std::isspace(ch))
        {
            token.reserve(32);
            token = ch;
            state = &HeaderParser::state_hfieldbody;
            return;
        }
    }

    void HeaderParser::state_hfieldbody(char ch)
    {
        if (ch == '\r')
        {
            state = &HeaderParser::state_hfieldbody_cr;
            return;
        }
        else if (ch == '\n')
        {
            state = &HeaderParser::state_hfieldbody_crlf;
            return;
        }
        else
        {
            token += ch;
            return;
        }
    }

    void HeaderParser::state_hfieldbody_cr(char ch)
    {
        if (ch == '\n')
        {
            state = &HeaderParser::state_hfieldbody_crlf;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in fieldbody");
            state = &HeaderParser::state_error;
            return;
        }

    }

    void HeaderParser::state_hfieldbody_crlf(char ch)
    {
        if (ch == '\r')
        {
            ev.onValue(token);
            state = &HeaderParser::state_hend_cr;
            return;
        }
        else if (ch == '\n')
        {
            ev.onValue(token);
            ev.onEnd();
            state = &HeaderParser::state_end;
            return;
        }
        else if (ch == ' ' || ch == '\t')
        {
            token += ch;
            state = &HeaderParser::state_hfieldbody;
            return;
        }
        else if (ch > 32 && ch < 127)
        {
            ev.onValue(token);
            token.reserve(32);
            token = ch;
            state = &HeaderParser::state_hfieldname;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in fieldbody");
            state = &HeaderParser::state_error;
            return;
        }
    }

    void HeaderParser::state_hend_cr(char ch)
    {
        if (ch == '\n')
        {
            ev.onEnd();
            state = &HeaderParser::state_end;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in fieldbody");
            state = &HeaderParser::state_error;
            return;
        }
    }

    void HeaderParser::state_cl_protocol0(char ch)
    {
        if (ch == ' ' || ch == '\t')
        {
            return;
        }
        else if (std::isalpha(ch))
        {
            token.reserve(32);
            token = ch;
            state = &HeaderParser::state_cl_protocol;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in http protocol field");
            state = &HeaderParser::state_error;
            return;
        }
    }

    void HeaderParser::state_cl_protocol(char ch)
    {
        if (ch == ' ' || ch == '\t' || ch == '/')
        {
            if (token != "HTTP")
            {
                log_warn("invalid protocol " << token << " in http protocol field");
                state = &HeaderParser::state_error;
                return;
            }
            else
            {
                state = (ch == '/' ? &HeaderParser::state_cl_version_major : &HeaderParser::state_cl_version0);
                return;
            }
        }
        else if (std::isalpha(ch))
        {
            token += std::toupper(ch);
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in http protocol field");
            state = &HeaderParser::state_error;
            return;
        }
    }

    void HeaderParser::state_cl_version0(char ch)
    {
        if (ch == ' ' || ch == '\t')
        {
            return;
        }
        else if (ch == '/')
        {
            state = &HeaderParser::state_cl_version_major;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in http version field");
            state = &HeaderParser::state_error;
            return;
        }
    }

    void HeaderParser::state_cl_version_major(char ch)
    {
        if (ch == ' ' || ch == '\t')
        {
            return;
        }
        else if (ch == '1')
        {
            state = &HeaderParser::state_cl_version_major_e;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in http version field");
            state = &HeaderParser::state_error;
            return;
        }
    }

    void HeaderParser::state_cl_version_major_e(char ch)
    {
        if (ch == ' ' || ch == '\t')
        {
            state = &HeaderParser::state_cl_version_major_e;
            return;
        }
        else if (ch == '.')
        {
            state = &HeaderParser::state_cl_version_minor;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in http version field");
            state = &HeaderParser::state_error;
            return;
        }
    }

    void HeaderParser::state_cl_version_minor(char ch)
    {
        if (ch == ' ' || ch == '\t')
        {
            return;
        }
        else if (ch == '0' || ch == '1')
        {
            ev.onHttpVersion(1, ch - '0');
            state = &HeaderParser::state_cl_httpresult0;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in http result");
            state = &HeaderParser::state_error;
            return;
        }
    }

    void HeaderParser::state_cl_httpresult0(char ch)
    {
        if (ch == ' ' || ch == '\t')
        {
            return;
        }
        else if (std::isdigit(ch))
        {
            value = (ch - '0');
            state = &HeaderParser::state_cl_httpresult;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in http result");
            state = &HeaderParser::state_error;
            return;
        }
    }

    void HeaderParser::state_cl_httpresult(char ch)
    {
        if (std::isdigit(ch))
        {
            value = value * 10 + (ch - '0');
            return;
        }
        else if (ch == ' ' || ch == '\t')
        {
            token.clear();
            token.reserve(32);
            state = &HeaderParser::state_cl_httpresulttext;
        }
    }

    void HeaderParser::state_cl_httpresulttext(char ch)
    {
        if (ch == '\r')
        {
            ev.onHttpReturn(value, token);
            state = &HeaderParser::state_cl_httpresult_cr;
            return;
        }
        else if (ch == '\n')
        {
            ev.onHttpReturn(value, token);
            state = &HeaderParser::state_h0;
            return;
        }
        else if (token.empty() && (ch == ' ' || ch == '\t'))
        {
            return;
        }
        else
        {
            token += ch;
            return;
        }
    }

    void HeaderParser::state_cl_httpresult_cr(char ch)
    {
        if (ch == '\n')
        {
            state = &HeaderParser::state_h0;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in requestheader");
            state = &HeaderParser::state_error;
            return;
        }
    }

    void HeaderParser::state_end(char ch)
    {
        return;
    }

    void HeaderParser::state_error(char ch)
    {
        return;
    }

}

}
