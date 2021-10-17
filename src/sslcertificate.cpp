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

#include <cxxtools/sslcertificate.h>
#include "config.h"

#ifdef WITH_SSL
#include <cxxtools/systemerror.h>
#include "sslcertificateimpl.h"
#include <stdio.h>
#include <errno.h>
#endif

namespace cxxtools
{
#ifdef WITH_SSL
SslCertificate::SslCertificate(const std::string& filename)
    : _impl(0)
{
    X509* cert = 0;
    FILE* fp = fopen(filename.c_str(), "r");
    if (fp == 0)
        throw SystemError(errno, ("fopen(" + filename + ')').c_str());

    cert = PEM_read_X509(fp, 0, 0, 0);
    if (cert == 0)
    {
        fclose(fp);
        SslError::checkSslError();
        throw std::runtime_error("failed to read certificate from file \""  + filename + '"');
    }

    fclose(fp);
    _impl = new SslCertificateImpl(cert);
}

SslCertificate::SslCertificate(SslCertificateImpl* impl)
    : _impl(impl)
{
    if (_impl)
        _impl->addRef();
}

SslCertificate::SslCertificate(const SslCertificate& s)
    : _impl(s._impl)
{
    if (_impl)
        _impl->addRef();
}

SslCertificate& SslCertificate::operator=(const SslCertificate& s)
{
    if (_impl && _impl->release() <= 0)
        delete _impl;

    _impl = s._impl;

    if (_impl)
        _impl->addRef();

    return *this;
}

SslCertificate::~SslCertificate()
{
    if (_impl && _impl->release() <= 0)
        delete _impl;
}

void SslCertificate::clear()
{
    if (_impl)
    {
        if (_impl->release() <= 0)
            delete _impl;
        _impl = 0;
    }
}

String SslCertificate::getSubject() const
{
    if (!_impl)
        return String();
    else
        return _impl->getSubject();
}

String SslCertificate::getIssuer() const
{
    if (!_impl)
        return String();
    else
        return _impl->getIssuer();
}

DateTime SslCertificate::getNotBefore() const
{
    if (!_impl)
        return DateTime(0, 1, 1, 0, 0, 0);
    else
        return _impl->getNotBefore();
}

DateTime SslCertificate::getNotAfter() const
{
    if (!_impl)
        return DateTime(2999, 12, 31, 23, 59, 59, 999);
    else
        return _impl->getNotAfter();
}

std::string SslCertificate::getSerial() const
{
    if (!_impl)
        return std::string();
    else
        return _impl->getSerial();
}

#else

// no ssl enabled - provide just dummies

SslCertificate::SslCertificate(const std::string& filename)
    : _impl(0)
{
}

SslCertificate::SslCertificate(SslCertificateImpl* impl)
    : _impl(impl)
{
}

SslCertificate::SslCertificate(const SslCertificate& s)
    : _impl(s._impl)
{
}

SslCertificate& SslCertificate::operator=(const SslCertificate& s)
{
    return *this;
}

SslCertificate::~SslCertificate()
{
}

void SslCertificate::clear()
{
}

String SslCertificate::getSubject() const
{
    return String();
}

String SslCertificate::getIssuer() const
{
    return String();
}

DateTime SslCertificate::getNotBefore() const
{
    return DateTime(0, 1, 1, 0, 0, 0);
}

DateTime SslCertificate::getNotAfter() const
{
    return DateTime(2999, 12, 31, 23, 59, 59, 999);
}

std::string SslCertificate::getSerial() const
{
    return std::string();
}
#endif

}
