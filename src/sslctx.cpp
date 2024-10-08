/*
 * Copyright (C) 2022 Tommi Maekitalo
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

#include <cxxtools/sslctx.h>
#include <cxxtools/serializationinfo.h>
#include <cxxtools/serializationerror.h>
#include <cxxtools/log.h>
#include "sslctximpl.h"

log_define("cxxtools.sslctx")

namespace cxxtools
{

static void release(SslCtx::Impl* impl)
{
    if (impl && impl->release() == 0)
        delete impl;
}

SslCtx::SslCtx(const SslCtx& sslCtx)
    : _impl(sslCtx._impl)
{
    if (_impl)
        _impl->addRef();
}

SslCtx& SslCtx::operator= (const SslCtx& sslCtx)
{
    if (_impl != sslCtx._impl)
    {
        release(_impl);
        _impl = sslCtx._impl;
        if (_impl)
            _impl->addRef();
    }

    return *this;
}

SslCtx::~SslCtx()
{
    release(_impl);
}

SslCtx& SslCtx::enable(bool sw)
{
    if (sw)
    {
        impl();
    }
    else
    {
        release(_impl);
        _impl = nullptr;
    }

    return *this;
}

SslCtx& SslCtx::loadCertificateFile(const std::string& certFile, const std::string& privateKeyFile)
{
    impl()->loadCertificateFile(certFile, privateKeyFile);
    return *this;
}

SslCtx& SslCtx::setVerify(VERIFY_LEVEL level, const std::string& ca)
{
    impl()->setVerify(level, ca);
    return *this;
}

SslCtx& SslCtx::setVerify(const std::string& ca)
{
    impl()->setVerify(VERIFY_LEVEL::REQUIRE, ca);
    return *this;
}

SslCtx& SslCtx::setProtocolVersion(PROTOCOL_VERSION min_version, PROTOCOL_VERSION max_version)
{
    impl()->setProtocolVersion(min_version, max_version);
    return *this;
}

SslCtx& SslCtx::setCiphers(const std::string& ciphers)
{
    impl()->setCiphers(ciphers);
    return *this;
}

SslCtx& SslCtx::setCipherServerPreference(bool sw)
{
    if (sw)
        impl()->setCipherServerPreference();
    else
        impl()->clearCipherServerPreference();
    return *this;
}

SslCtx SslCtx::secure()
{
    log_debug("SslCtx SslCtx::secure()");
    SslCtx result;
    result.setProtocolVersion(PROTOCOL_VERSION::TLSv12, PROTOCOL_VERSION::TLSv13);
    result.setCiphers("ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA256");
    return result;
}


SslCtx::Impl* SslCtx::impl()
{
    if (!_impl)
    {
        _impl = new SslCtx::Impl();
        _impl->addRef();
    }

    return _impl;
}

const SslCtx::Impl* SslCtx::impl() const
{
    return const_cast<SslCtx*>(this)->impl();
}

void operator<<= (cxxtools::SerializationInfo& si, const SslCtx::PROTOCOL_VERSION& protocolVersion)
{
    switch (protocolVersion)
    {
        case SslCtx::PROTOCOL_VERSION::SSLv2: si <<= "SSLv2"; break;
        case SslCtx::PROTOCOL_VERSION::SSLv3: si <<= "SSLv3"; break;
        case SslCtx::PROTOCOL_VERSION::TLSv1: si <<= "TLSv1"; break;
        case SslCtx::PROTOCOL_VERSION::TLSv11: si <<= "TLSv11"; break;
        case SslCtx::PROTOCOL_VERSION::TLSv12: si <<= "TLSv12"; break;
        case SslCtx::PROTOCOL_VERSION::TLSv13: si <<= "TLSv13"; break;
    }
}

void operator>>= (const cxxtools::SerializationInfo& si, SslCtx::PROTOCOL_VERSION& protocolVersion)
{
    std::string value;
    si >>= value;
    if (value == "SSLv2")
        protocolVersion = cxxtools::SslCtx::PROTOCOL_VERSION::SSLv2;
    else if (value == "SSLv3")
        protocolVersion = cxxtools::SslCtx::PROTOCOL_VERSION::SSLv3;
    else if (value == "TLSv1")
        protocolVersion = cxxtools::SslCtx::PROTOCOL_VERSION::TLSv1;
    else if (value == "TLSv11")
        protocolVersion = cxxtools::SslCtx::PROTOCOL_VERSION::TLSv11;
    else if (value == "TLSv12")
        protocolVersion = cxxtools::SslCtx::PROTOCOL_VERSION::TLSv12;
    else if (value == "TLSv13")
        protocolVersion = cxxtools::SslCtx::PROTOCOL_VERSION::TLSv13;
    else if (!value.empty())
        throw SerializationError("invalid protocol version <" + value + '>');
}

void operator<<= (cxxtools::SerializationInfo& si, const SslCtx::VERIFY_LEVEL& verifyLevel)
{
    switch (verifyLevel)
    {
        case SslCtx::VERIFY_LEVEL::NONE:        si <<= "NONE";      break;
        case SslCtx::VERIFY_LEVEL::OPTIONAL:    si <<= "OPTIONAL";  break;
        case SslCtx::VERIFY_LEVEL::REQUIRE:     si <<= "REQUIRE";   break;
    }
}

void operator>>= (const cxxtools::SerializationInfo& si, SslCtx::VERIFY_LEVEL& verifyLevel)
{
    std::string value;
    si >>= value;
    if (value == "NONE" || value == "none" || value == "0")
        verifyLevel = cxxtools::SslCtx::VERIFY_LEVEL::NONE;
    else if (value == "OPTIONAL" || value == "optional" || value == "1")
        verifyLevel = cxxtools::SslCtx::VERIFY_LEVEL::OPTIONAL;
    else if (value == "REQUIRE" || value == "require" || value == "2")
        verifyLevel = cxxtools::SslCtx::VERIFY_LEVEL::REQUIRE;
    else
        throw SerializationError("invalid verify level <" + value + '>');
}
}
