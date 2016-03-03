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
#include <cxxtools/iodevice.h>

namespace cxxtools
{

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
class StreamBuffer : public BasicStreamBuffer<char>,
                     public Connectable
{
    public:
        //! @brief Constructs an IOBuffer for an IODevice
        explicit StreamBuffer(IODevice& ioDevice, size_t bufferSize = 8192, bool extend = false);

        //! @brief Default constructor
        explicit StreamBuffer(size_t bufferSize = 8192, bool extend = false);

        ~StreamBuffer();

        void attach(IODevice& ioDevice);

        IODevice* device()
            { return _ioDevice; }

        /** Sets the stream buffer in reading mode.
         *
         *  This signals the cxxtools::Selector, that the streambuffer is interested in
         *  receiving data. When data is available the cxxtools::Selector will emit
         *  the inputReady signal. After that endRead can be called without blocking.
         */
        void beginRead();

        /** Finalizes a read initiated with beginRead.
         *
         *  The call to endRead reads at least one byte from the underlying I/O device
         *  to the buffer. When no data is available, the call will block.
         */
        void endRead();

        /** Sets the stream buffer in writing mode if data is available in output buffer.
         *
         *  The device will emit a outputReady signal when it will be ready to
         *  write without blocking. When there is no data to be written, the
         *  device mode is not changed.
         *
         *  The streambuffer tries to write data immediately. The number of
         *  bytes written is returned. When data is already written, the device
         *  is still changed to writing mode and a endWrite must be called to
         *  finalize the write.
         *
         *  When the device is already in writing mode, nothing is done.
         */
        size_t beginWrite();

        /** Finalizes the write operation.
         *
         *  A write operation must be finalized with the call to endWrite.
         *  The method returns the number of bytes written either when
         *  beginWrite was called or here on endWrite.
         */
        size_t endWrite();

        /** Returns true if the underlying device is in reading mode.
         *
         *  The device is in reading mode, when beginRead has been called.
         *  The call of endRead finished the reading mode.
         */
        bool reading() const
            { return _ioDevice && _ioDevice->reading(); }

        /** Returns true if the underlying device is in writing mode.
         *
         *  The device is in writing mode, when beginWrite has been called.
         *  The call of endWrite finished the writing mode.
         */
        bool writing() const
            { return _ioDevice && _ioDevice->writing(); }

        /** Empties the data in the buffer.
         *
         *  The device must not be in reading or writing mode.  A exception of
         *  type cxxtools::IOPending will be thrown otherwise.
         */
        void discard();

        /** Signals, that the underlying I/O device has data to read.
         */
        Signal<StreamBuffer&> inputReady;

        /** Signals, that the underlying I/O device can receive data without blocking.
         */
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

        void onWrite(IODevice& dev);

    private:
        IODevice* _ioDevice;
        size_t _ibufferSize;
        char* _ibuffer;
        std::size_t _obufferSize;
        char* _obuffer;
        const size_t _pbmax;
        bool _oextend;
};

} // namespace cxxtools

#endif
