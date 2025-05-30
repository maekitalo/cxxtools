/***************************************************************************
 * Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2007 Marc Boris Duerner
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
#ifndef Cxxtools_posix_PipeImpl_h
#define Cxxtools_posix_PipeImpl_h

#include "iodeviceimpl.h"
#include <cxxtools/iodevice.h>
#include <unistd.h>

namespace cxxtools {

class PipeIODevice : public IODevice
{
        friend class PipeImpl;

    public:
        PipeIODevice();

        ~PipeIODevice();

        int fd() const
        { return _impl.fd(); }

        void redirect(int fd, bool close, bool inherit);

    protected:
        void open(int fd, bool isAsync, bool inherit = true);

        void onClose()
        { cancel(); _impl.close(); }

        IODeviceImpl& ioimpl()
        { return _impl; }

        SelectableImpl& simpl()
        { return _impl; }

    private:
        IODeviceImpl _impl;
};


class PipeImpl
{
    public:
        PipeImpl(bool isAsync, bool inherit);

        PipeIODevice& in();

        const PipeIODevice& in() const;

        PipeIODevice& out();

        const PipeIODevice& out() const;

        void redirect(int fd, bool close, bool inherit);

        int getReadFd() const
            { return out().fd(); }

        int getWriteFd() const
            { return in().fd(); }

        /// Redirect write-end to stdout.
        /// When the close argument is set, closes the original filedescriptor
        void redirectStdout(bool close, bool inherit);

        /// Redirect read-end to stdin.
        /// When the close argument is set, closes the original filedescriptor
        void redirectStdin(bool close, bool inherit);

        /// Redirect write-end to stdout.
        /// When the close argument is set, closes the original filedescriptor
        void redirectStderr(bool close, bool inherit);

    private:
        PipeIODevice _out;
        PipeIODevice _in;
};

} // namespace cxxtools


#endif
