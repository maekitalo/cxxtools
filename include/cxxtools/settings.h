/*
 * Copyright (C) 2005-2007 by Dr. Marc Boris Duerner
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
#ifndef cxxtools_Settings_h
#define cxxtools_Settings_h

#include <cxxtools/serializationinfo.h>
#include <cxxtools/serializationerror.h>

#include <iostream>

namespace cxxtools
{

class SettingsError : public cxxtools::SerializationError
{
    public:
        SettingsError(const std::string& what, unsigned line);

        //! @brief Destructor
        ~SettingsError() throw()
        {}

        unsigned line() const
        { return _line; }

    private:
        unsigned _line;
};


class Settings : public cxxtools::SerializationInfo
{
    public:
        Settings();

        void load( std::basic_istream<cxxtools::Char>& is );

        void save( std::basic_ostream<cxxtools::Char>& os ) const;

        template <typename T>
        bool getObject(T& type, const std::string& name) const
        {
            const cxxtools::SerializationInfo* si = this->findMember(name);
            if(si == 0)
                return false;

            *si >>= type;
            return true;
        }

        template <typename T>
        void setObject(const T& type, const std::string& name)
        {
            SerializationInfo& si = this->addMember(name);
            si <<= type;
        }
};

}

#endif
