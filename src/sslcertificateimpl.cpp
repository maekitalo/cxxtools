/*
 * Copyright (C) 2017 Tommi Maekitalo
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

#include "sslcertificateimpl.h"
#include "config.h"
#include <cxxtools/utf8codec.h>
#include <vector>

#ifndef HAVE_ASN1_TIME_diff
#include <sstream>
#endif

namespace cxxtools
{
static String str(X509_NAME* a)
{
    class MemBIO
    {
        BIO* _out;
        MemBIO(const MemBIO&) { }
        MemBIO& operator=(const MemBIO&) { return *this; }

    public:
        MemBIO()
            : _out(BIO_new(BIO_s_mem()))
        { }

        ~MemBIO()
        { BIO_free(_out); }

        operator BIO* ()  { return _out; }
    };

    MemBIO out;
    X509_NAME_print_ex(out, a, 0, XN_FLAG_RFC2253 & ~ASN1_STRFLGS_ESC_MSB);
    std::vector<char> buf(BIO_number_written(out));
    BIO_read(out, &buf[0], buf.size());

    return Utf8Codec::decode(&buf[0], buf.size());
}

static std::string str(ASN1_INTEGER* a)
{
    BIGNUM* bn = ASN1_INTEGER_to_BN(a, 0);
    if (bn == 0)
        throw std::bad_alloc();

    char* ch = BN_bn2hex(bn);
    if (ch == 0)
    {
        BN_free(bn);
        throw std::bad_alloc();
    }

    BN_free(bn);
    std::string ret = ch;
    OPENSSL_free(ch);
    return ret;
}

#ifdef HAVE_ASN1_TIME_diff
static DateTime dt(ASN1_TIME* t)
{
    class Asn1Time
    {
        ASN1_TIME* _t;
        Asn1Time(const Asn1Time&) { }
        Asn1Time& operator=(const Asn1Time&) { return *this; }
    public:
        Asn1Time(time_t t)
            : _t(ASN1_TIME_set(0, t))
            { }

        ~Asn1Time()
            { ASN1_STRING_free(_t); }

        operator ASN1_TIME* ()   { return _t; }
    };

    Asn1Time epoch(0);

    int pday, psec;
    ASN1_TIME_diff(&pday, &psec, epoch, t);

    int secs = pday * (24*60*60) + psec;

    return DateTime::fromMSecsSinceEpoch(Seconds(secs));
}
#else
static DateTime dt(ASN1_TIME* time)
{
    const char* str = (const char*) time->data;
    size_t i = 0;

    int year;
    unsigned mon, mday, hour, min, sec;

    if (time->type == V_ASN1_UTCTIME)
    {/* two digit year */
        year = (str[i++] - '0') * 10;
        year += (str[i++] - '0');
        if (year < 70)
            year += 2000;
        else
            year += 1900;
    }
    else if (time->type == V_ASN1_GENERALIZEDTIME)
    {/* four digit year */
        year = (str[i++] - '0') * 1000;
        year+= (str[i++] - '0') * 100;
        year+= (str[i++] - '0') * 10;
        year+= (str[i++] - '0');
    }
    else
    {
        std::ostringstream msg;
        msg << "unknown time format in ssl certificate; type=" << time->type << " str=\"" << str << '"';
        throw std::runtime_error(msg.str());
    }

    mon  = (str[i++] - '0') * 10;
    mon += (str[i++] - '0');
    mday = (str[i++] - '0') * 10;
    mday+= (str[i++] - '0');
    hour = (str[i++] - '0') * 10;
    hour+= (str[i++] - '0');
    min  = (str[i++] - '0') * 10;
    min += (str[i++] - '0');
    sec  = (str[i++] - '0') * 10;
    sec += (str[i++] - '0');

    return DateTime(year, mon, mday, hour, min, sec);

}
#endif

SslCertificateImpl::~SslCertificateImpl()
{
    X509_free(_cert);
}

String SslCertificateImpl::getSubject() const
{
    return str(X509_get_subject_name(_cert));
}

String SslCertificateImpl::getIssuer() const
{
    return str(X509_get_issuer_name(_cert));
}

DateTime SslCertificateImpl::getNotBefore() const
{
    return dt(X509_get_notBefore(_cert));
}

DateTime SslCertificateImpl::getNotAfter() const
{
    return dt(X509_get_notAfter(_cert));
}

std::string SslCertificateImpl::getSerial() const
{
    return str(X509_get_serialNumber(_cert));
}

}
