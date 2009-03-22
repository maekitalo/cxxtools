/*
 * Copyright (C) 2005 Marc Boris Duerner
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
#ifndef cxxtools_System_StreamBuffer_h
#define cxxtools_System_StreamBuffer_h

#include <ios>
#include <streambuf>
#include <cxxtools/api.h>
#include <cxxtools/iodevice.h>

namespace cxxtools {

template <typename CharT>
class BasicStreamBuffer : public std::basic_streambuf<CharT>
{
    public:
        std::streamsize speekn(CharT* buffer, std::streamsize size)
        { return this->xspeekn(buffer, size); }

        std::streamsize out_avail()
        {
            if( this->pptr() )
                return this->pptr() - this->pbase();

             return this->showfull();
        }

     protected:
        virtual std::streamsize xspeekn(CharT* buffer, std::streamsize size)
        {
            if(size == 0)
                return 0;

            buffer[0] = this->sgetc();
            return 1;
        }

        virtual std::streamsize showfull()
        { return 0; }
};

//! @brief A stream buffer for IODevices with linear buffer area
class CXXTOOLS_API StreamBuffer : public BasicStreamBuffer<char>
                                 , public Connectable
{
    public:
        //! @brief Contructs an IOBuffer for an IODevice
        explicit StreamBuffer(IODevice& ioDevice, size_t bufferSize = 8192, bool extend = false);

        //! @brief Default constructor
        explicit StreamBuffer(size_t bufferSize = 8192, bool extend = false);

        ~StreamBuffer();

        void attach(IODevice& ioDevice);

        IODevice* device();

        void beginRead();

        void beginWrite();

        void discard();

        Signal<StreamBuffer&> inputReady;

        Signal<StreamBuffer&> outputReady;

    protected:
        virtual int sync();

        virtual std::streamsize showfull();

        virtual std::streamsize xspeekn(char* buffer, std::streamsize size);

        virtual int_type underflow();

        virtual int_type overflow(int_type ch);

        virtual int_type pbackfail(int_type c);

        /** @brief  Alters the stream positions
        */
        virtual pos_type seekoff(off_type offset, std::ios::seekdir sd, std::ios::openmode mode);

        /** @brief  Alters the stream positions
        */
        virtual pos_type seekpos(pos_type p, std::ios::openmode mode );

    private:
        void onRead(IODevice& dev);

        void endRead();

        void onWrite(IODevice& dev);

        void endWrite();

    private:
        IODevice* _ioDevice;
        size_t _bufferSize;
        char* _ibuffer;
        std::size_t _obufferSize;
        char* _obuffer;
        bool _oextend;
        const size_t _pbmax;
        bool _reading;
        bool _flushing;
};

} // namespace cxxtools

#endif
