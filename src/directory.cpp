/*
 * Copyright (C) 2006-2008 Marc Boris Duerner
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
#include "directoryimpl.h"
#include "cxxtools/directory.h"

namespace cxxtools
{

DirectoryIterator::DirectoryIterator()
: _impl(0)
{ }


DirectoryIterator::DirectoryIterator(const std::string& path, bool skipHidden)
: _impl(new DirectoryIteratorImpl( path.c_str(), skipHidden ))
{
    if (!_impl->advance())
    {
        delete _impl;
        _impl = 0;
    }
}


DirectoryIterator::DirectoryIterator(const DirectoryIterator& it)
: _impl(it._impl)
{
    if (_impl)
        _impl->ref();
}


DirectoryIterator::~DirectoryIterator()
{
    if (_impl && 0 == _impl->deref())
    {
        delete _impl;
    }
}


DirectoryIterator& DirectoryIterator::operator++()
{
    if (_impl && !_impl->advance())
    {
        if (0 == _impl->deref())
            delete _impl;
        _impl = 0;
    }

    return *this;
}


DirectoryIterator& DirectoryIterator::operator=(const DirectoryIterator& it)
{
    if (_impl == it._impl)
        return *this;

    if (_impl && 0 == _impl->deref())
    {
        delete _impl;
    }

    _impl = it._impl;

    if (_impl)
        _impl->ref();

    return *this;
}


const std::string& DirectoryIterator::path() const
{
    return _impl->path();
}


const std::string& DirectoryIterator::operator*() const
{
    return _impl->name();
}


const std::string* DirectoryIterator::operator->() const
{
    return &_impl->name();
}


FileInfo::Type DirectoryIterator::type() const
{
    return _impl->type();
}


Directory::Directory()
{
}


Directory::Directory(const std::string& path)
: _path(path)
{
    if (! exists(_path) )
        throw DirectoryNotFound(path);
}


Directory::Directory(const FileInfo& fi)
: _path( fi.path() )
{
    if (!exists(_path))
        throw DirectoryNotFound(fi.path());
}


Directory::Directory(const Directory& dir)
: _path(dir._path)
{
}


Directory::~Directory()
{
}


Directory& Directory::operator=(const Directory& dir)
{
    _path = dir._path;
    return *this;
}


std::size_t Directory::size() const
{
    return 0;
}


Directory::const_iterator Directory::begin(bool skipHidden) const
{
    return DirectoryIterator( path(), skipHidden );
}


Directory::const_iterator Directory::end() const
{
    return DirectoryIterator();
}


void Directory::remove()
{
    DirectoryImpl::remove( path() );
}


void Directory::move(const std::string& to)
{
    DirectoryImpl::move(path(), to);
    _path = to;
}


std::string Directory::dirName() const
{
    // Find last slash. This separates the last path segment from the rest of the path
    std::string::size_type separatorPos = path().find_last_of( this->sep() );

    // If there is no separator, this directory is relative to the current current directory.
    // So an empty path is returned.
    if (separatorPos == std::string::npos)
    {
        return "";
    }

    // Include trailing separator to be able to distinguish between no path ("") and a path
    // which is relative to the root ("/"), for example.
    return path().substr(0, separatorPos + 1);
}


std::string Directory::name() const
{
    std::string::size_type separatorPos = path().rfind( this->sep() );

    if (separatorPos != std::string::npos)
    {
        return path().substr(separatorPos + 1);
    }
    else
    {
        return path();
    }
}


Directory Directory::create(const std::string& path, bool fullPath, mode_t mode)
{
    DirectoryImpl::create(path, fullPath, mode);
    return Directory(path);
}


bool Directory::exists(const std::string& path)
{
    return DirectoryImpl::exists(path);
}


void Directory::chdir(const std::string& path)
{
    DirectoryImpl::chdir(path);
}


std::string Directory::cwd()
{
    return DirectoryImpl::cwd();
}


std::string Directory::curdir()
{
    return DirectoryImpl::curdir();
}


std::string Directory::updir()
{
    return DirectoryImpl::updir();
}


std::string rootdir()
{
    return DirectoryImpl::rootdir();
}


std::string tmpdir()
{
    return DirectoryImpl::tmpdir();
}


char Directory::sep()
{
    return DirectoryImpl::sep();
}

} // namespace cxxtools
