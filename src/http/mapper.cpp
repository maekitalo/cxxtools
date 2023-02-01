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

    WriteLockType serviceLock(_serviceMutex);
    _services.push_back(ServicesType::value_type(url, &service));
}

void Mapper::addService(Regex&& url, Service& service)
{
    log_debug("add service for regex");

    WriteLockType serviceLock(_serviceMutex);
    _services.push_back(ServicesType::value_type(std::move(url), &service));
}

void Mapper::removeService(Service& service)
{
    WriteLockType serviceLock(_serviceMutex);
    service.waitIdle();

    ServicesType::size_type n = 0;
    while (n < _services.size())
    {
        if (_services[n].second == &service)
        {
            _services.erase(_services.begin() + n);
        }
        else
        {
            ++n;
        }
    }
}

Responder* Mapper::getResponder(const Request& request)
{
    log_debug("get responder for url <" << request.url() << '>');

    ReadLockType serviceLock(_serviceMutex);

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
