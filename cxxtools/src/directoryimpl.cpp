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
#include "cxxtools/systemerror.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

namespace cxxtools {

DirectoryIteratorImpl::DirectoryIteratorImpl()
: _refs(1),
  _handle(0),
  _current(0),
  _dirty(true)
{
}


DirectoryIteratorImpl::DirectoryIteratorImpl(const char* path)
: _refs(1),
  _path(path),
  _handle(0),
  _current(0),
  _dirty(true)
{
    _handle = ::opendir( path );

    // EACCES Permission denied.
    // EMFILE Too many file descriptors in use by process.
    // ENFILE Too many files are currently open in the system.
    // ENOENT Directory does not exist, or name is an empty string.
    // ENOMEM Insufficient memory to complete the operation.
    // ENOTDIR name is not a directory.

    if( !_handle )
    {
        throw SystemError("opendir", "Could not open directory '" + std::string(path) + '\'');
    }

    // append a trailing slash if not empty, so we can add the
    // directory entry name easily
    if( ! _path.empty() && _path[_path.size()-1] != '/')
        _path += '/';

    this->advance();
}


DirectoryIteratorImpl::~DirectoryIteratorImpl()
{
    if(_handle)
        ::closedir(_handle);
}


const std::string& DirectoryIteratorImpl::name() const
{
    return _name;
}


const std::string& DirectoryIteratorImpl::path() const
{
    if(_dirty)
    {
        // replace substring after last slash with the new file-name or
        // append the file-name if we have a trailing slash. Ctor makes
        // sure we have a trailing slash.
        std::string::size_type idx = _path.rfind('/');
        if(idx != std::string::npos && ++idx < _path.size() )
        {
        	_path.replace(idx, _path.size(), _current->d_name);
        }
        else
        {
        	_path += _current->d_name;
        }
    }

    return _path;
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
    _dirty  = true;

    // _current == 0 means end
    _current = ::readdir( _handle );
    if(_current)
    	_name = _current->d_name;

    return _current != 0;
}




void DirectoryImpl::create(const std::string& path)
{
    if( -1 == ::mkdir(path.c_str(), 0777) )
    {
        throw SystemError("mkdir", "Could not create directory '" + path + "'");
    }
}

bool DirectoryImpl::exists(const std::string& path)
{
    struct stat buff;
    int err = stat(path.c_str(), &buff);

    if (err == -1 )
    {
        if (errno == ENOENT || errno == ENOTDIR)
        {
            return false;
        }

        throw SystemError("stat", "Could not stat file '" + path + "'");
    }

    return true;
}


void DirectoryImpl::remove(const std::string& path)
{
    if( -1 == ::rmdir(path.c_str()) )
    {
        throw SystemError("rmdir", "Could not remove directory '" + path + "'");
    }
}


void DirectoryImpl::move(const std::string& oldName, const std::string& newName)
{
    if (0 != ::rename(oldName.c_str(), newName.c_str()))
    {
        throw SystemError("rename", "Could not move directory '" + oldName + "' to '" + newName + "'");
    }
}


void DirectoryImpl::chdir(const std::string& path)
{
    if( -1 == ::chdir(path.c_str()) )
    {
        throw SystemError("chdir", "Could not change working directory to '" + path + "'");
    }
}


std::string DirectoryImpl::cwd()
{
    char cwd[PATH_MAX];

    if( !getcwd(cwd, PATH_MAX) )
        throw SystemError("getcwd");

    return std::string(cwd);
}


std::string DirectoryImpl::curdir()
{
    return ".";
}


std::string DirectoryImpl::updir()
{;
    return "..";
}


std::string DirectoryImpl::rootdir()
{
    return "/";
}


std::string DirectoryImpl::tmpdir()
{
    const char* tmpdir = getenv("TEMP");

    if(tmpdir)
    {
        return tmpdir;
    }

    tmpdir = getenv("TMP");
    if(tmpdir)
    {
        return tmpdir;
    }

    return DirectoryImpl::exists("/tmp") ? "/tmp" : curdir();
}


std::string DirectoryImpl::sep()
{
    return "/";
}

} // namespace cxxtools
