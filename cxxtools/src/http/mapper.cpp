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

#include <cxxtools/http/service.h>
#include <cxxtools/http/request.h>
#include <cxxtools/log.h>
#include "mapper.h"

log_define("cxxtools.http.mapper")

namespace cxxtools
{
namespace http
{

void Mapper::addService(const std::string& url, Service& service)
{
    log_debug("add service for url <" << url << '>');

    WriteLock serviceLock(_serviceMutex);
    _services.push_back(ServicesType::value_type(url, &service));
}

void Mapper::addService(const Regex& url, Service& service)
{
    log_debug("add service for regex");

    WriteLock serviceLock(_serviceMutex);
    _services.push_back(ServicesType::value_type(url, &service));
}

void Mapper::removeService(Service& service)
{
    WriteLock serviceLock(_serviceMutex);
    service.waitIdle();

    ServicesType::iterator it = _services.begin();
    while (it != _services.end())
    {
        if (it->second == &service)
        {
            _services.erase(it++);
        }
        else
        {
            ++it;
        }
    }
}

Responder* Mapper::getResponder(const Request& request)
{
    log_debug("get responder for url <" << request.url() << '>');

    ReadLock serviceLock(_serviceMutex);

    for (ServicesType::const_iterator it = _services.begin();
         it != _services.end(); ++it)
    {
        if (it->first.match(request.url()))
        {
            if (!it->second->checkAuth(request))
            {
                return _noAuthService.createResponder(request, it->second->realm(), it->second->authContent());
            }

            Responder* resp = it->second->doCreateResponder(request);
            if (resp)
            {
                log_debug("got responder");
                return resp;
            }
        }
    }

    log_debug("use default responder");
    return _defaultService.createResponder(request);
}

}
}
