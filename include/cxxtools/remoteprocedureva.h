/*
 * Copyright (C) 2025 by Tommi Maekitalo
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

#ifndef CXXTOOLS_REMOTEPROCEDUREVA_H
#define CXXTOOLS_REMOTEPROCEDUREVA_H

#include <cxxtools/remoteprocedure.h>
#include <tuple>

namespace cxxtools
{

template <typename R>
class RemoteProcedureVa : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedureVa(RemoteClient& client, const String& name)
        : RemoteProcedureBase<R>(client, name)
        { }

        RemoteProcedureVa(RemoteClient& client, const char* name)
        : RemoteProcedureBase<R>(client, String(name))
        { }

        void begin(const SerializationInfo& si)
        {
            this->_result.clearFault();
            std::vector<Decomposer<SerializationInfo>> args(si.memberCount());
            std::vector<IDecomposer*> argi(si.memberCount());
            for (unsigned n = 0; n < si.memberCount(); ++n)
            {
                args[n].begin(si.getMember(n));
                argi[n] = &args[n];
            }
            this->_r.begin(this->_result.value());
            this->client().beginCall(this->_r, *this, argi.data(), si.memberCount());
        }

        void begin()
        {
            begin(SerializationInfo());
        }

        template <typename... Args>
            void begin(Args... args)
        {
            SerializationInfo si;
            begin(SerializationInfo(), args...);
        }

        template <typename Arg, typename... Args>
        void begin(SerializationInfo&& si, const Arg& arg,  Args... args)
        {
            si.addMember() <<= arg;
            begin(std::move(si), args...);
        }

        template <typename... Args>
        void begin(const std::tuple<Args...>& tuple)
        {
            SerializationInfo si;
            si <<= tuple;
            begin(si);
        }

        R&& call(const SerializationInfo& si)
        {
            this->_result.clearFault();
            std::vector<Decomposer<SerializationInfo>> args(si.memberCount());
            std::vector<IDecomposer*> argi(si.memberCount());
            for (unsigned n = 0; n < si.memberCount(); ++n)
            {
                args[n].begin(si.getMember(n));
                argi[n] = &args[n];
            }
            this->_r.begin(this->_result.value());
            this->client().call(this->_r, *this, argi.data(), argi.size());
            return this->_result.get();
        }

        R&& call()
        {
            return call(SerializationInfo());
        }

        template <typename Arg, typename... Args>
        R&& call(SerializationInfo&& si, const Arg& arg,  Args... args)
        {
            si.addMember() <<= arg;
            return call(std::move(si), args...);
        }

        template <typename... Args>
            R&& call(Args... args)
        {
            SerializationInfo si;
            return call(SerializationInfo(), args...);
        }

        template <typename... Args>
        R&& call(const std::tuple<Args...>& tuple)
        {
            SerializationInfo si;
            si <<= tuple;
            return call(si);
        }

        R&& operator()()
        {
            return call(SerializationInfo());
        }

        template <typename... Args>
            R&& operator()(Args... args)
        {
            SerializationInfo si;
            return call(SerializationInfo(), args...);
        }

};

}

#endif
