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
class MimeObject;
class MimeMultipart;
class SerializationInfo;

class MimeHeader
{
        friend std::ostream& operator<< (std::ostream& out, const MimeHeader& mimeHeader);
        friend void operator<<= (SerializationInfo& si, const MimeHeader& mh);

        typedef std::vector<std::pair<std::string, std::string> > HeadersType;
        HeadersType headers;

    public:
        /// returns the header value or a default value
        std::string getHeader(const std::string& key, const std::string& def = std::string()) const;

        /// Set a header-line to the mime-object.
        void setHeader(const std::string& key, const std::string& value, bool replace = true);

        /// Adds a header-line to the mime-object.
        void addHeader(const std::string& key, const std::string& value)
            { headers.push_back(HeadersType::value_type(key, value)); }

        void unsetHeader(const std::string& key);

        /// Returns true, if the content type is multipart/*
        bool isMultipart() const;
};

class MimeObject : public MimeHeader
{
        friend std::ostream& operator<< (std::ostream& out, const MimeObject& mimeObject);

    public:
        enum ContentTransferEncoding {
            none,
            quotedPrintable,
            base64
        };

    private:
        std::string body;

    public:
        MimeObject() { }
        explicit MimeObject(const std::string& data);

        const std::string& getBody() const       { return body; }
        std::string& getBody()                   { return body; }

        void setContentTransferEncoding(ContentTransferEncoding cte);
        ContentTransferEncoding getContentTransferEncoding() const;

        void setContentType(const std::string& ct)
            { setHeader("Content-Type", ct); }
        std::string getContentType() const
            { return getHeader("Content-Type"); }
};

class MimeMultipart : public MimeHeader
{
        friend std::ostream& operator<< (std::ostream& out, const MimeMultipart& mime);
        friend void operator<<= (SerializationInfo& si, const MimeMultipart& mm);

    public:
        typedef MimeObject::ContentTransferEncoding ContentTransferEncoding;

        enum Type {
                typeMixed,
                typeAlternative
        };

    private:
        typedef std::vector<MimeObject> PartsType;
        PartsType parts;

        std::string type;    // mixed or alternative

        void partsFromBody(const std::string& body, std::string::size_type pos = 0);

    public:
        /// parses a multipart object
        explicit MimeMultipart(const std::string& data);

        /// Make a MimeMultipart object from a MimeObject
        MimeMultipart(const MimeObject& mimeObject);

        explicit MimeMultipart(Type type_ = typeMixed)
            : type(type_ == typeMixed ? "mixed" : "alternative")
        { }

        void setType(Type type_)
            { type = (type_ == typeMixed ? "mixed" : "alternative"); }
        void setType(const std::string& type_)
            { type = type; }

        /// Adds a empty mime object and returns a reference to it.
        MimeObject& addObject()
            { parts.resize(parts.size() + 1); return parts.back(); }

        /// Adds a object to the mime-object.
        MimeObject& addObject(const MimeObject& mimeObject)
            { parts.push_back(mimeObject); return parts.back(); }

        /// Adds a object to the mime-object. The data is passed as a std::string.
        MimeObject& addObject(const std::string& data, const std::string& contentType = "text/plain; charset=UTF-8",
            ContentTransferEncoding contentTransferEncoding = MimeObject::quotedPrintable);

        /// Adds a object to the mime-object. The data is read from a input stream.
        MimeObject& addObject(std::istream& in, const std::string& contentType = "text/plain; charset=UTF-8",
            ContentTransferEncoding contentTransferEncoding = MimeObject::quotedPrintable);

        /// Adds a text file. The data is passed as a std::string.
        MimeObject& attachTextFile(const std::string& data, const std::string& filename, const std::string& contentType = "text/plain; charset=UTF-8")
        {
            MimeObject& mimeObject = addObject(data, contentType, MimeObject::quotedPrintable);
            mimeObject.addHeader("Content-Disposition", "attachment; filename=" + filename);
            return mimeObject;
        }

        /// Adds a text file. The data is read from a istream
        MimeObject& attachTextFile(std::istream& in, const std::string& filename, const std::string& contentType = "text/plain; charset=UTF-8")
        {
            MimeObject& mimeObject = addObject(in, contentType, MimeObject::quotedPrintable);
            mimeObject.addHeader("Content-Disposition", "attachment; filename=" + filename);
            return mimeObject;
        }

        /// Adds a text file. The data is read from a file.
        MimeObject& attachTextFile(const std::string& filename, const std::string& contentType = "text/plain; charset=UTF-8");

        /// Adds a binary file. The data is passed as a std::string.
        MimeObject& attachBinaryFile(const std::string& data, const std::string& filename, const std::string& contentType = "application/x-binary")
        {
            MimeObject& mimeObject = addObject(data, contentType, MimeObject::base64);
            mimeObject.addHeader("Content-Disposition", "attachment; filename=" + filename);
            return mimeObject;
        }

        /// Adds a binary file. The data is read from a istream
        MimeObject& attachBinaryFile(std::istream& in, const std::string& filename, const std::string& contentType = "application/x-binary")
        {
            MimeObject& mimeObject = addObject(in, contentType, MimeObject::base64);
            mimeObject.addHeader("Content-Disposition", "attachment; filename=" + filename);
            return mimeObject;
        }

        /// Adds a binary file. The data is read from a file.
        MimeObject& attachBinaryFile(const std::string& filenamea, const std::string& contentType = "application/x-binary");

};

std::ostream& operator<< (std::ostream& out, const MimeHeader& mimeHeader);
std::ostream& operator<< (std::ostream& out, const MimeObject& mimeObject);
std::ostream& operator<< (std::ostream& out, const MimeMultipart& mime);

}
