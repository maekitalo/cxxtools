/*
 * Copyright (C) 2011 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
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
        void addService(const Regex& url, Service& service);
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
          Key(const Regex& regex_)
            : regex(regex_)
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
