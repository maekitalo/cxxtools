/*
 * Copyright (C) 2007 Tommi Maekitalo
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.    See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA    02110-1301    USA
 */

#include <cxxtools/mime.h>
#include <cxxtools/base64stream.h>
#include <cxxtools/base64codec.h>
#include <cxxtools/quotedprintablestream.h>
#include <cxxtools/quotedprintablecodec.h>
#include <cxxtools/regex.h>
#include <cxxtools/serializationinfo.h>
#include <cxxtools/log.h>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <cctype>

log_define("cxxtools.mime")

namespace cxxtools
{
namespace
{

int compareIgnoreCase(const std::string& s1, const std::string& s2)
{
    std::string::const_iterator it1 = s1.begin();
    std::string::const_iterator it2 = s2.begin();
    while (it1 != s1.end() && it2 != s2.end())
    {
        if (*it1 != *it2)
        {
            char c1 = std::toupper(*it1);
            char c2 = std::toupper(*it2);
            if (c1 < c2)
                return -1;
            else if (c2 < c1)
                return 1;
        }
        ++it1;
        ++it2;
    }

    return it1 != s1.end() ? 1
         : it2 != s2.end() ? -1 : 0;
}

std::string chartoprint(char ch)
{
    const static char hex[] = "0123456789abcdef";
    if (std::isprint(ch))
        return std::string(1, '\'') + ch + '\'';
    else
        return std::string("'\\x") + hex[(ch >> 4) & 0xf] + hex[ch & 0xf] + '\'';
}

class HeaderParser
{
private:
    typedef void (HeaderParser::*state_type)(char);

    void state_h0(char ch);
    void state_hcr(char ch);
    void state_hfieldname(char ch);
    void state_hfieldnamespace(char ch);
    void state_hfieldbody0(char ch);
    void state_hfieldbody(char ch);
    void state_hfieldbody_cr(char ch);
    void state_hfieldbody_crlf(char ch);
    void state_hend_cr(char ch);

    void state_end(char ch);

    state_type state;
    MimeHeader& mm;

    std::string key;
    std::string value;

public:
    explicit HeaderParser(MimeHeader& mm_)
        : state(&HeaderParser::state_h0),
            mm(mm_)
        { }

    /// parses a single character and returns true, if message is finished
    bool parse(char ch)
    {
        (this->*state)(ch);
        return state == &HeaderParser::state_end;
    }
};

void HeaderParser::state_h0(char ch)
{
    if (ch == ' ' || ch == '\t')
    {
    }
    else if (ch > 32 && ch < 127)
    {
        key.reserve(32);
        key = ch;
        state = &HeaderParser::state_hfieldname;
    }
    else if (ch == '\r')
    {
        state = &HeaderParser::state_hcr;
    }
    else if (ch == '\n')
    {
        state = &HeaderParser::state_end;
    }
    else
    {
        throw std::runtime_error("invalid character " + chartoprint(ch) + " in mime header");
    }
}

void HeaderParser::state_hcr(char ch)
{
    if (ch == '\n')
    {
        state = &HeaderParser::state_end;
    }
    else
    {
        throw std::runtime_error("invalid character " + chartoprint(ch) + " in mime header");
    }
}

void HeaderParser::state_hfieldname(char ch)
{
    if (ch == ':')
    {
        log_debug("key=\"" << key << '"');
        state = &HeaderParser::state_hfieldbody0;
    }
    else if (ch == ' ' || ch == '\t')
    {
        log_debug("key=\"" << key << '"');
        state = &HeaderParser::state_hfieldnamespace;
    }
    else if (ch > 32 && ch < 127)
    {
        key += ch;
    }
    else
    {
        throw std::runtime_error("invalid character " + chartoprint(ch) + " in fieldname");
    }
}

void HeaderParser::state_hfieldnamespace(char ch)
{
    if (ch == ':')
    {
        state = &HeaderParser::state_hfieldbody0;
    }
    else if (ch == ' ' || ch == '\t')
    {
    }
    else
    {
        throw std::runtime_error("invalid character " + chartoprint(ch) + " in fieldname");
    }
}

void HeaderParser::state_hfieldbody0(char ch)
{
    if (ch == '\r')
    {
        state = &HeaderParser::state_hfieldbody_cr;
    }
    else if (ch == '\n')
    {
        state = &HeaderParser::state_hfieldbody_crlf;
    }
    else if (!std::isspace(ch))
    {
        value.reserve(32);
        value = ch;
        state = &HeaderParser::state_hfieldbody;
    }
}

void HeaderParser::state_hfieldbody(char ch)
{
    if (ch == '\r')
    {
        state = &HeaderParser::state_hfieldbody_cr;
    }
    else if (ch == '\n')
    {
        state = &HeaderParser::state_hfieldbody_crlf;
    }
    else
    {
        value += ch;
    }
}

void HeaderParser::state_hfieldbody_cr(char ch)
{
    if (ch == '\n')
    {
        state = &HeaderParser::state_hfieldbody_crlf;
    }
    else
    {
        throw std::runtime_error("invalid character " + chartoprint(ch) + " in field body");
    }

}

void HeaderParser::state_hfieldbody_crlf(char ch)
{
    if (ch == ' ' || ch == '\t')
    {
        // continuation line
        value += ch;
        state = &HeaderParser::state_hfieldbody;
    }
    else
    {
        log_debug(static_cast<const void*>(this) << " header key=\"" << key << "\" value=\"" << value << '"');
        mm.setHeader(key, value);

        if (ch == '\r')
        {
            state = &HeaderParser::state_hend_cr;
        }
        else if (ch == '\n')
        {
            state = &HeaderParser::state_end;
        }
        else if (ch > 32 && ch < 127)
        {
            key.reserve(32);
            key = ch;
            state = &HeaderParser::state_hfieldname;
        }
        else
        {
            throw std::runtime_error("invalid character " + chartoprint(ch) + " in field body");
        }
    }
}

void HeaderParser::state_hend_cr(char ch)
{
    if (ch == '\n')
    {
        state = &HeaderParser::state_end;
    }
    else
    {
        throw std::runtime_error("invalid character " + chartoprint(ch) + " in field body");
    }
}

void HeaderParser::state_end(char /*ch*/)
{
}

}

////////////////////////////////////////////////////////////////////////
// MimeHeader
//

std::string MimeHeader::getHeader(const std::string& key, const std::string& def) const
{
    for (HeadersType::const_iterator it = headers.begin(); it != headers.end(); ++it)
        if (compareIgnoreCase(key, it->first) == 0)
            return it->second;
    return def;
}

bool MimeHeader::isMultipart() const
{
    static Regex mpr("multipart/.*boundary=\"(.*)\"");
    std::string contentType = getHeader("Content-Type");
    return mpr.match(contentType);
}

void operator<<= (SerializationInfo& si, const MimeHeader& mh)
{
    for (MimeHeader::HeadersType::const_iterator it = mh.headers.begin(); it != mh.headers.end(); ++it)
        si.addMember(it->first) <<= it->second;
}

////////////////////////////////////////////////////////////////////////
// Mimepart
//
Mimepart::Mimepart(const std::string& contentType_,
                                     ContentTransferEncoding contentTransferEncoding)
{
    setHeader("Content-Type", contentType_);

    if (contentTransferEncoding == Mimepart::quotedPrintable)
        setHeader("Content-Transfer-Encoding", "quoted-printable");
    else if (contentTransferEncoding == Mimepart::base64)
        setHeader("Content-Transfer-Encoding", "base64");
}

void Mimepart::addData(std::istream& in)
{
    std::ostringstream data;
    data << in.rdbuf();
    body += data.str();
}

void operator<<= (SerializationInfo& si, const Mimepart& mp)
{
    if (mp.isMultipart())
    {
        si <<= MimeMultipart(mp);
    }
    else
    {
        si.addMember("header") <<= static_cast<const MimeHeader&>(mp);
        si.addMember("body") <<= mp.body;
    }
}

////////////////////////////////////////////////////////////////////////
// MimeMultipart
//
void MimeMultipart::partsFromBody(const std::string& body, std::string::size_type pos)
{
    log_debug("partsFromBody <" << body << '>');

    static Regex mpr("multipart/([^;]+);.*boundary=\"(.*)\"");
    RegexSMatch sm;

    std::string contentType = getHeader("Content-Type");
    if (!mpr.match(contentType, sm))
        throw std::runtime_error("data is no mime multipart");

    // multipart
    type = sm.get(1);
    std::string boundary = "--";
    boundary += sm.get(2);
    log_debug("boundary <" << boundary << "> found");
    pos = body.find(boundary, pos);

    if (pos != std::string::npos
        && body.size() > pos + boundary.size() + 2)
    {
        if (body[pos + boundary.size()] == '\n')
            pos += boundary.size() + 1;
        else if (body[pos + boundary.size()] == '\r' && body[pos + boundary.size() + 1] == '\n')
            pos += boundary.size() + 2;
        else
        {
            log_debug("boundary not delimited by CRLF or LF");
            return;
        }

        std::string::size_type posEnd;
        while ((posEnd = body.find(boundary, pos)) != std::string::npos)
        {
            log_debug("boundary at " << posEnd << " found");
            parts.resize(parts.size() + 1);
            Mimepart& part = parts.back();

            HeaderParser headerParser(part);
            for (; pos < posEnd; ++pos)
            {
                if (headerParser.parse(body[pos]))
                {
                    ++pos;
                    break;
                }
            }

            part.body.assign(body, pos, posEnd - pos);
            std::string contentTransferEncoding = part.getHeader("Content-Transfer-Encoding");
            if (contentTransferEncoding == "base64")
            {
                log_debug("decode base64 <" << part.body << '>');
                part.body = Base64Codec::decode(part.body);
            }
            else if (contentTransferEncoding == "quoted-printable")
                part.body = QuotedPrintableCodec::decode(part.body);

            pos = posEnd + boundary.size() + 2;
            if (body[posEnd + boundary.size()] == '\n')
                pos = posEnd + boundary.size() + 1;
            else if (body[posEnd + boundary.size()] == '\r' && body[posEnd + boundary.size() + 1] == '\n')
                pos = posEnd + boundary.size() + 2;
            else
            {
            log_debug("boundary not delimited by CRLF or LF");
            return;
        }

            if (body[posEnd + boundary.size()] == '-'
              && body[posEnd + boundary.size() + 1] == '-')
            {
                log_debug("end boundary found");
                break;
            }
        }
    }
}

MimeMultipart::MimeMultipart(const std::string& p)
{
    HeaderParser headerParser(*this);
    std::string::size_type pos;
    for (pos = 0; pos < p.size(); ++pos)
    {
        if (headerParser.parse(p[pos]))
        {
            ++pos;
            break;
        }
    }

    partsFromBody(p, pos);
}

MimeMultipart::MimeMultipart(const Mimepart& part)
    : MimeHeader(part)
{
    partsFromBody(part.getBody());
}

Mimepart& MimeMultipart::addPart(const std::string& data, const std::string& contentType,
    ContentTransferEncoding contentTransferEncoding)
{
    log_debug("add part " << data.size() << " bytes, contentType \""
            << contentType << "\" content transfer encoding " << contentTransferEncoding);

    parts.push_back(Mimepart(contentType, contentTransferEncoding));
    parts.back().setData(data);
    return parts.back();
}

Mimepart& MimeMultipart::addPart(std::istream& in, const std::string& contentType,
    ContentTransferEncoding contentTransferEncoding)
{
    log_debug("add part from stream, contentType \""
            << contentType << "\" content transfer encoding " << contentTransferEncoding);

    parts.push_back(Mimepart(contentType, contentTransferEncoding));
    std::ostringstream body;
    body << in.rdbuf();

    log_debug("part has " << body.str().size() << " bytes");

    parts.back().setData(body.str());
    return parts.back();
}

Mimepart& MimeMultipart::addTextFile(const std::string& contentType, const std::string& filename)
{
    std::ifstream in(filename.c_str());
    if (!in)
        throw std::runtime_error("cannot open file \"" + filename + '"');

    return addTextFile(contentType, filename, in);
}

Mimepart& MimeMultipart::addBinaryFile(const std::string& contentType, const std::string& filename)
{
    std::ifstream in(filename.c_str());
    if (!in)
        throw std::runtime_error("cannot open file \"" + filename + '"');

    return addBinaryFile(contentType, filename, in);
}

std::ostream& operator<< (std::ostream& out, const Mimepart& mimePart)
{
    // print headers
    for (Mimepart::HeadersType::const_iterator it = mimePart.headers.begin();
             it != mimePart.headers.end(); ++it)
        out << it->first << ": " << it->second << "\r\n";
    out << "\r\n";

    // encode data
    std::string contentTransferEncoding = mimePart.getHeader("Content-Transfer-Encoding");
    if (contentTransferEncoding == "quoted-printable")
    {
        QuotedPrintable_ostream enc(out);
        enc << mimePart.getBody();
    }
    else if (contentTransferEncoding == "base64")
    {
        Base64ostream enc(out);
        enc << mimePart.getBody();
        enc.terminate();
    }
    else
    {
        log_warn_if(!contentTransferEncoding.empty(), "unknown content transfer encoding \"" << contentTransferEncoding << '"');
        out << mimePart.getBody();
    }

    out << "\r\n";

    return out;
}

std::ostream& operator<< (std::ostream& out, const MimeMultipart& mime)
{
    log_debug("output " << mime.parts.size() << " parts");

    // build string parts
    typedef std::vector<std::string> SpartsType;
    SpartsType sparts;

    for (MimeMultipart::PartsType::const_iterator pit = mime.parts.begin(); pit != mime.parts.end(); ++pit)
    {
        std::ostringstream out;
        out << *pit;
        sparts.push_back(out.str());
    }

    // choose suitable boundary
    std::string boundary(10, '-');
    while (true)
    {
        unsigned r = static_cast<unsigned>(rand());
        while (r > 0)
        {
            boundary += '0' + r%10;
            r /= 10;
        }
        for (SpartsType::const_iterator it = sparts.begin(); it != sparts.end(); ++it)
            if (it->find(boundary) != std::string::npos)
                continue;
        break;
    }

    // print headers
    out << "MIME-Version: 1.0\r\n"
                 "Content-Type: multipart/" << mime.type << "; boundary=\"" << boundary << "\"\r\n";
    for (MimeMultipart::HeadersType::const_iterator it = mime.headers.begin();
             it != mime.headers.end(); ++it)
        out << it->first << ": " << it->second << "\r\n";
    out << "\r\n";

    // print parts
    for (SpartsType::const_iterator it = sparts.begin(); it != sparts.end(); ++it)
        out << "--" << boundary << "\r\n"
                << *it;

    out << "--" << boundary << "--\r\n";

    return out;
}

void operator<<= (SerializationInfo& si, const MimeMultipart& mm)
{
    si.addMember("header") <<= static_cast<const MimeHeader&>(mm);
    si.addMember("type") <<= ("multipart/" + mm.type);
    si.addMember("parts") <<= mm.parts;
}

}
