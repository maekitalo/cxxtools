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

#include <iosfwd>
#include <string>
#include <vector>
#include <utility>

namespace cxxtools
{
class Mimepart;
class MimeMultipart;
class SerializationInfo;

class MimeHeader
{
        friend std::ostream& operator<< (std::ostream& out, const Mimepart& mimePart);
        friend std::ostream& operator<< (std::ostream& out, const MimeMultipart& mime);
        friend void operator<<= (SerializationInfo& si, const MimeHeader& mh);

        typedef std::vector<std::pair<std::string, std::string> > HeadersType;
        HeadersType headers;

    public:
        /// returns the header value or a default value
        std::string getHeader(const std::string& key, const std::string& def = std::string()) const;

        /// Adds a header-line to the mime-object.
        void setHeader(const std::string& key, const std::string& value)
            { headers.push_back(HeadersType::value_type(key, value)); }

        /// Returns true, if the content type is multipart/*
        bool isMultipart() const;
};

class Mimepart : public MimeHeader
{
        friend std::ostream& operator<< (std::ostream& out, const Mimepart& mimePart);
        friend class MimeMultipart;
        friend void operator<<= (SerializationInfo& si, const Mimepart& mp);

    public:
        enum ContentTransferEncoding {
            none,
            quotedPrintable,
            base64
        };

    private:
        std::string body;

    public:
        Mimepart() { }
        explicit Mimepart(const std::string& contentType_,
                                            ContentTransferEncoding contentTransferEncoding_ = base64);

        void setData(const std::string& data)     { body = data; }
        void addData(const std::string& data)     { body += data; }
        void addData(std::istream& in);
        const std::string& getBody() const             { return body; }
};

class MimeMultipart : public MimeHeader
{
        friend std::ostream& operator<< (std::ostream& out, const MimeMultipart& mime);
        friend void operator<<= (SerializationInfo& si, const MimeMultipart& mm);

    public:
        typedef Mimepart::ContentTransferEncoding ContentTransferEncoding;

        enum Type {
                typeMixed,
                typeAlternative
        };

    private:
        typedef std::vector<Mimepart> PartsType;
        PartsType parts;

        std::string type;    // mixed or alternative

        void partsFromBody(const std::string& body, std::string::size_type pos = 0);

    public:
        /// parses a multipart object
        explicit MimeMultipart(const std::string& p);

        /// Make a MimeMultipart object from a Mimepart object
        MimeMultipart(const Mimepart& part);

        explicit MimeMultipart(Type type_ = typeMixed)
            : type(type_ == typeMixed ? "mixed" : "alternative")
        { }

        void setType(Type type_)
            { type = (type_ == typeMixed ? "mixed" : "alternative"); }
        void setType(const std::string& type_)
            { type = type; }

        /// Adds a empty part to the mime-object.
        Mimepart& addPart()
            { parts.resize(parts.size() + 1); return parts.back(); }

        /// Adds a part to the mime-object.
        Mimepart& addPart(const Mimepart& part)
            { parts.push_back(part); return parts.back(); }

        /// Adds a part to the mime-object. The data is passed as a std::string.
        Mimepart& addPart(const std::string& data, const std::string& contentType = "text/plain; charset=UTF-8",
            ContentTransferEncoding contentTransferEncoding = Mimepart::quotedPrintable);

        /// Adds a part to the mime-object. The data is read from a input stream.
        Mimepart& addPart(std::istream& in, const std::string& contentType = "text/plain; charset=UTF-8",
            ContentTransferEncoding contentTransferEncoding = Mimepart::quotedPrintable);

        /// Adds a text file. The data is passed as a std::string.
        Mimepart& addTextFile(const std::string& contentType, const std::string& filename, const std::string& data)
        {
            Mimepart& part = addPart(data, contentType, Mimepart::quotedPrintable);
            part.setHeader("Content-Disposition", "attachment; filename=" + filename);
            return part;
        }

        /// Adds a text file. The data is read from a istream
        Mimepart& addTextFile(const std::string& contentType, const std::string& filename, std::istream& in)
        {
            Mimepart& part = addPart(in, contentType, Mimepart::quotedPrintable);
            part.setHeader("Content-Disposition", "attachment; filename=" + filename);
            return part;
        }

        /// Adds a text file. The data is read from a file.
        Mimepart& addTextFile(const std::string& contentType, const std::string& filename);

        /// Adds a binary file. The data is passed as a std::string.
        Mimepart& addBinaryFile(const std::string& contentType, const std::string& filename, const std::string& data)
        {
            Mimepart& part = addPart(data, contentType, Mimepart::base64);
            part.setHeader("Content-Disposition", "attachment; filename=" + filename);
            return part;
        }

        /// Adds a binary file. The data is read from a istream
        Mimepart& addBinaryFile(const std::string& contentType, const std::string& filename, std::istream& in)
        {
            Mimepart& part = addPart(in, contentType, Mimepart::base64);
            part.setHeader("Content-Disposition", "attachment; filename=" + filename);
            return part;
        }

        /// Adds a binary file. The data is read from a file.
        Mimepart& addBinaryFile(const std::string& contentType, const std::string& filename);

};

std::ostream& operator<< (std::ostream& out, const Mimepart& mimePart);
std::ostream& operator<< (std::ostream& out, const MimeMultipart& mime);
}
