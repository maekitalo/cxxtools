/*
 * Copyright (C) 2009 by Marc Boris Duerner, Tommi Maekitalo
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

#ifndef CXXTOOLS_HTTP_SERVICE_H
#define CXXTOOLS_HTTP_SERVICE_H

#include <mutex>
#include <condition_variable>
#include <vector>
#include <string>

namespace cxxtools
{

namespace http
{

class Responder;
class Request;

class Authenticator
{
    protected:
        virtual ~Authenticator()
        { }
    public:
        virtual bool checkAuth(const Request&) const = 0;
};

class Service
{
        std::vector<const Authenticator*> _authenticators;
        std::string _realm;
        std::string _authContent;

        unsigned _responderCount;
        std::mutex _mutex;
        std::condition_variable _isIdle;

    public:
        Service()
            : _responderCount(0)
        { }

        virtual ~Service() { }
        Responder* doCreateResponder(const Request&);
        void doReleaseResponder(Responder*);

        bool checkAuth(const Request& request);

        void setRealm(const std::string& realm, const std::string& content = std::string())
            { _realm = realm; _authContent = content; }

        const std::string& realm() const        { return _realm; }
        const std::string& authContent() const  { return _authContent; }

        void addAuthenticator(const Authenticator* auth)
            { _authenticators.push_back(auth); }

        void waitIdle();

    protected:
        virtual Responder* createResponder(const Request&) = 0;
        virtual void releaseResponder(Responder*) = 0;
};

class CachedServiceBase : public Service
{
        typedef std::vector<Responder*> Responders;
        Responders responders;

    public:
        ~CachedServiceBase();

    protected:
        virtual Responder* newResponder() = 0;
        Responder* createResponder(const Request& request);
        void releaseResponder(Responder* resp);

};

template <typename ResponderType>
class CachedService : public CachedServiceBase
{
    virtual Responder* newResponder()
    {
        return new ResponderType(*this);
    }
};

} // namespace http

} // namespace cxxtools

#endif
