/*
 * Copyright (C) 2009 by Dr. Marc Boris Duerner
 * Copyright (C) 2009 by Tommi Meakitalo
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
#ifndef CXXTOOLS_SERVICEPROCEDURE_H
#define CXXTOOLS_SERVICEPROCEDURE_H

#include <cxxtools/composer.h>
#include <cxxtools/decomposer.h>
#include <cxxtools/void.h>
#include <cxxtools/typetraits.h>
#include <cxxtools/callable.h>

namespace cxxtools
{

class ServiceProcedure
{
    public:
        ServiceProcedure() = default;
        virtual ~ServiceProcedure() = default;
        ServiceProcedure(const ServiceProcedure&) = delete;
        ServiceProcedure& operator=(const ServiceProcedure&) = delete;
        ServiceProcedure(ServiceProcedure&&) = delete;
        ServiceProcedure& operator=(ServiceProcedure&&) = delete;

        virtual ServiceProcedure* clone() const = 0;

        virtual IComposers* beginCall() = 0;

        virtual IDecomposer* endCall() = 0;
};

#include <cxxtools/serviceprocedure.tpp>

}

#endif // CXXTOOLS_SERVICEPROCEDURE_H
