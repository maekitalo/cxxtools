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
 */

#ifndef CXXTOOLS_POSIX_PIPE_H
#define CXXTOOLS_POSIX_PIPE_H

#include <cxxtools/pipe.h>

namespace cxxtools
{
namespace posix
{

class Pipe : public cxxtools::Pipe
{
    public:
        /** @brief Creates the pipe with two IODevices

            The default constructor will create the pipe and the appropriate
            IODevices to read and write to the pipe.
        */
        explicit Pipe(OpenMode mode = Sync, bool inherit = true)
            : cxxtools::Pipe(mode, inherit)
            { }

        /** @brief Endpoint of the pipe to read from

            @return An IODevice used to read from the pipe
        */
        int getReadFd() const;

        int getWriteFd() const;

        void closeReadFd()
        { in().close(); }

        void closeWriteFd()
        { out().close(); }

        /// Redirect write-end to stdout.
        /// When the close argument is set, closes the original filedescriptor
        void redirectStdout(bool close = true, bool inherit = true);

        /// Redirect read-end to stdin.
        /// When the close argument is set, closes the original filedescriptor
        void redirectStdin(bool close = true, bool inherit = true);

        /// Redirect write-end to stdout.
        /// When the close argument is set, closes the original filedescriptor
        void redirectStderr(bool close = true, bool inherit = true);

        size_t write(const char* buf, size_t count)
        {
          return in().write(buf, count);
        }

        void write(char ch)
        {
          in().write(&ch, 1);
        }

        size_t read(char* buf, size_t count)
        {
          return out().read(buf, count);
        }

        char read()
        {
          char ch;
          out().read(&ch, 1);
          return ch;
        }

};

}
}

#endif // CXXTOOLS_POSIX_PIPE_H
