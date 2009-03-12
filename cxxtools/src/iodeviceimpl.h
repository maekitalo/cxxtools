/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction. Specifically, if other files    *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License. This exception does not however invalidate any other         *
 *   reasons why the executable file might be covered by the GNU Library   *
 *   General Public License.                                               *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef CXXTOOLS_SYSTEM_IODEVICEIMPL_H
#define CXXTOOLS_SYSTEM_IODEVICEIMPL_H

#include "selectableimpl.h"
#include <cxxtools/iodevice.h>
#include <string>
#include <iostream>

struct pollfd;

namespace cxxtools {

    class IODeviceImpl : public SelectableImpl
    {
        public:
            static const short POLLERR_MASK;
            static const short POLLIN_MASK;
            static const short POLLOUT_MASK;

            IODeviceImpl(IODevice&);

            virtual ~IODeviceImpl();

            int fd() const
            { return _fd; }

            void setTimeout(std::size_t msecs)
            { _timeout = msecs; }

            std::size_t timeout() const
            { return _timeout; }

            virtual void open(const std::string& path, IODevice::OpenMode mode);

            virtual void open(int fd, bool isAsync);

            virtual void close();

            virtual size_t beginRead(char* buffer, size_t n, bool& eof);

            virtual size_t endRead(bool& eof);

            virtual size_t read( char* buffer, size_t count, bool& eof );

            virtual size_t beginWrite(const char* buffer, size_t n);

            virtual size_t endWrite();

            virtual size_t write( const char* buffer, size_t count );

            virtual void sync() const;

            void attach(SelectorBase& s);

            void detach(SelectorBase& s);

            virtual bool wait(std::size_t msecs);

            virtual bool wait(std::size_t msecs, pollfd& pfd);

            virtual void initWait(pollfd& pfd);

            virtual size_t pollSize() const
            { return 1; }

            virtual size_t initializePoll(pollfd* pfd, size_t pollSize);

            virtual bool checkPollEvent();

            virtual bool checkPollEvent(pollfd& pfd);

        protected:
            IODevice& _device;
            int _fd;
            std::size_t _timeout;
            pollfd* _pfd;
            bool* _deleted;
    };

} //namespace cxxtools

#endif
