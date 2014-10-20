/*
 * Copyright (C) 2014 Tommi Maekitalo
 *
 */

#ifndef CXXTOOLS_LIMITSTREAM_H
#define CXXTOOLS_LIMITSTREAM_H

#include <iostream>
#include <algorithm>

namespace cxxtools
{
    /**
      The class implements a stream buffer, which reads or writes from another streambuffer
      but limits the number of characters.

      There are separate counters for reading and writing. They may be reset whenever
      needed.

      A buffer is used for reading. It is not guaranteed, that characters not yet read
      from this stream buffer can be read from the underlying stream buffer.

     */
    template <typename char_type>
    class BasicLimitStreambuf : public std::basic_streambuf<char_type>
    {
            std::basic_streambuf<char_type>* _sb;
            unsigned _icount;
            unsigned _ocount;
            unsigned _ibufsize;
            char_type* _ibuffer;

            typedef typename std::basic_streambuf<char_type>::traits_type traits_type;
            typedef typename std::basic_streambuf<char_type>::int_type int_type;

        public:
            /// initialize a BasicLimitStreambuf with a underlying stream.
            BasicLimitStreambuf(std::basic_streambuf<char_type>* sb,
                unsigned icount, unsigned ocount, unsigned ibufsize = 8192)
                : _sb(sb),
                  _icount(icount),
                  _ocount(ocount),
                  _ibufsize(ibufsize),
                  _ibuffer(0)
            { }

            ~BasicLimitStreambuf()
            { delete _ibuffer; }

            /// returns the number of remaining characters to read including characters in the buffer.
            unsigned icount() const
            { return _icount + (this->egptr() - this->gptr()); }

            /// sets the number of remaining characters to read.
            /// When there are characters in the buffer already, they are not counted
            /// i.e. when there are 10 bytes in the buffer the call to icount(20)
            /// leads to actually 30 bytes to read.
            void icount(unsigned c)
            { _icount = c; }

            /// returns the number of remaining characters to write.
            unsigned ocount() const
            { return _ocount; }

            /// sets the number of remaining characters to write.
            void ocount(unsigned c)
            { _ocount = c; }

            /// returns the underlying stream buffer.
            std::basic_streambuf<char_type>* streambuf()
            { return _sb; }

            /// sets the underlying stream buffer.
            void attach(std::basic_streambuf<char_type>* s)
            { _sb = s; }

        protected:
            char_type* ibuffer()
            {
                if (_ibuffer == 0)
                    _ibuffer = new char_type[_ibufsize];
                return _ibuffer;
            }

            std::streamsize showmanyc()
            {
                if (this->gptr() == this->egptr() && _icount > 0)
                {
                    std::streamsize n = _sb->in_avail();

                    if (n > 0)
                    {
                        n = _sb->sgetn(ibuffer(), std::min(static_cast<std::streamsize>(_icount), n));
                        this->setg(_ibuffer, _ibuffer, _ibuffer + n);

                        _icount -= n;
                    }
                }

                return this->egptr() - this->gptr();
            }

            int_type overflow(int_type ch)
            {
                if (ch != traits_type::eof() && _ocount > 0)
                {
                    int r = _sb->sputc(traits_type::to_char_type(ch));
                    if (r == traits_type::eof())
                        return traits_type::eof();
                    --_ocount;
                    return traits_type::not_eof(ch);
                }

                return traits_type::eof();
            }

            int_type underflow()
            {
                if (_icount == 0)
                    return traits_type::eof();

                std::streamsize c = _sb->sgetn(ibuffer(), std::min(_icount, _ibufsize));
                if (c == 0)
                    return traits_type::eof();

                _icount -= c;

                this->setg(_ibuffer, _ibuffer, _ibuffer + c);

                return this->sgetc();
            }

            int sync()
            {
                return 0;
            }
    };

    /**
       The class implements a input stream, which limits the number of characters to read.

       When trying to read more than the specified number of characters, eof is reported.
    */
    template <typename char_type>
    class BasicLimitIStream : public std::basic_istream<char_type>
    {
            BasicLimitStreambuf<char_type> _streambuf;

        public:
            /// Initialize a limiting input stream using a underlying input stream
            /// and the number of characters to read.
            explicit BasicLimitIStream(std::basic_istream<char_type>& source, unsigned count = 0)
                : std::basic_istream<char_type>(0),
                 _streambuf(source.rdbuf(), count, 0)
            {
                std::basic_istream<char_type>::init(&_streambuf);
            }

            /// Initialize a limiting input stream using a underlying stream buffer
            /// and the number of characters to read.
            BasicLimitIStream(std::basic_streambuf<char_type>* source, unsigned count = 0)
                : std::basic_istream<char_type>(0),
                 _streambuf(source, count, 0)
            {
                std::basic_istream<char_type>::init(&_streambuf);
            }

            /// returns the number of remaining characters to read.
            unsigned icount() const
            { return _streambuf.icount(); }

            /// sets the number of remaining characters to read.
            void icount(unsigned c)
            { _streambuf.icount(c); }

            /// sets the underlying stream buffer.
            void attach(std::basic_istream<char_type>& s)
            { _streambuf.attach(s.rdbuf()); }

            /// sets the underlying stream buffer.
            void attach(std::basic_streambuf<char_type>* s)
            { _streambuf.attach(s); }

    };

    /**
       The class implements a output stream, which limits the number of characters to write.

       When trying to write more than the specified number of characters, eof is reported.
     */
    template <typename char_type>
    class BasicLimitOStream : public std::basic_ostream<char_type>
    {
            BasicLimitStreambuf<char_type> _streambuf;

        public:
            /// Initialize a limiting output stream using a underlying output stream
            /// and the number of characters to write.
            explicit BasicLimitOStream(std::basic_ostream<char_type>& sink, unsigned count = 0)
                : std::basic_ostream<char_type>(0),
                 _streambuf(sink.rdbuf(), 0, count)
            {
                std::basic_ostream<char_type>::init(&_streambuf);
            }

            /// Initialize a limiting output stream using a underlying stream buffer
            /// and the number of characters to write.
            explicit BasicLimitOStream(std::basic_streambuf<char_type>* sink, unsigned count = 0)
                : std::basic_ostream<char_type>(0),
                 _streambuf(sink, 0, count)
            {
                std::basic_ostream<char_type>::init(&_streambuf);
            }

            /// sets the number of remaining characters to write.
            unsigned ocount() const
            { return _streambuf.ocount(); }

            /// returns the number of remaining characters to write.
            void ocount(unsigned c)
            { _streambuf.ocount(c); }

            /// sets the underlying stream buffer.
            void attach(std::basic_ostream<char_type>& s)
            { _streambuf.attach(s.rdbuf()); }

            /// sets the underlying stream buffer.
            void attach(std::basic_streambuf<char_type>* s)
            { _streambuf.attach(s); }

    };

    /**
       This class implements a input/output stream, which limits the number of characters to read and write.
     */
    template <typename char_type>
    class BasicLimitIOStream : public std::basic_iostream<char_type>
    {
            BasicLimitStreambuf<char_type> _streambuf;

        public:
            /// Initialize a limiting I/O stream using a underlying output stream,
            /// the number of characters to read and the number of characters to write.
            BasicLimitIOStream(std::basic_iostream<char_type>& sinksource, unsigned icount, unsigned ocount)
                : std::basic_iostream<char_type>(0),
                 _streambuf(sinksource.rdbuf(), icount, ocount)
            {
                std::basic_iostream<char_type>::init(&_streambuf);
            }

            /// Initialize a limiting I/O stream using a underlying output stream.
            /// The number of characters to read and the number of characters to write are initialized to 0.
            BasicLimitIOStream(std::basic_iostream<char_type>& sinksource)
                : std::basic_iostream<char_type>(0),
                 _streambuf(sinksource.rdbuf(), 0, 0)
            {
                std::basic_iostream<char_type>::init(&_streambuf);
            }

            /// Initialize a limiting I/O stream using a underlying stream buffer,
            /// the number of characters to read and the number of characters to write.
            BasicLimitIOStream(std::basic_streambuf<char_type>* sinksource, unsigned icount, unsigned ocount)
                : std::basic_iostream<char_type>(0),
                 _streambuf(sinksource, icount, ocount)
            {
                std::basic_iostream<char_type>::init(&_streambuf);
            }

            /// Initialize a limiting I/O stream using a underlying stream buffer.
            /// The number of characters to read and the number of characters to write are initialized to 0.
            BasicLimitIOStream(std::basic_streambuf<char_type>* sinksource)
                : std::basic_iostream<char_type>(0),
                 _streambuf(sinksource, 0, 0)
            {
                std::basic_iostream<char_type>::init(&_streambuf);
            }

            /// returns the number of remaining characters to read.
            unsigned icount() const
            { return _streambuf.icount(); }

            /// sets the number of remaining characters to read.
            void icount(unsigned c)
            { _streambuf.icount(c); }

            /// returns the number of remaining characters to write.
            unsigned ocount() const
            { return _streambuf.ocount(); }

            /// sets the number of remaining characters to write.
            void ocount(unsigned c)
            { _streambuf.ocount(c); }

            /// sets the underlying stream buffer.
            void attach(std::basic_iostream<char_type>& s)
            { _streambuf.attach(s.rdbuf()); }

            /// sets the underlying stream buffer.
            void attach(std::basic_streambuf<char_type>* s)
            { _streambuf.attach(s); }

    };

    typedef BasicLimitStreambuf<char> LimitStreambuf;
    typedef BasicLimitIStream<char> LimitIStream;
    typedef BasicLimitOStream<char> LimitOStream;
    typedef BasicLimitIOStream<char> LimitIOStream;
}

#endif // CXXTOOLS_LIMITSTREAM_H

