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

#ifndef CXXTOOLS_SSLCTXIMPL_H
#define CXXTOOLS_SSLCTXIMPL_H

#include <cxxtools/sslctx.h>
#include <cxxtools/refcounted.h>

#include <openssl/ssl.h>

namespace cxxtools
{
class SslCtx::Impl : public cxxtools::AtomicRefCounted
{
    SSL_CTX* _ctx;

public:
    Impl();
    Impl(const Impl&) = delete;
    Impl& operator=(const Impl&) = delete;
    ~Impl();

    SSL_CTX* ctx() const        { return _ctx; }

    void loadCertificateFile(const std::string& certFile, const std::string& privateKeyFile);
    void setVerify(SslCtx::VERIFY_LEVEL level, const std::string& ca);
    void setProtocolVersion(PROTOCOL_VERSION min, PROTOCOL_VERSION max);
    void setCiphers(const std::string& ciphers);
    void setCipherServerPreference();
    void clearCipherServerPreference();
};
}

#endif
