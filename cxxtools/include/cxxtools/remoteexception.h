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
