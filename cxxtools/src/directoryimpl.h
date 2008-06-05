/***************************************************************************
 *   Copyright (C) 2006-2008 Marc Boris Duerner                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction. Specifically, if other files    *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License. This exception does not however invalidate any other         *
 *   reasons why the executable file might be covered by the GNU Library   *
 *   General Public License.                                               *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "cxxtools/api.h"
#include <string>
#include <dirent.h>

namespace cxxtools {

class DirectoryIteratorImpl
{
    public:
        DirectoryIteratorImpl();

        DirectoryIteratorImpl(const char* path);

        ~DirectoryIteratorImpl();

        const std::string& name() const;

        const std::string& path() const;

        int ref();

        int deref();

        bool advance();

    private:
        unsigned int _refs;
        mutable std::string _path;
        mutable std::string _name;
        DIR* _handle;
        ::dirent* _current;
        bool _dirty;
};


class DirectoryImpl
{
    public:
        static void create(const std::string& path);

        static void remove(const std::string& path);

        static void move(const std::string& oldName, const std::string& newName);

        static bool exists(const std::string& path);

        static void chdir(const std::string& path);

        static std::string cwd();

        static std::string curdir();

        static std::string updir();

        static std::string rootdir();

        static std::string tmpdir();

        static std::string sep();
};

} // namespace cxxtools
