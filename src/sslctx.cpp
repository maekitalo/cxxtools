/*
 * Copyright (C) 2020 Tommi Maekitalo
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

#include "sslctx.h"
#include <cxxtools/systemerror.h>
#include <cxxtools/log.h>

#include <openssl/ssl.h>
#include "config.h"

log_define("cxxtools.sslctx")

namespace cxxtools
{

static Mutex *openssl_mutex = nullptr;
static Mutex sslMutex;
static bool sslInitialized = false;

static unsigned long pthreads_thread_id()
    { return (unsigned long)pthread_self(); }

static void pthreads_locking_callback(int mode, int n, const char* /* file */, int /* line */)
{
    if (mode & CRYPTO_LOCK)
        openssl_mutex[n].lock();
    else
        openssl_mutex[n].unlock();
}

static void thread_setup()
{
    openssl_mutex = new Mutex[CRYPTO_num_locks()];

    CRYPTO_set_id_callback(pthreads_thread_id);
    CRYPTO_set_locking_callback(pthreads_locking_callback);
}

void SslCtx::create()
{
    if (_ctx)
        return;

    MutexLock lock(sslMutex);
    if (!sslInitialized)
    {
        log_debug("SSL_library_init");
        SSL_library_init();

        SslError::checkSslError();

        thread_setup();

        sslInitialized = true;
    }

#ifdef HAVE_TLS_METHOD
    log_debug("SSL_CTX_new(TLS_method())");
    _ctx = SSL_CTX_new(TLS_method());
#else
    log_debug("SSL_CTX_new(SSLv23_method())");
    _ctx = SSL_CTX_new(SSLv23_method());
#endif

    log_debug("ctx=" << static_cast<void*>(ctx()));
    SslError::checkSslError();
}

void SslCtx::loadSslCertificateFile(const std::string& certFile, const std::string& privateKeyFile)
{
    create();

    log_debug("load ssl certificate file \"" << certFile << '"');
    int ret = SSL_CTX_use_certificate_chain_file(ctx(), certFile.c_str());
    if (ret != 1)
        SslError::checkSslError();

    std::string key = privateKeyFile.empty() ? certFile : privateKeyFile;
    log_debug("load ssl private key file \"" << key << '"');
    ret = SSL_CTX_use_PrivateKey_file(ctx(), key.c_str(), SSL_FILETYPE_PEM);
    if (ret != 1)
        SslError::checkSslError();

    log_debug("check private key");
    if (!SSL_CTX_check_private_key(ctx()))
        throw SslError("private key does not match the certificate public key", 0);

    log_debug("private key ok");
}

}
