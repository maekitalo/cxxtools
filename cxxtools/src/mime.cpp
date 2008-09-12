/*
 * Copyright (C) 2007 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
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
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#include <cxxtools/mime.h>
#include <cxxtools/base64stream.h>
#include <cxxtools/quotedprintablestream.h>
#include <cxxtools/log.h>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <stdexcept>

log_define("cxxtools.mime")

namespace cxxtools
{
  Mimepart::Mimepart(const std::string& contentType_,
                     ContentTransferEncoding contentTransferEncoding_)
    : contentTransferEncoding(contentTransferEncoding_)
  {
    setHeader("Content-Type", contentType_);
  }

  void Mimepart::addData(std::istream& in)
  {
    std::ostringstream data;
    data << in.rdbuf();
    body += data.str();
  }

  Mimepart& Mime::addPart(const std::string& data, const std::string& contentType,
    ContentTransferEncoding contentTransferEncoding)
  {
    log_debug("add part " << data.size() << " bytes, contentType \""
        << contentType << "\" content transfer encoding " << contentTransferEncoding);

    parts.push_back(Mimepart(contentType, contentTransferEncoding));
    parts.back().setData(data);
    return parts.back();
  }

  Mimepart& Mime::addPart(std::istream& in, const std::string& contentType,
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

  Mimepart& Mime::addTextFile(const std::string& contentType, const std::string& filename)
  {
    std::ifstream in(filename.c_str());
    if (!in)
      throw std::runtime_error("cannot open file \"" + filename + '"');

    return addTextFile(contentType, filename, in);
  }

  Mimepart& Mime::addBinaryFile(const std::string& contentType, const std::string& filename)
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
      out << it->first << ": " << it->second << '\n';

    // encode data
    if (mimePart.contentTransferEncoding == Mimepart::quotedPrintable)
    {
      out << "Content-Transfer-Encoding: quoted-printable\n\n";
      QuotedPrintable_ostream enc(out);
      enc << mimePart.getBody();
      out << '\n';
    }
    else if (mimePart.contentTransferEncoding == Mimepart::base64)
    {
      out << "Content-Transfer-Encoding: base64\n\n";
      Base64ostream enc(out);
      enc << mimePart.getBody();
      enc.end();
      out << "\n\n";
    }
    else
    {
      std::ostringstream msg;
      msg << "unknown Content-Transfer-Encoding " << mimePart.contentTransferEncoding;
      log_error(msg.str());
      throw std::runtime_error(msg.str());
    }

    return out;
  }

  std::ostream& operator<< (std::ostream& out, const Mime& mime)
  {
    // build string parts
    typedef std::vector<std::string> SpartsType;
    SpartsType sparts;

    for (Mime::PartsType::const_iterator pit = mime.parts.begin(); pit != mime.parts.end(); ++pit)
    {
      std::ostringstream out;
      out << *pit;
      sparts.push_back(out.str());
    }

    // choose suitable boundary
    std::string boundary;
    time_t t;
    time(&t);
    while (true)
    {
      std::ostringstream h;
      h << std::hex << t;
      boundary = "=Boundary=" + h.str() + "=";
      for (SpartsType::const_iterator it = sparts.begin(); it != sparts.end(); ++it)
        if (it->find(boundary) != std::string::npos)
          continue;
      t += rand();
      break;
    }

    // print headers
    out << "MIME-Version: 1.0\n"
           "Content-Type: multipart/mixed; boundary=\"" << boundary << "\"\n";
    for (Mime::HeadersType::const_iterator it = mime.headers.begin();
         it != mime.headers.end(); ++it)
      out << it->first << ": " << it->second << '\n';
    out << '\n';

    // print parts
    for (SpartsType::const_iterator it = sparts.begin(); it != sparts.end(); ++it)
      out << "--" << boundary << '\n'
          << *it;

    out << "--" << boundary << "--\n";

    return out;
  }

}
