/*
 * Copyright (C) 2011 Tommi Maekitalo
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

#ifndef CXXTOOLS_HTTP_MAPPER_H
#define CXXTOOLS_HTTP_MAPPER_H

#include "notfoundservice.h"
#include "notauthenticatedservice.h"
#include <map>
#include <cxxtools/regex.h>

namespace cxxtools
{
namespace http
{

class Mapper
{
    public:
        void addService(const std::string& url, Service& service);
        void addService(Regex&& url, Service& service);
        void removeService(Service& service);

        Responder* getResponder(const Request& request);
        Responder* getDefaultResponder(const Request& request)
            { return _defaultService.createResponder(request); }

    private:
        struct Key
        {
          Regex regex;
          std::string url;
          Key() { }
          Key(Regex&& regex_)
            : regex(std::move(regex_))
          { }
          Key(const std::string& url_)
            : url(url_)
          { }
          bool match(const std::string& u) const
          { return regex.empty() ? url == u
                                 : regex.match(u); }
        };
        typedef std::vector<std::pair<Key, Service*> > ServicesType;
        ReadWriteMutex _serviceMutex;
        ServicesType _services;
        NotFoundService _defaultService;
        NotAuthenticatedService _noAuthService;
};
}
}

#endif // CXXTOOLS_HTTP_MAPPER_H
