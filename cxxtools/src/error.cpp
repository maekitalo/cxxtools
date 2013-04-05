/*
 * Copyright (C) 2009 Tommi Maekitalo
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
 *
 */

#include "error.h"
#include <sstream>
#include <vector>
#include <string.h>

namespace cxxtools
{

namespace
{
    // XSI compliant strerror_r
    inline void errorOut(int (*errfn)(int, char*, size_t), std::ostream& out, int errnum)
    {
        std::vector<char> buffer(512);
        while (errfn(errnum, &buffer[0], buffer.size()) != 0)
        {
            if (errno != ERANGE)
            {
                out << "Unknown error " << errnum;
                return;
            }

            buffer.resize(buffer.size() * 2);
        }

        out << &buffer[0];
    }

    // GNU specific strerror_r
    inline void errorOut(char* (*errfn)(int, char*, size_t), std::ostream& out, int errnum)
    {
        std::vector<char> buffer(512);
        while (true)
        {
            char* f = errfn(errnum, &buffer[0], buffer.size());
            if (f != &buffer[0])
            {
                out << f;
                return;
            }

            if (strlen(&buffer[0]) < buffer.size() - 1)
                break;

            buffer.resize(buffer.size() * 2);
        }

        out << &buffer[0];
    }

    inline void errorOut(std::ostream& out, int errnum)
    {
      errorOut(strerror_r, out, errnum);
    }
}

std::string getErrnoString(int errnum)
{
    std::ostringstream msg;
    msg << "errno " << errnum << ": ";
    errorOut(msg, errnum);
    return msg.str();
}

std::string getErrnoString(int errnum, const char* fn)
{
    if (errnum != 0)
    {
        std::ostringstream msg;
        msg << fn << ": errno " << errnum << ": ";
        errorOut(msg, errnum);
        return msg.str();
    }
    else
        return fn;
}

}
