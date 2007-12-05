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
#include <windows.h>

namespace cxxtools {

class DirectoryIteratorImpl {
    public:
        DirectoryIteratorImpl();

        DirectoryIteratorImpl(const char* path);

        ~DirectoryIteratorImpl();

        int ref();

        int deref();

        bool advance();

        FileSystemNode& node();

        std::string name() const;

    private:
        unsigned int    _refs;
        std::string     _path;
        FileSystemNode* _node;
        HANDLE          _findHandle;
        WIN32_FIND_DATA _current;
};


DirectoryIteratorImpl::DirectoryIteratorImpl()
: _refs(1),
  _node(0),
  _findHandle(INVALID_HANDLE_VALUE)
{
}


DirectoryIteratorImpl::DirectoryIteratorImpl(const char* path)
: _refs(1),
  _node(0),
  _findHandle(INVALID_HANDLE_VALUE)
{
    std::string firstFile = path;
    if( !firstFile.empty() && firstFile[firstFile.size()-1] != '\\' )
        firstFile += "\\";

    firstFile += '*';

    std::basic_string<TCHAR> tpath = win32::fromMultiByte( firstFile );
    _findHandle = FindFirstFile( tpath.c_str(), &_current );

    if(_findHandle == INVALID_HANDLE_VALUE)
        throw std::runtime_error("Could not open find handle.");

    _path = path;
}


DirectoryIteratorImpl::~DirectoryIteratorImpl()
{
    delete _node;

    if(_findHandle == INVALID_HANDLE_VALUE)
        ::FindClose(_findHandle);
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
    // cannot advance an unintialised iterator
    if(_findHandle == INVALID_HANDLE_VALUE) {
        return false;
    }

    // the current node becomes invalid now
    delete _node;
    _node = 0;

    // _findHandle = INVALID_HANDLE_VALUE means end
    if( FALSE == FindNextFile(_findHandle, &_current) )
    {
        ::FindClose(_findHandle);
        _findHandle = INVALID_HANDLE_VALUE;
    }

    return _findHandle != INVALID_HANDLE_VALUE;
}


FileSystemNode& DirectoryIteratorImpl::node()
{
    // reuse previously created node
    if(_node)
        return *_node;

    // build complete path
    std::string path = _path;
    if( !path.empty() && path[path.size()-1] != '\\')
        path += '\\';
    path += this->name();

    // create file system node by full path
    _node = FileSystemNode::create( path.c_str() );
    if(!_node)
        throw std::runtime_error("Unknown file system node");

    return *_node;
}


std::string DirectoryIteratorImpl::name() const
{
    if(_findHandle != INVALID_HANDLE_VALUE)
        return win32::toMultiByte( _current.cFileName );

    return "";
}

} // namespace cxxtools
