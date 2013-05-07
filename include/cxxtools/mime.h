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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <iosfwd>
#include <string>
#include <vector>
#include <map>

namespace cxxtools
{
  class Mimepart
  {
      friend std::ostream& operator<< (std::ostream& out, const Mimepart& mimePart);

    public:
      enum ContentTransferEncoding {
        quotedPrintable,
        base64
      };

    private:
      typedef std::map<std::string, std::string> HeadersType;
      HeadersType headers;
      ContentTransferEncoding contentTransferEncoding;
      std::string body;

    public:
      explicit Mimepart(const std::string& contentType_ = "text/plain, charset=UTF-8",
                        ContentTransferEncoding contentTransferEncoding_ = quotedPrintable);

      void setData(const std::string& data)   { body = data; }
      void addData(const std::string& data)   { body += data; }
      void addData(std::istream& in);
      const std::string& getBody() const       { return body; }
      void setHeader(const std::string& key, const std::string& value)
        { headers[key] = value; }
  };

  class Mime
  {
      friend std::ostream& operator<< (std::ostream& out, const Mime& mime);

    public:
      typedef Mimepart::ContentTransferEncoding ContentTransferEncoding;

    private:
      typedef std::map<std::string, std::string> HeadersType;
      HeadersType headers;

      typedef std::vector<Mimepart> PartsType;
      PartsType parts;

    public:
      /// Adds a header-line to the mime-object.
      void setHeader(const std::string& key, const std::string& value)
         { headers[key] = value; }

      /// Adds a part to the mime-object.
      Mimepart& addPart(const Mimepart& part)
        { parts.push_back(part); return parts.back(); }

      /// Adds a part to the mime-object. The data is passed as a std::string.
      Mimepart& addPart(const std::string& data, const std::string& contentType = "text/plain",
        ContentTransferEncoding contentTransferEncoding = Mimepart::quotedPrintable);

      /// Adds a part to the mime-object. The data is read from a input stream.
      Mimepart& addPart(std::istream& in, const std::string& contentType = "text/plain",
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
  std::ostream& operator<< (std::ostream& out, const Mime& mime);
}
