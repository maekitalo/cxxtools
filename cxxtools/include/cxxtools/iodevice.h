/*
 * Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2007 Marc Boris Duerner
 * Copyright (C) 2006-2007 PTV AG
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
#ifndef CXXTOOLS_IODevice_h
#define CXXTOOLS_IODevice_h

#include <cxxtools/types.h>
#include <cxxtools/signal.h>
#include <cxxtools/api.h>
#include <cxxtools/ioerror.h>
#include <cxxtools/selectable.h>
#include <limits>
#include <ios>

namespace cxxtools {

enum IOS_OpenMode
{
    IOS_Sync        = 0,
    IOS_Async       = 1L << 0,
    IOS_Read        = 1L << 1,
    IOS_Write       = 1L << 2,
    IOS_AtEnd       = 1L << 3,
    IOS_Append      = 1L << 4,
    IOS_Trunc       = 1L << 5,
    IOS_OpenModeEnd = 1L << 16
};

inline IOS_OpenMode operator&(IOS_OpenMode a, IOS_OpenMode b)
{ return IOS_OpenMode(static_cast<int>(a) & static_cast<int>(b)); }

inline IOS_OpenMode operator|(IOS_OpenMode a, IOS_OpenMode b)
{ return IOS_OpenMode(static_cast<int>(a) | static_cast<int>(b)); }

inline IOS_OpenMode operator^(IOS_OpenMode a, IOS_OpenMode b)
{ return IOS_OpenMode(static_cast<int>(a) ^ static_cast<int>(b)); }

inline IOS_OpenMode& operator|=(IOS_OpenMode& a, IOS_OpenMode b)
{ return a = a | b; }

inline IOS_OpenMode& operator&=(IOS_OpenMode& a, IOS_OpenMode b)
{ return a = a & b; }

inline IOS_OpenMode& operator^=(IOS_OpenMode& a, IOS_OpenMode b)
{ return a = a ^ b; }

inline IOS_OpenMode operator~(IOS_OpenMode a)
{ return IOS_OpenMode(~static_cast<int>(a)); }


class IODeviceImpl;

/** @brief Endpoint for I/O operations

    This class serves as the base class for all kinds of I/O devices. The
    interface supports synchronous and asynchronous I/O operations, peeking
    and seeking. I/O buffers and I/O streams within the cxxtools framework use
    IODevices as endpoints and therefore fully feaured standard C++ compliant
    IOStreams can be constructed at runtime.
    Examples of %IODevices are the SerialDevice, the endpoints of a Pipe
    or the FileDevice. A Selector can be used to wait on activity on an
    %IODevice, which will send the %Signal inputReady or outputReady of the
    %IODevice that is ready to perform I/O.
*/
class CXXTOOLS_API IODevice : public Selectable
{
    public:
        typedef std::char_traits<char>::pos_type pos_type;

        typedef std::char_traits<char>::off_type off_type;

        typedef std::ios_base::seekdir SeekDir;

        typedef IOS_OpenMode OpenMode;

        static const OpenMode Sync   = IOS_Sync;
        static const OpenMode Async  = IOS_Async;
        static const OpenMode Read   = IOS_Read;
        static const OpenMode Write  = IOS_Write;
        static const OpenMode AtEnd  = IOS_AtEnd;
        static const OpenMode Append = IOS_Append;
        static const OpenMode Trunc  = IOS_Trunc;

    public:
        //! @brief Destructor
        virtual ~IODevice();

        void beginRead(char* buffer, size_t n);

        size_t endRead();

        //! @brief Read data from I/O device
        /*!
            Reads up to n bytes and stores them in buffer. Returns the number
            of bytes read, which may be less than requested and even 0 if the
            device operates in asynchronous (non-blocking) mode. In case of
            EOF the IODevice is set to eof.

            \param buffer buffer where to place the data to be read.
            \param n number of bytes to read
            \return number of bytes read, which may be less than requested.
            \throw IOError
         */
        size_t read(char* buffer, size_t n);

        size_t beginWrite(const char* buffer, size_t n);

        size_t endWrite();

        //! @brief Write data to I/O device
        /**
            Writes n bytes from buffer to this I/O device. Returns the number
            of bytes written, which may be less than requested and even 0 if the
            device operates in asynchronous (non-blocking) mode. In case of
            EOF the IODevice is set to eof.

            \param buffer buffer containing the data to be written.
            \param n number of bytes that should be written.
            \return number of bytes written, which may be less than requested.
            \throw IOError
         */
        size_t write(const char* buffer, size_t n);

        /** @brief Cancels asynchronous reading and writing
        */
        void cancel();

        //! @brief Returns true if device is seekable
        /**
            Tests if the device is seekable.

            \return true if the device is seekable, false otherwise.
        */
        bool seekable() const;

        //! @brief Move the next read position to the given offset
        /**
            Tries to move the current read position to the given offset.
            SeekMode determines the relative starting point of offset.

            \param offset the offset the pointer should be moved by.
            \param mode determines the relative starting offset.
            \return the new abosulte read positing.
            \throw IOError
        */
        pos_type seek(off_type offset, std::ios::seekdir sd);

        //! @brief Read data from I/O device without consuming them
        /**
            Tries to extract up to n bytes from this object
            without consuming them. The bytes are stored in
            buffer, and the number of bytes peeked is returned.

            \param buffer buffer where to place the data to be read.
            \param n number of bytes to peek
            \return number of bytes peek.
            \throw IOError
        */
        size_t peek(char* buffer, size_t n);

        //! @brief Synchronize device
        /**
            Commits written data to physical device.

            \throw IOError
        */
        void sync();

        //! @brief Returns the current I/O position
        /**
            The current I/O position is returned or an IOError
            is thrown if the device is not seekable. Seekability
            can be tested with BasicIODevice::seekable().

            \throw IOError
        */
        pos_type position();

        //! @brief Returns if the device has reached EOF
        /*!
            Test if the I/O device has reached eof.

            \return true if the I/O device is usable, false otherwise.
        */
        bool eof() const;

        /** @brief Returns true if the device operates in asynchronous mode
        */
        bool async() const;

        /** @brief Notifies about available data

            This signal is send when the IODevice is monitored
            in a Selector or EventLoop and data becomes available.
        */
        Signal<IODevice&> inputReady;

        /** @brief Notifies when data can be written

            This signal is send when the IODevice is monitored
            in a Selector or EventLoop and the device is ready
            to write data.
        */
        Signal<IODevice&> outputReady;
 
        virtual IODeviceImpl& ioimpl() = 0;

        bool reading() const
        { return _rbuf != 0; }

        bool writing() const
        { return _wbuf != 0; }

        char* rbuf() const
        { return _rbuf; }

        size_t rbuflen() const
        { return _rbuflen; }

        size_t ravail() const
        { return _ravail; }

        const char* wbuf() const
        { return _wbuf; }

        size_t wbuflen() const
        { return _wbuflen; }

        size_t wavail() const
        { return _wavail; }

    protected:
        //! @brief Default Constructor
        IODevice();

        virtual size_t onBeginRead(char* buffer, size_t n, bool& eof) = 0;

        virtual size_t onEndRead(bool& eof) = 0;

        //! @brief Read bytes from device
        virtual size_t onRead(char* buffer, size_t count, bool& eof) = 0;

        virtual size_t onBeginWrite(const char* buffer, size_t n) = 0;

        virtual size_t onEndWrite() = 0;

        //! @brief Write bytes to device
        virtual size_t onWrite(const char* buffer, size_t count) = 0;

        virtual void onCancel() = 0;

        //! @brief Read data from I/O device without consuming them
        virtual size_t onPeek(char*, size_t)
        { return 0; }

        //! @brief Returns true if device is seekable
        virtual bool onSeekable() const
        { return false; }

        //! @brief Move the next read position to the given offset
        virtual pos_type onSeek(off_type, std::ios::seekdir)
        { throw IOError("Could not seek on device."); }

        //! @brief Synchronize device
        virtual void onSync() const
        { }

        //! @brief Returns the size of the device
        virtual size_t onSize() const
        { return 0; }

        //! @brief Sets or unsets the device to eof
        void setEof(bool eof);

        //! @brief Sets or unsets the device to async
        void setAsync(bool async);

    private:
        bool _eof;
        bool _async;

    protected:
        char* _rbuf;
        size_t _rbuflen;
        size_t _ravail;
        const char* _wbuf;
        size_t _wbuflen;
        size_t _wavail;
        void* _reserved;
};

} // namespace cxxtools

#endif
