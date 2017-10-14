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
#include "sslcertificateimpl.h"

namespace cxxtools
{
SslCertificate::SslCertificate(SslCertificateImpl* impl)
    : _impl(impl)
{
    if (_impl)
        _impl->addRef();
}

SslCertificate::SslCertificate(SslCertificate& s)
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

}
