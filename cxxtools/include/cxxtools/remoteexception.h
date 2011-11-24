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

#ifndef CXXTOOLS_REMOTEEXCEPTION_H
#define CXXTOOLS_REMOTEEXCEPTION_H

#include <exception>
#include <string>

namespace cxxtools
{

class RemoteException : public std::exception
{
    public:
        RemoteException()
            : _rc(0)
        { }

        RemoteException(const std::string& text, int rc)
            : _text(text),
              _rc(rc)
        { }

        ~RemoteException() throw ()
        { }

        const char* what() const throw ()
        { return _text.c_str(); }

        const std::string& text() const
        { return _text; }

        int rc() const
        { return _rc; }

        void text(const std::string& t)
        { _text = t; }

        void rc(int v)
        { _rc = v; }

        void clear()
        {
            _rc = 0;
            _text.clear();
        }

    protected:
        std::string _text;
        int _rc;
};

}

#endif // CXXTOOLS_REMOTEEXCEPTION_H
