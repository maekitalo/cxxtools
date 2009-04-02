/*
 * Copyright (C) 2004 Marc Boris Duerner
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

#ifndef cxxtools_BasicTextStream_h
#define cxxtools_BasicTextStream_h

#include <cxxtools/api.h>
#include <cxxtools/string.h>
#include <cxxtools/textbuffer.h>
#include <iostream>

namespace cxxtools {

/** @brief Converts character sequences using a Codec.

    This generic stream, which only supports input, wraps another input-stream and converts the
    character data which is received from the stream on-the-fly using a Codec. The data which is
    received from the wrapped input-stream is buffered.

    This class derives from std::basic_istream which is the super-class of input-stream classes.
    Stream classes are used to connect to an external device and transport characters
    from this external device.

    The internal character set can be specified using the template parameters 'I', the
    external character set using 'E'. The external type is the input type and output
    type when reading from or writing to the external device. The internal type is the type
    which is used to internally store the data from the external device after the external
    format was converted using the Codec which is passed when constructing an object of this
    class.

    The Codec object which is passed as pointer to the constructor will afterwards be
    managed by this class and also be deleted by this class when it's destructed!

    @see std::basic_istream
*/
template <typename CharT, typename ByteT>
class BasicTextIStream : public std::basic_istream<CharT>
{
    public:
        typedef ByteT extern_type;
        typedef CharT intern_type;
        typedef CharT char_type;
        typedef typename std::char_traits<CharT> traits_type;
        typedef typename traits_type::int_type int_type;
        typedef typename traits_type::pos_type pos_type;
        typedef typename traits_type::off_type off_type;
        typedef std::basic_istream<extern_type> StreamType;
        typedef TextCodec<char_type, extern_type> CodecType;

    public:
        /** @brief Construct by input stream and codec.

            The input stream @a is is used ro read a character sequence and
            convert it using the codec @a codec. The Codec object which is
            passed as pointer will afterwards be managed by this class and
            also be deleted on destruction
        */
        BasicTextIStream(StreamType& is, CodecType* codec)
        : std::basic_istream<intern_type>(0)
        , _buffer( &is, codec )
        {
            init(&_buffer);
        }

        BasicTextIStream(CodecType* codec)
        : std::basic_istream<intern_type>(0)
        , _buffer( 0, codec )
        {
            init(&_buffer);
        }

        //! @brief Deletes to codec.
        ~BasicTextIStream()
        {  }

        void attach(StreamType& is)
        {
            _buffer.attach( is );
            this->clear();
        }

        void detach()
        {
            _buffer.detach();
            this->clear();
        }

        void terminate()
        {
            _buffer.terminate();
        }

        BasicTextBuffer<intern_type, extern_type>& buffer()
        { return _buffer; }

    private:
        BasicTextBuffer<intern_type, extern_type> _buffer;
};


/** @brief Converts character sequences using a Codec.

    This generic stream, which only supports output, wraps another input-stream and converts the
    character data which is received from the stream on-the-fly using a Codec. The data which is
    received from the wrapped input-stream is buffered.

    This class derives from std::basic_istream which is the super-class of input-stream classes.
    Stream classes are used to connect to an external device and transport characters
    from this external device.

    The internal character set can be specified using the template parameters 'I', the
    external character set using 'E'. The external type is the input type and output
    type when reading from or writing to the external device. The internal type is the type
    which is used to internally store the data from the external device after the external
    format was converted using the Codec which is passed when constructing an object of this
    class.

    The Codec object which is passed as pointer to the constructor will afterwards be
    managed by this class and also be deleted by this class when it's destructed!

    @see std::basic_istream
*/
template <typename CharT, typename ByteT>
class BasicTextOStream : public std::basic_ostream<CharT>
{
    public:
        typedef ByteT extern_type;
        typedef CharT intern_type;
        typedef CharT char_type;
        typedef typename std::char_traits<CharT> traits_type;
        typedef typename traits_type::int_type int_type;
        typedef typename traits_type::pos_type pos_type;
        typedef typename traits_type::off_type off_type;
        typedef std::basic_ostream<extern_type> StreamType;
        typedef TextCodec<char_type, extern_type> CodecType;

    public:
        /** @brief Construct by output stream and codec.

            The output stream @a os is used ro write a character sequence
            which has been converted using the codec @a codec. The Codec
            object which is passed as pointer will afterwards be managed
            by this class and be deleted on destruction
        */
        BasicTextOStream(StreamType& os, CodecType* codec)
        : std::basic_ostream<intern_type>(0)
        , _buffer( &os , codec )
        { init(&_buffer); }

        BasicTextOStream(CodecType* codec)
        : std::basic_ostream<intern_type>(0)
        , _buffer( 0 , codec )
        { init(&_buffer); }

        //! @brief Deletes to codec.
        ~BasicTextOStream()
        {  }

        void attach(StreamType& os)
        {
            _buffer.attach( os );
            this->clear();
        }

        void detach()
        {
            _buffer.detach();
            this->clear();
        }

        void terminate()
        {
            _buffer.terminate();
        }

        BasicTextBuffer<intern_type, extern_type>& buffer()
        { return _buffer; }

    private:
        BasicTextBuffer<intern_type, extern_type> _buffer;
};

/** @brief Converts character sequences using a Codec.

    This generic stream, which only supports input and output, wraps another input-stream and converts the
    character data which is received from the stream on-the-fly using a Codec. The data which is
    received from the wrapped input-stream is buffered.

    This class derives from std::basic_istream which is the super-class of input-stream classes.
    Stream classes are used to connect to an external device and transport characters
    from this external device.

    The internal character set can be specified using the template parameters 'I', the
    external character set using 'E'. The external type is the input type and output
    type when reading from or writing to the external device. The internal type is the type
    which is used to internally store the data from the external device after the external
    format was converted using the Codec which is passed when constructing an object of this
    class.

    The Codec object which is passed as pointer to the constructor will afterwards be
    managed by this class and also be deleted by this class when it's destructed!

    @see std::basic_istream
*/
template <typename CharT, typename ByteT>
class BasicTextStream : public std::basic_iostream<CharT>
{
    public:
        typedef ByteT extern_type;
        typedef CharT intern_type;
        typedef CharT char_type;
        typedef typename std::char_traits<CharT> traits_type;
        typedef typename traits_type::int_type int_type;
        typedef typename traits_type::pos_type pos_type;
        typedef typename traits_type::off_type off_type;
        typedef std::basic_iostream<extern_type> StreamType;
        typedef TextCodec<char_type, extern_type> CodecType;

    public:
        /** @brief Construct by stream and codec.

            The stream @a ios is used to read a character sequences and
            convert is using the codec @a codec and write character
            sequences which have been converted using the codec @a codec.
            The codec object which is passed as pointer will afterwards
            be managed by this class and be deleted on destruction
        */
        BasicTextStream(StreamType& ios, CodecType* codec)
        : std::basic_iostream<intern_type>(0)
        , _buffer( &ios, codec)
        { init(&_buffer); }

        BasicTextStream(CodecType* codec)
        : std::basic_iostream<intern_type>(0)
        , _buffer(0, codec)
        { init(&_buffer); }

        //! @brief Deletes to codec.
        ~BasicTextStream()
        { }

        void attach(StreamType& ios)
        {
            _buffer.attach( ios );
            this->clear();
        }

        void detach()
        {
            _buffer.detach();
            this->clear();
        }

        void terminate()
        {
            _buffer.terminate();
        }

        BasicTextBuffer<intern_type, extern_type>& buffer()
        { return _buffer; }

    private:
        BasicTextBuffer<intern_type, extern_type> _buffer;
};


/** @brief Converts 8-bit character sequences to unicode.
	*
	* The internal type is Pt::Char. The external type is $char$.
	*
	* See BasicTextBuffer for a more detailed description.
	*/
class CXXTOOLS_API TextBuffer : public BasicTextBuffer<cxxtools::Char, char>
{
	public:
		typedef TextCodec<cxxtools::Char, char> Codec;

	public:
		/** @brief Constructs a new TextBuffer object using the given stream buffer as external device and
			* the codec for character conversion.
			*
			* See BasicTextBuffer::BasicTextBuffer() for a more detailed description.
			*
			* @param buffer The buffer (external device) which is wrapped by this object.
			* @param codec The codec which is used to convert data from and to the external device.
			*/
		TextBuffer(std::ios* buffer, Codec* codec);
};


/** @brief Specialized class derived from BasicTextIStream using cxxtools::Char and $char$
    * as internal and external type.
    *
    * The internal type is cxxtools::Char. The external type is $char$.
    *
    * See BasicTextIStream for a more detailed description.
    */
class CXXTOOLS_API TextIStream : public BasicTextIStream<Char, char>
{
    public:
        typedef TextCodec<cxxtools::Char, char> Codec;

    public:
        /** @brief Constructs a new TextIStream object using the given input-stream as external device and
            * the codec for character conversion.
            *
            * See BasicTextIStream::BasicTextIStream() for a more detailed description.
            *
            * @param is The input-stream (external device) which is wrapped by this object.
            * @param codec The codec which is used to convert data from the external device.
            */
        TextIStream(std::istream& is, Codec* codec);

        TextIStream(Codec* codec);

        //! @brief Destructs this object freeing the internal buffer.
        ~TextIStream();
};


/** @brief Specialized class derived from BasicTextOStream using cxxtools::Char and $char$
    * as internal and external type.
    *
    * The internal type is cxxtools::Char. The external type is $char$.
    *
    * See BasicTextOStream for a more detailed description.
    */
class CXXTOOLS_API TextOStream : public BasicTextOStream<Char, char>
{
    public:
        typedef TextCodec<cxxtools::Char, char> Codec;

    public:
        /** @brief Constructs a new TextOStream object using the given output-stream as external device and
            * the codec for character conversion.
            *
            * See BasicTextOStream::BasicTextOStream() for a more detailed description.
            *
            * @param os The output-stream (external device) which is wrapped by this object.
            * @param codec The codec which is used to convert data to the external device.
            */
        TextOStream(std::ostream& os, Codec* codec);

        TextOStream(Codec* codec);

        //! @brief Destructs this object freeing the internal buffer.
        ~TextOStream();
};

/** @brief Specialized class derived from BasicTextStream using cxxtools::Char and $char$
    * as internal and external type.
    *
    * The internal type is cxxtools::Char. The external type is $char$.
    *
    * See BasicTextStream for a more detailed description.
    */
class CXXTOOLS_API TextStream : public BasicTextStream<Char, char>
{
    public:
        typedef TextCodec<cxxtools::Char, char> Codec;

    public:
        /** @brief Constructs a new TextStream object using the given I/O-stream as external device and
            * the codec for character conversion.
            *
            * See BasicTextStream::BasicTextStream() for a more detailed description.
            *
            * @param ios The I/O-stream (external device) which is wrapped by this object.
            * @param codec The codec which is used to convert data from or to the external device.
            */
        TextStream(std::iostream& ios, Codec* codec);

        TextStream(Codec* codec);

        //! @brief Destructs this object freeing the internal buffer.
        ~TextStream();
};

} // namespace cxxtools

#endif

