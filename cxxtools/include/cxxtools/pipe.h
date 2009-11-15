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

#ifndef CXXTOOLS_PIPE_H
#define CXXTOOLS_PIPE_H

#include <cxxtools/noncopyable.h>
#include <cxxtools/api.h>
#include <cxxtools/iodevice.h>
#include <unistd.h>

namespace cxxtools
{
    class PipeImpl;

class CXXTOOLS_API Pipe : private NonCopyable
{
    private:
        class PipeImpl* _impl;

    public:
        typedef int OpenMode;

        static const int Sync  = 0;
        static const int Async = 1;

        /** @brief Creates the pipe with two IODevices

            The default constructor will create the pipe and the appropriate
            IODevices to read and write to the pipe.
        */
        explicit Pipe(OpenMode mode = Sync);

        /** @brief Destructor

            Destroys the pipe and closes the internal IODevices.
        */
        ~Pipe();

        /** @brief Endpoint of the pipe to read from

            @return An IODevice used to read from the pipe
        */
        IODevice& out();

        const IODevice& out() const;

        /** @brief Endpoint of the pipe to write to

            @return An IODevice used to write to the pipe
        */
        IODevice& in();

        const IODevice& in() const;

        int getReadFd() const;

        int getWriteFd() const;

        void closeReadFd()
        { out().close(); }

        void closeWriteFd()
        { in().close(); }

        /// Redirect write-end to stdout.
        /// When the close argument is set, closes the original filedescriptor
        void redirectStdout(bool close = true);

        /// Redirect read-end to stdin.
        /// When the close argument is set, closes the original filedescriptor
        void redirectStdin(bool close = true);

        /// Redirect write-end to stdout.
        /// When the close argument is set, closes the original filedescriptor
        void redirectStderr(bool close = true);

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

#endif // CXXTOOLS_PIPE_H
