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
 This program converts a mime object to json using cxxtools.

 A file name can be passed as parameter. The file is read and printed in json
 format to stdout. When no parameter is passed, the program reads the message
 from stdin.

 Mails are mime messages, which can be read with this program.

 */

#include <iostream>
#include <sstream>
#include <fstream>
#include <cxxtools/mime.h>
#include <cxxtools/json.h>
#include <cxxtools/log.h>

int main(int argc, char* argv[])
{
    try
    {
        // initialize logging - reads log.xml or log.properties
        log_init();

        // read file to a stringstream
        std::ostringstream m;

        if (argc > 1)
        {
            std::ifstream in(argv[1]);
            m << in.rdbuf();
        }
        else
        {
            m << std::cin.rdbuf();
        }

        // parse the file
        cxxtools::MimeObject mo(m.str());

        // print as json using serialization framework
        std::cout << cxxtools::Json(mo).beautify(true);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

