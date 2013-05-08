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
#ifndef cxxtools_TextCodec_h
#define cxxtools_TextCodec_h

#include <cxxtools/api.h>
#include <cxxtools/char.h>
#include <cxxtools/conversionerror.h>
#include <string>

#ifdef CXXTOOLS_WITH_STD_LOCALE

namespace std {

template<>
class CXXTOOLS_API codecvt<cxxtools::Char, char, cxxtools::MBState> : public codecvt_base, public locale::facet
{
    public:
        static locale::id id;
        virtual locale::id& __get_id (void) const { return id; }

    public:
        explicit codecvt(size_t ref = 0);

        virtual ~codecvt();

        codecvt_base::result out(cxxtools::MBState& state,
                                 const cxxtools::Char* from,
                                 const cxxtools::Char* from_end,
                                 const cxxtools::Char*& from_next,
                                 char* to, 
                                 char* to_end, 
                                 char*& to_next) const
        { return this->do_out(state, from, from_end, from_next, to, to_end, to_next); }

        codecvt_base::result unshift(cxxtools::MBState& state,
                                     char* to, 
                                     char* to_end,
                                     char*& to_next) const
        { return this->do_unshift(state, to, to_end, to_next); }

        codecvt_base::result in(cxxtools::MBState& state,
                                const char* from,
                                const char* from_end, 
                                const char*& from_next,
                                cxxtools::Char* to,
                                cxxtools::Char* to_end,
                                cxxtools::Char*& to_next) const
        { return this->do_in(state, from, from_end, from_next, to, to_end, to_next); }

        int encoding() const
        { return this->do_encoding(); }

        bool always_noconv() const
        { return this->do_always_noconv(); }

        int length(cxxtools::MBState& state, const char* from,
                   const char* end, size_t max) const
        { return this->do_length(state, from, end, max); }

        int max_length() const
        { return this->do_max_length(); }

    protected:
        virtual codecvt_base::result do_out(cxxtools::MBState& state,
                                            const cxxtools::Char* from,
                                            const cxxtools::Char* from_end,
                                            const cxxtools::Char*& from_next,
                                            char* to, 
                                            char* to_end, 
                                            char*& to_next) const = 0;

        virtual codecvt_base::result do_unshift(cxxtools::MBState& state,
                                                char* to,
                                                char* to_end, 
                                                char*& to_next) const = 0;

        virtual codecvt_base::result do_in(cxxtools::MBState& state,
                                           const char* from, 
                                           const char* from_end,
                                           const char*& from_next, 
                                           cxxtools::Char* to,
                                           cxxtools::Char* to_end,
                                           cxxtools::Char*& to_next) const = 0;

        virtual int do_encoding() const throw() = 0;

        virtual bool do_always_noconv() const throw() = 0;

        virtual int do_length(cxxtools::MBState&,
                              const char* from,
                              const char* end, 
                              size_t max) const = 0;

        virtual int do_max_length() const throw() = 0;
};


template<>
class CXXTOOLS_API codecvt<char, char, cxxtools::MBState> : public codecvt_base, public locale::facet
{
    public:
        static locale::id id;
        virtual locale::id& __get_id (void) const { return id; }

    public:
        explicit codecvt(size_t ref = 0);

        virtual ~codecvt();

        codecvt_base::result out(cxxtools::MBState& state,
                                 const char* from,
                                 const char* from_end, 
                                 const char*& from_next,
                                 char* to, 
                                 char* to_end, 
                                 char*& to_next) const
        { return this->do_out(state, from, from_end, from_next, to, to_end, to_next); }

        codecvt_base::result unshift(cxxtools::MBState& state,
                                     char* to, 
                                     char* to_end,
                                     char*& to_next) const
        { return this->do_unshift(state, to, to_end, to_next); }

        codecvt_base::result in(cxxtools::MBState& state,
                                const char* from,
                                const char* from_end, 
                                const char*& from_next,
                                char* to, char* to_end, 
                                char*& to_next) const
        { return this->do_in(state, from, from_end, from_next, to, to_end, to_next); }

        int encoding() const
        { return this->do_encoding(); }

        bool always_noconv() const
        { return this->do_always_noconv(); }

        int length(cxxtools::MBState& state, const char* from,
                   const char* end, size_t max) const
        { return this->do_length(state, from, end, max); }

        int max_length() const
        { return this->do_max_length(); }

    protected:
        virtual codecvt_base::result do_out(cxxtools::MBState& state,
                                            const char* from,
                                            const char* from_end, 
                                            const char*& from_next,
                                            char* to, 
                                            char* to_end, 
                                            char*& to_next) const = 0;

        virtual codecvt_base::result do_unshift(cxxtools::MBState& state,
                                                char* to,
                                                char* to_end, 
                                                char*& to_next) const = 0;

        virtual codecvt_base::result do_in(cxxtools::MBState& state,
                                           const char* from, 
                                           const char* from_end,
                                           const char*& from_next, 
                                           char* to, 
                                           char* to_end,
                                           char*& to_next) const = 0;

        virtual int do_encoding() const throw() = 0;

        virtual bool do_always_noconv() const throw() = 0;

        virtual int do_length(cxxtools::MBState&,
                              const char* from,
                              const char* end, 
                              size_t max) const = 0;

        virtual int do_max_length() const throw() = 0;
}; 

}

#else // no CXXTOOLS_WITH_STD_LOCALE

namespace std {

class codecvt_base
{
    public:
        enum { ok, partial, error, noconv };
        typedef int result;

        virtual ~codecvt_base()
        { }
};

template <typename I, typename E, typename S>
class codecvt : public std::codecvt_base
{
    public:
        typedef I InternT;
        typedef E ExternT;
        typedef S StateT; 

    public: 
        explicit codecvt(size_t ref = 0)
        {}

        virtual ~codecvt()
        { }

        codecvt_base::result out(StateT& state, 
                                 const InternT* from,
                                 const InternT* from_end, 
                                 const InternT*& from_next,
                                 ExternT* to, 
                                 ExternT* to_end, 
                                 ExternT*& to_next) const
        { return this->do_out(state, from, from_end, from_next, to, to_end, to_next); }

        codecvt_base::result unshift(StateT& state, 
                                     ExternT* to, 
                                     ExternT* to_end,
                                     ExternT*& to_next) const
        { return this->do_unshift(state, to, to_end, to_next); }

        codecvt_base::result in(StateT& state, 
                                const ExternT* from,
                                const ExternT* from_end, 
                                const ExternT*& from_next,
                                InternT* to, 
                                InternT* to_end, 
                                InternT*& to_next) const
        { return this->do_in(state, from, from_end, from_next, to, to_end, to_next); }

        int encoding() const
        { return this->do_encoding(); }

        bool always_noconv() const
        { return this->do_always_noconv(); }

        int length(StateT& state, const ExternT* from,
                   const ExternT* end, size_t max) const
        { return this->do_length(state, from, end, max); }

        int max_length() const
        { return this->do_max_length(); }

    protected:
        virtual result do_in(StateT& s, const ExternT* fromBegin,
                             const ExternT* fromEnd, const ExternT*& fromNext,
                             InternT* toBegin, InternT* toEnd, InternT*& toNext) const = 0;

        virtual result do_out(StateT& s, const InternT* fromBegin,
                              const InternT* fromEnd, const InternT*& fromNext,
                              ExternT* toBegin, ExternT* toEnd, ExternT*& toNext) const = 0;

        virtual bool do_always_noconv() const = 0;

        virtual int do_length(StateT& s, const ExternT* fromBegin, 
                              const ExternT* fromEnd, size_t max) const = 0;

        virtual int do_max_length() const = 0;

        virtual std::codecvt_base::result do_unshift(StateT&, 
                                                     ExternT*, 
                                                     ExternT*, 
                                                     ExternT*&) const = 0;

        virtual int do_encoding() const = 0;
};

}

#endif // CXXTOOLS_WITH_STD_LOCALE

namespace cxxtools {

/**
 * @brief Generic TextCodec class/facet which may be subclassed by specific Codec classes.
 *
 * This class contains default implementations for the methods do_unshift(), do_encoding()
 * and do_always_noconv() so sub-classes do not have to implement this default behaviour.
 *
 * Codecs are used to convert one Text-encoding into another Text-encoding. The internal
 * and external data type can be specified using the template parameter 'I' (internal) and
 * 'E' (external).
 *
 * When used on a platform which supports locales and facets the conversion may use
 * locale-specific conversion of the Text.
 *
 * This class derives from facet std::codecvt. Further documentation can be found there.
 *
 * @param I The character type associated with the internal code set.
 * @param E The character type associated with the external code set.
 *
 * @see Utf8Codec
 * @see Utf16Codec
 * @see Utf32Codec
 */
template <typename I, typename E>
class TextCodec : public std::codecvt<I, E, cxxtools::MBState>
{
    public:
        typedef I InternT;
        typedef E ExternT;

    public:
        /**
         * @brief Constructs a new TextCodec object.
         *
         * The internal and external type are specified by the template parameters of the class.
         *
         * @param ref This parameter is passed to std::codecvt. When ref == 0 the locale takes care
         * of deleting the facet. If ref == 1 the locale does not destroy the facet.
         */
        explicit TextCodec(size_t ref = 0)
        : std::codecvt<InternT, ExternT, MBState>(ref)
        , _refs(ref)
        {}

    public:
        //! Empty desctructor
        virtual ~TextCodec()
        {}

        size_t refs() const
        { return _refs; }

    private:
        size_t _refs;
};

/** @brief helper template function for decoding data using a codec.
 
    This template function makes it easy to use a codec to convert a string with
    one encoding to another.
 */
template <typename CodecType>
std::basic_string<typename CodecType::InternT> decode(const typename CodecType::ExternT* data, unsigned size)
{
    CodecType codec;

    typename CodecType::InternT to[64];
    MBState state;
    std::basic_string<typename CodecType::InternT> ret;
    const typename CodecType::ExternT* from = data;

    typename CodecType::result r;
    do
    {
        typename CodecType::InternT* to_next = to;

        const typename CodecType::ExternT* from_next = from;
        r = codec.in(state, from, from + size, from_next, to, to + sizeof(to)/sizeof(typename CodecType::InternT), to_next);

        if (r == CodecType::error)
            throw ConversionError("character conversion failed");

        if (r == CodecType::partial && from_next == from)
            throw ConversionError("character conversion failed - unexpected end of input sequence");

        ret.append(to, to_next);

        size -= (from_next - from);
        from = from_next;

    } while (r == CodecType::partial);

    return ret;
}

/** @brief helper template function for decoding strings using a codec.
 
    This template function makes it easy to use a codec to convert a string with
    one encoding to another.

    @code
      std::string utf8data = ...;
      cxxtools::String unicodeString = cxxtools::decode<cxxtools::Utf8Codec>(utf8data);

      std::string base64data = ...;
      std::string data = cxxtools::decode<cxxtools::Base64Codec>(base64data);
    @endcode
 */
template <typename CodecType>
std::basic_string<typename CodecType::InternT> decode(const std::basic_string<typename CodecType::ExternT>& data)
{
    return decode<CodecType>(data.data(), data.size());
}


template <typename CodecType>
std::basic_string<typename CodecType::ExternT> encode(const typename CodecType::InternT* data, unsigned size)
{
    CodecType codec;
    char to[64];
    MBState state;
    
    typename CodecType::result r;
    const typename CodecType::InternT* from = data;
    std::basic_string<typename CodecType::ExternT> ret;

    do{
        const typename CodecType::InternT* from_next;

        typename CodecType::ExternT* to_next = to;
        r = codec.out(state, from, from + size, from_next, to, to + sizeof(to), to_next);

        if (r == CodecType::error)
            throw ConversionError("character conversion failed");

        ret.append(to, to_next);

        size -= (from_next - from);
        from = from_next;

    } while (r == CodecType::partial);

    typename CodecType::ExternT* to_next = to;
    r = codec.unshift(state, to, to + sizeof(to), to_next);
    if (r == CodecType::error)
        throw ConversionError("character conversion failed");

    ret.append(to, to_next);

    return ret;
}

/** @brief helper template function for encoding strings using a codec.
 
    This template function makes it easy to use a codec to convert a string with
    one encoding to another.

    @code
      cxxtools::String unicodeString = ...;
      std::string utf8data = cxxtools::encode<cxxtools::Utf8Codec>(unicodeString);

      std::string base64data = cxxtools::encode<cxxtools::Base64Codec>("some data");
    @endcode
 */
template <typename CodecType>
std::basic_string<typename CodecType::ExternT> encode(const std::basic_string<typename CodecType::InternT>& data)
{
    return encode<CodecType>(data.data(), data.size());
}

}

#endif
