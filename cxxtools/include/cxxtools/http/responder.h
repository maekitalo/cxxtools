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

#ifndef cxxtools_Http_Responder_h
#define cxxtools_Http_Responder_h

#include <cxxtools/http/api.h>
#include <cxxtools/http/service.h>
#include <iosfwd>
#include <exception>

namespace cxxtools {

namespace http {

class Responder;
class Request;
class Reply;

class CXXTOOLS_HTTP_API Responder
{
    public:
        explicit Responder(Service& service)
            : _service(service)
        { }

        virtual ~Responder() { }

        virtual void beginRequest(std::istream& in, Request& request);
        virtual std::size_t readBody(std::istream&);
        virtual void reply(std::ostream&, Request& request, Reply& reply) = 0;
        virtual void replyError(std::ostream&, Request& request, Reply& reply, const std::exception& ex);

        void release()     { _service.releaseResponder(this); }

    private:
        Service& _service;
};

class CXXTOOLS_HTTP_API NotFoundResponder : public Responder
{
    public:
        explicit NotFoundResponder(Service& service)
            : Responder(service)
            { }

        void reply(std::ostream&, Request& request, Reply& reply);
};

class CXXTOOLS_HTTP_API NotFoundService : public Service
{
    public:
        NotFoundService()
            : _responder(*this)
            { }

        Responder* createResponder(const Request&);
        void releaseResponder(Responder*);

    private:
        NotFoundResponder _responder;
};

} // namespace http

} // namespace cxxtools

#endif
