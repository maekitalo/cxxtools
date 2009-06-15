/*
 * Copyright (C) 2004-2009 Marc Boris Duerner
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
#ifndef cxxtools_TextBuffer_h
#define cxxtools_TextBuffer_h

#include <cxxtools/api.h>
#include <cxxtools/textcodec.h>
#include <cxxtools/conversionerror.h>
#include <iostream>

namespace cxxtools {

/** @brief Converts character sequences with different encodings.

	This class derives from std::basic_streambuf which is the super-class of all stream buffer
	classes. Stream buffer classes are used to connect to an external device, transport characters
	from and to this external device and buffer the characters in an internal buffer.

	The internal character set can be specified using the template parameters 'char_type_', the
	external character set using 'extern_type_'. The external type is the input type and output
	type when reading from or writing to the external device. The internal type is the type
	which is used to internally store the data from the external device after the external
	format was converted using the Codec which is passed when constructing an object of this
	class.

	The Codec object which is passed as pointer to the constructor will afterwards be completely
	managed by this class and also be deleted by this class when it's destructed!

  @see std::basic_streambuf
*/
template <typename CharT, typename ByteT>
class BasicTextBuffer : public std::basic_streambuf<CharT>
{
    public:
        typedef ByteT extern_type;
        typedef CharT intern_type;
        typedef CharT char_type;
        typedef typename std::char_traits<CharT> traits_type;
        typedef typename traits_type::int_type int_type;
        typedef typename traits_type::pos_type pos_type;
        typedef typename traits_type::off_type off_type;
        typedef TextCodec<char_type, extern_type> CodecType;
        typedef MBState state_type;

    private:
        static const int _pbmax = 4;

        static const int _ebufmax = 256;
        extern_type _ebuf[_ebufmax];
        int _ebufsize;

        static const int _ibufmax = 256;
        intern_type _ibuf[_ibufmax];

        //! Contains the state of conversion.
        state_type _state;

        //! The codec which is used to convert character data from or to the external device.
        CodecType* _codec;

        std::basic_ios<extern_type>* _target;

    public:
        /** @brief Creates a BasicTextBuffer using the given stream buffer and codec.

            The given stream buffer @a target is used as external device,
            buffered by this Text buffer and all input from and output to
            the external device is converted using the codec @a codec.

            Note: The Codec object which is passed as pointer will be
            managed by this class and also be deleted by this class
            on destruction.
        */
        BasicTextBuffer(std::basic_ios<extern_type>* target, CodecType* codec)
        : _ebufsize(0)
        , _codec(codec) 
        , _target(target)
        {
            this->setg(0, 0, 0);
            this->setp(0, 0);
        }

        ~BasicTextBuffer() throw()
        {
            try
            {
                this->terminate();
            }
            catch(...) {}

            if(_codec && _codec->refs() == 0)
                delete _codec;
        }

        void attach(std::basic_ios<extern_type>& target)
        {
            this->terminate();
            _target = &target;
        }

        void detach()
        {
            this->terminate();
            _target = 0;
        }

        int terminate()
        {
            if( this->pptr() )
            {
                if( -1 == this->sync() )
                    return -1;

                if( _codec && ! _codec->always_noconv() )
                {
                    typename CodecType::result res = CodecType::error;
                    do
                    {
                        extern_type* next = 0;
                        res = _codec->unshift(_state, _ebuf, _ebuf + _ebufmax, next);
                        _ebufsize = next - _ebuf;

                        if(res == CodecType::error)
                        {
                            throw ConversionError("character conversion failed");
                        }
                        else if(res == CodecType::ok || res == CodecType::partial)
                        {
                            if(_ebufsize > 0)
                            {
                                _ebufsize -= _target->rdbuf()->sputn(_ebuf, _ebufsize);
                                if(_ebufsize)
                                    return -1;
                            }
                        }
                    }
                    while(res == CodecType::partial);
                }
            }

            this->setp(0, 0);
            this->setg(0, 0, 0);
            _ebufsize = 0;
            _state = state_type();
            return 0;
        }

        std::streamsize import()
        {
            if( _target )
            {
                std::streamsize n = _target->rdbuf()->in_avail();
                return do_underflow(n).second;
            }

            return this->in_avail();
        }

    protected:
        // inheritdoc
        virtual int sync()
        {
            if( this->pptr() )
            {
                // Try to write out the whole buffer to the underlying stream.
                // Fail if we can not make progress, because more characters
                // are needed to continue a multi-byte sequence.
                while( this->pptr() > this->pbase() )
                {
                    const char_type* p = this->pptr();

                    if( this->overflow( traits_type::eof() ) == traits_type::eof() )
                        return -1;

                    if( p == this->pptr() )
                        throw ConversionError("character conversion failed");
                }
            }

            return 0;
        }

        virtual std::streamsize showmanyc()
        {
            return 0;
        }

        // inheritdoc
        virtual int_type overflow( int_type ch = traits_type::eof() )
        {
            if( ! _target || this->gptr() )
                return traits_type::eof();

            if( ! this->pptr() )
            {
                this->setp( _ibuf, _ibuf + _ibufmax );
            }

            while( this->pptr() > this->pbase() )
            {
                const char_type* fromBegin = _ibuf;
                const char_type* fromEnd   = this->pptr();
                const char_type* fromNext  = fromBegin;
                extern_type* toBegin       = _ebuf + _ebufsize;
                extern_type* toEnd         = _ebuf + _ebufmax;
                extern_type* toNext        = toBegin;

                typename CodecType::result res = CodecType::noconv;
                if(_codec)
                    res = _codec->out(_state, fromBegin, fromEnd, fromNext, toBegin, toEnd, toNext);

                if(res == CodecType::noconv)
                {
                    size_t fromSize = fromEnd - fromBegin;
                    size_t toSize   = toEnd - toBegin;
                    size_t size     = toSize < fromSize ? toSize : fromSize;

                    this->copyChars( toBegin, fromBegin, size );

                    fromNext += size;
                    toNext += size;
                }

                _ebufsize += toNext - toBegin;
                size_t leftover = fromEnd - fromNext;
                if(leftover && fromNext > fromBegin)
                {
                    std::char_traits<char_type>::move(_ibuf, fromNext, leftover);
                }

                this->setp( _ibuf, _ibuf + _ibufmax );
                this->pbump( leftover );

                if(res == CodecType::error)
                    throw ConversionError("character conversion failed");

                if(res == CodecType::partial && _ebufsize == 0)
                    break;

                _ebufsize -= _target->rdbuf()->sputn(_ebuf, _ebufsize);
                if(_ebufsize)
                    return traits_type::eof();
            }

            if( ! traits_type::eq_int_type(ch, traits_type::eof()) )
            {
                *( this->pptr() ) = traits_type::to_char_type(ch);
                this->pbump(1);
            }

            return traits_type::not_eof(ch);
        }


        // inheritdoc
        virtual int_type underflow()
        {
            if( ! _target )
                return traits_type::eof();

            if( this->gptr() < this->egptr() )
                return traits_type::to_int_type( *this->gptr() );

            return do_underflow(_ebufmax).first;
        }


        std::pair<int_type, std::streamsize> do_underflow(std::streamsize size)
        {
			typedef std::pair<int_type, std::streamsize> ret_type;

			std::streamsize n = 0;

            if( this->pptr() )
            {
                if( -1 == this->terminate() )
                    return ret_type(traits_type::eof(), 0);
            }

            if( ! this->gptr() )
            {
                this->setg(_ibuf, _ibuf, _ibuf);
            }

            if( this->gptr() - this->eback() > _pbmax)
            {
                std::streamsize movelen = this->egptr() - this->gptr() + _pbmax;
                std::char_traits<char_type>::move( _ibuf,
                                                   this->gptr() - _pbmax,
                                                   movelen );
                this->setg(_ibuf, _ibuf + _pbmax, _ibuf + movelen);
            }

            bool atEof = false;
            const std::streamsize bufavail = _ebufmax - _ebufsize;
            size = bufavail < size ? bufavail : size;
            if(size)
            {
                n = _target->rdbuf()->sgetn( _ebuf + _ebufsize, size );
                _ebufsize += n;
                if(n == 0)
                    atEof = true;
            }

            const extern_type* fromBegin = _ebuf;
            const extern_type* fromEnd   = _ebuf + _ebufsize;
            const extern_type* fromNext  = fromBegin;
            char_type* toBegin           = this->egptr();
            char_type* toEnd             = _ibuf + _ibufmax;
            char_type* toNext            = toBegin;

            typename CodecType::result r = CodecType::noconv;
            if(_codec)
                r = _codec->in(_state, fromBegin, fromEnd, fromNext, toBegin, toEnd, toNext);

            if(r == CodecType::noconv)
            {
                // copy characters and advance fromNext and toNext
                int n =_ebufsize > _ibufmax ? _ibufmax : _ebufsize ;
                this->copyChars(toBegin, fromBegin, n);
                _ebufsize -= n;
                fromNext += n;
                toNext += n;
            }

            std::streamsize consumed = fromNext - fromBegin;
            if(consumed)
            {
                std::char_traits<extern_type>::move( _ebuf, _ebuf + consumed, _ebufsize );
                _ebufsize -= consumed;
            }

            std::streamsize generated = toNext - toBegin;
            if(generated)
            {
                this->setg(this->eback(),              // start of read buffer
                           this->gptr(),               // gptr position
                           this->egptr() + generated ); // end of read buffer
            }

            if(r == CodecType::error)
                throw ConversionError("character conversion failed");

            if( this->gptr() < this->egptr() )
                return ret_type(traits_type::to_int_type( *this->gptr() ), n);

            // fail if partial charactes are at the end of the stream
            if(r == CodecType::partial && atEof)
                throw ConversionError("character conversion failed");

            return ret_type(traits_type::eof(), 0);
        }

        template <typename T>
        void copyChars(T* s1, const T* s2, size_t n)
        {
            std::char_traits<T>::copy(s1, s2, n);
        }

        //TODO: signature like codecvt with ptr refs
        template <typename A, typename B>
        void copyChars(A* s1, const B* s2, size_t n)
        {
            while(n-- > 0)
            {
                *s1 = *s2;
                ++s1;
                ++s2;
            }
        }
};


/** @brief Buffers the conversion of 8-bit character sequences to unicode.

    The internal type is cxxtools::Char. The external type is $char$.
    See BasicTextBuffer for a more detailed description.
*/
class CXXTOOLS_API TextBuffer : public BasicTextBuffer<cxxtools::Char, char>
{
    public:
        typedef TextCodec<cxxtools::Char, char> Codec;

    public:
        /** @brief Constructs a new TextBuffer
             See BasicTextBuffer::BasicTextBuffer() for a more detailed description.

             @param buffer The buffer (external device) which is wrapped by this object.
             @param codec The codec which is used to convert data from and to the external device.
        */
        TextBuffer(std::ios* buffer, Codec* codec);
};

} // namespace cxxtools

#endif

