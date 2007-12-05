/***************************************************************************
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 Tobias Mueller                                *
 *   Copyright (C) 2006-2007 PTV AG                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
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
#include "win32.h"
#include "cxxtools/filesystemnode.h"
#include <string>
#include <stdexcept>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

namespace cxxtools {

class PT_API DirectoryIteratorImpl 
{
    public:
        DirectoryIteratorImpl();

        DirectoryIteratorImpl(const char* path);

        ~DirectoryIteratorImpl();

        int ref();

        int deref();

        bool advance();

        const char* path() const
        { return _path.c_str(); }

        FileSystemNode& node();

        std::string name() const;

    private:
        unsigned int _refs;
        std::string _path;
        FileSystemNode* _node;
        DIR* _handle;
        ::dirent* _current;
};


DirectoryIteratorImpl::DirectoryIteratorImpl()
: _refs(1),
  _node(0),
  _handle(0),
  _current(0)
{
}


DirectoryIteratorImpl::DirectoryIteratorImpl(const char* path)
: _refs(1),
  _node(0),
  _handle(0),
  _current(0)
{
    _handle = ::opendir( path );

    if( !_handle )
    {
        throw std::runtime_error("Could not open directory");
    }

    _path = path;
    this->advance();
}


DirectoryIteratorImpl::~DirectoryIteratorImpl()
{
    delete _node;

    if(_handle)
        ::closedir(_handle);
}


int DirectoryIteratorImpl::ref()
{
    return ++_refs;
}


int DirectoryIteratorImpl::deref()
{
    return --_refs;
}


bool DirectoryIteratorImpl::advance()
{
    delete _node;
    _node = 0;

    // _current == 0 means end
    _current = ::readdir( _handle );
    return _current != 0;
}


FileSystemNode& DirectoryIteratorImpl::node()
{
    // reuse previously created node
    if(_node)
        return *_node;

    // build complete path
    std::string path = _path;
    if( !path.empty() && path[path.size()] != '/')
        path += '/';
    path += this->name();

    // create file system node by full path
    _node = FileSystemNode::create( path.c_str() );
    if(!_node)
        throw std::runtime_error("Unknown file system node");

    return *_node;
}


std::string DirectoryIteratorImpl::name() const
{
    if(_current)
        return _current->d_name;

    return "";
}

} // namespace cxxtools
