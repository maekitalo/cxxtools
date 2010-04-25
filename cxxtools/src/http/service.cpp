/*
 * Copyright (C) 2010 Tommi Maekitalo
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
#include <cxxtools/http/responder.h>

namespace cxxtools
{

namespace http
{

Responder* Service::doCreateResponder(const Request& request)
{
    MutexLock lock(_mutex);
    ++_responderCount;
    return createResponder(request);
}

void Service::doReleaseResponder(Responder* responder)
{
    MutexLock lock(_mutex);
    releaseResponder(responder);
    if (--_responderCount <= 0)
        _isIdle.signal();
}

void Service::waitIdle()
{
    MutexLock lock(_mutex);
    while (_responderCount > 0)
        _isIdle.wait(lock);
}

bool Service::checkAuth(const Request& request)
{
    for (std::vector<const Authenticator*>::const_iterator it = _authenticators.begin();
        it != _authenticators.end(); ++it)
    {
        if (!(*it)->checkAuth(request))
            return false;
    }

    return true;
}

CachedServiceBase::~CachedServiceBase()
{
    for (Responders::iterator it = responders.begin(); it != responders.end(); ++it)
        delete *it;
}

Responder* CachedServiceBase::createResponder(const Request& request)
{
    if (responders.empty())
    {
        return newResponder();
    }
    else
    {
        Responder* ret = responders.back();
        responders.pop_back();
        return ret;
    }
}

void CachedServiceBase::releaseResponder(Responder* resp)
{
    responders.push_back(resp);
}

}
}
