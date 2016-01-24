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
#include <cxxtools/serializationerror.h>
#include <cxxtools/log.h>

#include <vector>
#include <sstream>
#include <stdexcept>
#include <cctype>

#include <cstdlib>

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
        state = &HeaderParser::state_hfieldbody0;
    }
    else if (ch == ' ' || ch == '\t')
    {
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
        value.clear();
        state = &HeaderParser::state_hfieldbody_cr;
    }
    else if (ch == '\n')
    {
        value.clear();
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
        log_debug("header key=\"" << key << "\" value=\"" << value << '"');
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

struct TypeBoundary
{
    std::string type;
    std::string boundary;
};

TypeBoundary getTypeBoundary(const std::string& ct)
{
    static Regex mpr("^multipart/([^;]+).*;[[:space:]]*boundary=(\"([^\"]+)\"|([^][ ()<>@,;:\\\"/?=]+))");
    RegexSMatch sm;
    TypeBoundary ret;
    if (mpr.match(ct, sm))
    {
        ret.type = sm[1];
        ret.boundary = sm[sm.offsetEnd(3) > sm.offsetBegin(3) ? 3 : 4];
    }

    return ret;
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

void MimeHeader::setHeader(const std::string& key, const std::string& value, bool replace)
{
    if (replace)
        unsetHeader(key);
    addHeader(key, value);
}

void MimeHeader::unsetHeader(const std::string& key)
{
    for (HeadersType::size_type n = 0; n < headers.size(); )
    {
        if (compareIgnoreCase(key, headers[n].first) == 0)
            headers.erase(headers.begin() + n);
        else
            ++n;
    }
}

bool MimeHeader::isMultipart() const
{
    return !getTypeBoundary(getHeader("Content-Type")).boundary.empty();
}

void operator<<= (SerializationInfo& si, const MimeHeader& mh)
{
    si.setTypeName("mimeHeader");
    for (MimeHeader::HeadersType::const_iterator it = mh.headers.begin(); it != mh.headers.end(); ++it)
        si.addMember(it->first) <<= it->second;
}

void operator>>= (const SerializationInfo& si, MimeHeader& mh)
{
    std::string value;
    for (cxxtools::SerializationInfo::const_iterator it = si.begin(); it != si.end(); ++it)
    {
        it->getValue(value);
        mh.addHeader(it->name(), value);
    }
}

////////////////////////////////////////////////////////////////////////
// MimeEntity
//
MimeEntity::MimeEntity(const std::string& data)
{
    HeaderParser headerParser(*this);
    std::string::size_type pos;
    for (pos = 0; pos < data.size(); ++pos)
    {
        if (headerParser.parse(data[pos]))
        {
            ++pos;
            break;
        }
    }

    body.assign(data, pos, std::string::npos);
}

MimeEntity::MimeEntity(std::istream& in)
{
    HeaderParser headerParser(*this);

    // parse mime headers
    char ch;
    while (in.get(ch))
    {
        if (headerParser.parse(ch))
            break;
    }

    // copy rest of data to body
    while (in.get(ch))
        body += ch;
}

MimeEntity& MimeEntity::setContentTransferEncoding(ContentTransferEncoding cte)
{
    if (cte == MimeEntity::quotedPrintable)
        setHeader("Content-Transfer-Encoding", "quoted-printable");
    else if (cte == MimeEntity::base64)
        setHeader("Content-Transfer-Encoding", "base64");
    else
        unsetHeader("Content-Transfer-Encoding");

    return *this;
}

MimeEntity::ContentTransferEncoding MimeEntity::getContentTransferEncoding() const
{
    std::string contentTransferEncoding = getHeader("Content-Transfer-Encoding");
    return contentTransferEncoding == "base64"           ? MimeEntity::base64
         : contentTransferEncoding == "quoted-printable" ? MimeEntity::quotedPrintable
         : MimeEntity::none;
}

void operator<<= (SerializationInfo& si, const MimeEntity& mo)
{
    si.setTypeName("mimeEntity");
    if (mo.isMultipart())
    {
        si <<= MimeMultipart(mo);
    }
    else
    {
        si.addMember("header") <<= static_cast<const MimeHeader&>(mo);
        si.addMember("body") <<= mo.body;
    }
}

void operator>>= (const SerializationInfo& si, MimeEntity& mo)
{
    si.getMember("header") >>= static_cast<MimeHeader&>(mo);

    if (!si.getMember("body", mo.body))
    {
        std::vector<MimeEntity> parts;
        si.getMember("parts") >>= parts;

        std::ostringstream out;

        typedef std::vector<std::string> SpartsType;
        SpartsType sparts;
        std::string boundary = MimeMultipart::stringParts(parts, sparts);

        // print parts
        for (SpartsType::const_iterator it = sparts.begin(); it != sparts.end(); ++it)
            out << "--" << boundary << "\r\n"
                << *it;

        out << "--" << boundary << "--\r\n";

        mo.body = out.str();

        std::string type;
        si.getMember("type") >>= type;

        std::string contentType = type;
        contentType += ";boundary=\"";
        contentType += boundary;
        contentType += '"';
        mo.setHeader("Content-Type", contentType);

    }
}

////////////////////////////////////////////////////////////////////////
// MimeMultipart
//
void MimeMultipart::setType(Type type_)
{
    switch (type_)
    {
        case typeMixed: type = "mixed"; break;
        case typeAlternative: type = "alternative"; break;
        case typeDigest: type = "digest"; break;
        case typeParallel: type = "parallel"; break;
        case typeRelated: type = "related"; break;
    }
}

void MimeMultipart::partsFromBody(const std::string& body, std::string::size_type pos)
{
    TypeBoundary tb = getTypeBoundary(getHeader("Content-Type"));
    if (tb.boundary.empty())
        throw std::runtime_error("data is no mime multipart");

    // multipart
    type = tb.type;
    std::string boundary = "--";
    boundary += tb.boundary;
    pos = body.find(boundary, pos);

    if (pos != std::string::npos
        && body.size() > pos + boundary.size() + 2)
    {
        if (body[pos + boundary.size()] == '\n')
            pos += boundary.size() + 1;
        else if (body[pos + boundary.size()] == '\r' && body[pos + boundary.size() + 1] == '\n')
            pos += boundary.size() + 2;
        else
            return;

        std::string::size_type posEnd;
        while ((posEnd = body.find(boundary, pos)) != std::string::npos)
        {
            parts.resize(parts.size() + 1);
            MimeEntity& part = parts.back();

            HeaderParser headerParser(part);
            for (; pos < posEnd; ++pos)
            {
                if (headerParser.parse(body[pos]))
                {
                    ++pos;
                    break;
                }
            }

            std::string contentTransferEncoding = part.getHeader("Content-Transfer-Encoding");

            if (contentTransferEncoding == "base64")
                part.setBody(Base64Codec::decode(body.c_str() + pos, posEnd - pos));
            else if (contentTransferEncoding == "quoted-printable")
                part.setBody(QuotedPrintableCodec::decode(body.c_str() + pos, posEnd - pos));
            else
                part.getBody().assign(body, pos, posEnd - pos);

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

std::string MimeMultipart::stringParts(const std::vector<MimeEntity>& parts, std::vector<std::string>& sparts)
{
    for (MimeMultipart::PartsType::const_iterator pit = parts.begin(); pit != parts.end(); ++pit)
    {
        std::ostringstream out;
        out << *pit;
        sparts.push_back(out.str());
    }

    std::string boundary(10, '-');
    while (true)
    {
        unsigned r = static_cast<unsigned>(std::rand());
        while (r > 0)
        {
            boundary += '0' + r%10;
            r /= 10;
        }

        for (std::vector<std::string>::const_iterator it = sparts.begin(); it != sparts.end(); ++it)
            if (it->find(boundary) != std::string::npos)
                continue;
        break;
    }

    return boundary;
}

MimeMultipart::MimeMultipart(const std::string& data)
{
    HeaderParser headerParser(*this);
    std::string::size_type pos;
    for (pos = 0; pos < data.size(); ++pos)
    {
        if (headerParser.parse(data[pos]))
        {
            ++pos;
            break;
        }
    }

    partsFromBody(data, pos);
}

MimeMultipart::MimeMultipart(const MimeEntity& mimeEntity)
    : MimeHeader(mimeEntity)
{
    partsFromBody(mimeEntity.getBody());
}

MimeEntity& MimeMultipart::addObject(const std::string& data, const std::string& contentType,
    ContentTransferEncoding contentTransferEncoding)
{
    log_debug("add part " << data.size() << " bytes, contentType \""
            << contentType << "\" content transfer encoding " << contentTransferEncoding);

    MimeEntity& mimeEntity = addObject();
    mimeEntity.setContentType(contentType);
    mimeEntity.setContentTransferEncoding(contentTransferEncoding);
    mimeEntity.getBody() = data;
    return mimeEntity;
}

MimeEntity& MimeMultipart::addObject(std::istream& in, const std::string& contentType,
    ContentTransferEncoding contentTransferEncoding)
{
    log_debug("add part from stream, contentType \""
            << contentType << "\" content transfer encoding " << contentTransferEncoding);

    MimeEntity& mimeEntity = addObject();
    mimeEntity.setContentType(contentType);
    mimeEntity.setContentTransferEncoding(contentTransferEncoding);

    std::ostringstream body;
    body << in.rdbuf();
    mimeEntity.getBody() = body.str();
    return mimeEntity;
}

MimeEntity& MimeMultipart::addObject(const MimeMultipart& mimeMultipart)
{
    // build string parts
    typedef std::vector<std::string> SpartsType;
    SpartsType sparts;
    std::string boundary = stringParts(mimeMultipart.parts, sparts);

    // build content type header
    std::string contentType = "multipart/";
    contentType += mimeMultipart.type;
    contentType += "; boundary=\"";
    contentType += boundary;
    contentType += '"';

    MimeEntity& mimeEntity = addObject();
    mimeEntity.setContentType(contentType);

    // build body
    for (SpartsType::const_iterator it = sparts.begin(); it != sparts.end(); ++it)
        mimeEntity << "--" << boundary << "\r\n"
                   << *it;

    mimeEntity << "--" << boundary << "--\r\n";

    return mimeEntity;


}


std::ostream& operator<< (std::ostream& out, const MimeHeader& mimeHeaders)
{
    for (MimeEntity::HeadersType::const_iterator it = mimeHeaders.headers.begin();
             it != mimeHeaders.headers.end(); ++it)
        out << it->first << ": " << it->second << "\r\n";
    out << "\r\n";

    return out;
}

std::ostream& operator<< (std::ostream& out, const MimeEntity& mimePart)
{
    // print headers
    out << static_cast<const MimeHeader&>(mimePart);

    // encode data
    std::string contentTransferEncoding = mimePart.getHeader("Content-Transfer-Encoding");
    if (contentTransferEncoding == "quoted-printable")
    {
        QuotedPrintable_ostream enc(out);
        enc << mimePart.getBody();
        enc.flush();
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
    std::string boundary = MimeMultipart::stringParts(mime.parts, sparts);

    // print headers
    out << "MIME-Version: 1.0\r\n"
           "Content-Type: multipart/" << mime.type << "; boundary=\"" << boundary << "\"\r\n";
    MimeHeader mh = mime;
    mh.unsetHeader("MIME-Version");
    mh.unsetHeader("Content-Type");
    out << mh;
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
    si.setTypeName("mimeMultipart");
    si.addMember("header") <<= static_cast<const MimeHeader&>(mm);
    si.addMember("type") <<= ("multipart/" + mm.type);
    si.addMember("parts") <<= mm.parts;
}

void operator>>= (const SerializationInfo& si, MimeMultipart& mm)
{
    si.getMember("header") >>= static_cast<MimeHeader&>(mm);

    std::string type;
    if (si.getMember("type", type))
    {
        if (type.compare(0, 10, "multipart/") != 0)
            throw std::runtime_error("object is no mime multipart");
        mm.type.assign(type, 11, std::string::npos);
    }
    else
    {
        static Regex mpr("multipart/([^;]+)");
        RegexSMatch sm;

        std::string contentType = mm.getHeader("Content-Type");
        if (!mpr.match(contentType, sm))
            throw std::runtime_error("object is no mime multipart");

        mm.type = sm[1];
    }

    si.getMember("parts") >>= mm.parts;
}

}
