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

#ifndef CXXTOOLS_MIME_H
#define CXXTOOLS_MIME_H

#include <iosfwd>
#include <string>
#include <vector>
#include <utility>

namespace cxxtools
{
class MimeEntity;
class MimeMultipart;
class SerializationInfo;

/** The class implements a base class for mime entities, with headers as specified in rfc2045.
 */
class MimeHeader
{
        friend std::ostream& operator<< (std::ostream& out, const MimeHeader& mimeHeader);
        friend void operator<<= (SerializationInfo& si, const MimeHeader& mh);
        friend void operator>>= (const SerializationInfo& si, MimeHeader& mh);

        typedef std::vector<std::pair<std::string, std::string> > HeadersType;
        HeadersType headers;

    public:
        /// returns the header value or a default value
        std::string getHeader(const std::string& key, const std::string& def = std::string()) const;

        /// Set a header line to the mime entity.
        void setHeader(const std::string& key, const std::string& value, bool replace = true);

        /// Adds a header line to the mime entity.
        void addHeader(const std::string& key, const std::string& value)
            { headers.push_back(HeadersType::value_type(key, value)); }

        /// Removes the header
        void unsetHeader(const std::string& key);

        /// Returns true, if the content type is multipart/*
        bool isMultipart() const;
};

/** A MimeEntity is a message with headers and a body.

    The headers contain typically a content type, which specifies, how the body
    is to be interpreted.
 */
class MimeEntity : public MimeHeader
{
        friend std::ostream& operator<< (std::ostream& out, const MimeEntity& mimeEntity);
        friend void operator<<= (SerializationInfo& si, const MimeEntity& mo);
        friend void operator>>= (const SerializationInfo& si, MimeEntity& mo);
        friend MimeEntity& operator<< (MimeEntity& me, const std::string& str);
        friend MimeEntity& operator<< (MimeEntity& me, const char* str);

    public:
        enum ContentTransferEncoding {
            none,
            quotedPrintable,
            base64
        };

    private:
        std::string body;

    public:
        /// Creates an mime entity without headers and an empty body.
        MimeEntity() { }

        /// Creates an mime entity from a string.
        /// The string must be a mime message. On errors an exception is thrown.
        explicit MimeEntity(const std::string& data);

        /// Reads an mime entity from a input stream.
        explicit MimeEntity(std::istream& in);

        /// Returns the body of the message.
        const std::string& getBody() const       { return body; }
        /// Returns the body of the message.
        std::string& getBody()                   { return body; }

        /// Sets the body of the message.
        MimeEntity& setBody(const std::string& b)       { body = b; return *this; }

        MimeEntity& setContentTransferEncoding(ContentTransferEncoding cte);
        ContentTransferEncoding getContentTransferEncoding() const;

        MimeEntity& setContentType(const std::string& ct)
            { setHeader("Content-Type", ct); return *this; }
        std::string getContentType() const
            { return getHeader("Content-Type"); }

        MimeEntity& setHeader(const std::string& key, const std::string& value, bool replace = true)
            { MimeHeader::setHeader(key, value, replace); return *this; }
};

/// operator to add a std::string to a mime entity body.
inline MimeEntity& operator<< (MimeEntity& me, const std::string& str)
{
    me.body.append(str);
    return me;
}

/// operator to add a const char* to a mime entity body.
inline MimeEntity& operator<< (MimeEntity& me, const char* str)
{
    me.body.append(str);
    return me;
}


/** A MimeMultipart is a mime entity with multiple embedded entities.
 */
class MimeMultipart : public MimeHeader
{
        friend std::ostream& operator<< (std::ostream& out, const MimeMultipart& mime);
        friend void operator<<= (SerializationInfo& si, const MimeMultipart& mm);
        friend void operator>>= (const SerializationInfo& si, MimeMultipart& mm);
        friend void operator>>= (const SerializationInfo& si, MimeEntity& mo);

    public:
        typedef MimeEntity::ContentTransferEncoding ContentTransferEncoding;

        enum Type {
                typeMixed,
                typeAlternative,
                typeDigest,
                typeParallel,
                typeRelated
        };

    private:
        typedef std::vector<MimeEntity> PartsType;
        PartsType parts;

        std::string type;    // mixed or alternative

        void partsFromBody(const std::string& body, std::string::size_type pos = 0);
        static std::string stringParts(const std::vector<MimeEntity>& parts, std::vector<std::string>& sparts);

    public:
        typedef PartsType::const_iterator const_iterator;
        typedef PartsType::iterator iterator;
        typedef PartsType::size_type size_type;

        /// Parses a multipart entity.
        explicit MimeMultipart(const std::string& data);

        /// Make a MimeMultipart entity from a MimeEntity.
        /// If the content type is not multipart/something, a exception is thrown.
        MimeMultipart(const MimeEntity& mimeEntity);

        /// Creates a empty MimeMultipart object
        explicit MimeMultipart(Type type_ = typeMixed)
            { setType(type_); }

        /// Returns the number of entities in this object.
        size_type size() const
            { return parts.size(); }
        /// Returns the nth entity.
        MimeEntity& operator[] (size_type n)
            { return parts[n]; }
        /// Returns the nth entity.
        const MimeEntity& operator[] (size_type n) const
            { return parts[n]; }
        /// Returns a iterator to the first entity.
        iterator begin()
            { return parts.begin(); }
        /// Returns a iterator after the last entity.
        iterator end()
            { return parts.end(); }
        /// Returns a iterator to the first entity.
        const_iterator begin() const
            { return parts.begin(); }
        /// Returns a iterator after the last entity.
        const_iterator end() const
            { return parts.end(); }

        void setType(Type type_);
        void setType(const std::string& type_)
            { type = type_; }

        /// Adds a empty mime entity and returns a reference to it.
        MimeEntity& addObject()
            { parts.resize(parts.size() + 1); return parts.back(); }

        /// Adds a entity to the mime entity.
        MimeEntity& addObject(const MimeEntity& mimeEntity)
            { parts.push_back(mimeEntity); return parts.back(); }

        /// Adds a entity to the mime entity. The data is passed as a std::string.
        MimeEntity& addObject(const std::string& data, const std::string& contentType = "text/plain; charset=UTF-8",
            ContentTransferEncoding contentTransferEncoding = MimeEntity::quotedPrintable);

        /// Adds a entity to the mime entity. The data is read from a input stream.
        MimeEntity& addObject(std::istream& in, const std::string& contentType = "text/plain; charset=UTF-8",
            ContentTransferEncoding contentTransferEncoding = MimeEntity::quotedPrintable);

        MimeEntity& addObject(const MimeMultipart& mimeMultipart);

        /// Adds a text file. The data is passed as a std::string.
        MimeEntity& attachTextFile(const std::string& data, const std::string& filename, const std::string& contentType = "text/plain; charset=UTF-8")
        {
            MimeEntity& mimeEntity = addObject(data, contentType, MimeEntity::quotedPrintable);
            mimeEntity.addHeader("Content-Disposition", "attachment; filename=" + filename);
            return mimeEntity;
        }

        /// Adds a text file. The data is read from a istream
        MimeEntity& attachTextFile(std::istream& in, const std::string& filename, const std::string& contentType = "text/plain; charset=UTF-8")
        {
            MimeEntity& mimeEntity = addObject(in, contentType, MimeEntity::quotedPrintable);
            mimeEntity.addHeader("Content-Disposition", "attachment; filename=" + filename);
            return mimeEntity;
        }

        /// Adds a binary file. The data is passed as a std::string.
        MimeEntity& attachBinaryFile(const std::string& data, const std::string& filename, const std::string& contentType = "application/x-binary")
        {
            MimeEntity& mimeEntity = addObject(data, contentType, MimeEntity::base64);
            mimeEntity.addHeader("Content-Disposition", "attachment; filename=" + filename);
            return mimeEntity;
        }

        /// Adds a binary file. The data is read from a istream
        MimeEntity& attachBinaryFile(std::istream& in, const std::string& filename, const std::string& contentType = "application/x-binary")
        {
            MimeEntity& mimeEntity = addObject(in, contentType, MimeEntity::base64);
            mimeEntity.addHeader("Content-Disposition", "attachment; filename=" + filename);
            return mimeEntity;
        }

};

std::ostream& operator<< (std::ostream& out, const MimeHeader& mimeHeader);
std::ostream& operator<< (std::ostream& out, const MimeEntity& mimeEntity);
std::ostream& operator<< (std::ostream& out, const MimeMultipart& mime);

}

#endif
