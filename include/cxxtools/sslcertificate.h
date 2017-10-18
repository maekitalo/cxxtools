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

#ifndef CXXTOOLS_SSLCERTIFICATE_H
#define CXXTOOLS_SSLCERTIFICATE_H

#include <cxxtools/string.h>
#include <cxxtools/datetime.h>
#include <string>

namespace cxxtools
{

class SslCertificate
{
        class SslCertificateImpl* _impl;

    public:
        SslCertificate()
            : _impl(0)
            { }

        /// Reads certificate from file.
        explicit SslCertificate(const std::string& filename);

        explicit SslCertificate(SslCertificateImpl* impl);

        SslCertificate(SslCertificate& s);
        SslCertificate& operator=(const SslCertificate& s);
        ~SslCertificate();

        operator bool() const     { return _impl != 0; }
        void clear();
        String getSubject() const;
        String getIssuer() const;
        DateTime getNotBefore() const;
        DateTime getNotAfter() const;
        std::string getSerial() const;
        bool isValid(const DateTime& dt = DateTime::gmtime()) const
            { return dt >= getNotBefore() && dt <= getNotAfter(); }
};

} // namespace cxxtools

#endif
