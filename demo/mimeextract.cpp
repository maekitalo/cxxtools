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

/*
 This program extracts all parts of a mime/multipart message and writes
 them to the current directory. The message is either read from a file or
 when no file name is given as paramter from stdin.

 Attachements with file names are written using this file name. Other
 parts like the message itself are written in files named msgn.txt while
 n is a number starting from 0.

 The program searches recursively the whole message since parts of the
 message may be again mime/multipart messages.
 */

#include <iostream>
#include <cxxtools/argin.h>
#include <cxxtools/mime.h>
#include <cxxtools/regex.h>
#include <cxxtools/log.h>

class MimeExtractor
{
        unsigned N;

    public:
        MimeExtractor()
            : N(0)
            { }

        void extract(const cxxtools::MimeEntity& mo);
};

void MimeExtractor::extract(const cxxtools::MimeEntity& mo)
{
    if (mo.isMultipart())
    {
        cxxtools::MimeMultipart mp(mo);
        for (cxxtools::MimeMultipart::const_iterator it = mp.begin();
            it != mp.end(); ++it)
        {
            // recursively search for mime objects
            extract(*it);
        }
    }
    else
    {
        std::string filename;

        // Search for header "Content-Disposition: ... filename=something".
        // The filename can be enclosed in double quotes.
        // Not that we get a empty string from getHeader if the header
        // is not found at all, which is fine here.
        std::string contentDisposition = mo.getHeader("Content-Disposition");
        static const cxxtools::Regex re("filename=\"?([^;\"]+)");

        cxxtools::RegexSMatch sm;
        if (re.match(contentDisposition, sm))
        {
            filename = sm[1];
        }
        else
        {
            // When no filename is found, we use the filename msg0.txt, msg1.txt, ...
            std::ostringstream ss;
            ss << "msg" << N++ << ".txt";
            filename = ss.str();
        }

        std::ofstream of(filename.c_str());
        of << mo.getBody();
    }
}

int main(int argc, char* argv[])
{
    try
    {
        // Initialize logging - reads log.xml or log.properties
        log_init(argc, argv);

        // Define std::istream for input data
        // This reads data either from a file passed as argument or stdin
        // if no parameter is passed.
        cxxtools::ArgIn in(argc, argv);

        // Parse the file from input stream
        cxxtools::MimeEntity mo(in);

        // Extract objects from data
        MimeExtractor mx;
        mx.extract(mo);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

