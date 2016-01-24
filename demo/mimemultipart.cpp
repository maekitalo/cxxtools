/*
 * Copyright (C) 2016 Tommi Maekitalo
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

/* This creates a mail message with attachements and multiple message formats.
 *
 * A mail message with attachements is composed from a mime multipart message.
 * The type of it is multipart/related (or multipart/mixed).
 *
 * To add alternative formats (typically html and text) add a multipart
 * container with type mutlipart/alternative and add mime entities with
 * suitable content types.
 *
 * Attachements has a content type and a content disposition header. The
 * cxxtools mime multipart object makes it easy but offering suitable methods.
 */

#include <iostream>
#include <fstream>
#include <cxxtools/mime.h>

int main(int argc, char* argv[])
{
    try
    {
        // create a multipart/related object (the outer container)
        cxxtools::MimeMultipart mm(cxxtools::MimeMultipart::typeRelated);

        // create multipart/alternative object for text and html formats
        cxxtools::MimeMultipart al(cxxtools::MimeMultipart::typeAlternative);

        // create message body in text format.
        cxxtools::MimeEntity& plain =
            al.addObject()
              .setContentType("text/plain; charset=UTF-8");
        plain << "Hi there";

        // create message body in html format.
        cxxtools::MimeEntity& html =
            al.addObject()
              .setContentType("text/html; charset=UTF-8");
        html << "<html><body><h1>Hi there</h1></body></html>";

        // add multipart/alternative object to outer container
        mm.addObject(al);

        // create file attachement from data
        mm.attachTextFile("this is the content", "file2.txt");

        // read file from input stream
        std::ifstream in("mimemultipart.cpp");
        mm.attachTextFile(in, "mimemultipart.cpp");

        // output the outer container
        std::cout << mm;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
