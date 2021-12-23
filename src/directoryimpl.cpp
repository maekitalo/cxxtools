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
#include "cxxtools/ioerror.h"
#include "cxxtools/log.h"
#include "error.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

log_define("cxxtools.directory.impl")

namespace cxxtools {

DirectoryIteratorImpl::DirectoryIteratorImpl()
: _refs(1),
  _handle(0),
  _current(0),
  _dirty(true),
  _skipHidden(true)
{
}


DirectoryIteratorImpl::DirectoryIteratorImpl(const char* path, bool skipHidden)
: _refs(1),
  _path(path),
  _handle(0),
  _current(0),
  _dirty(true),
  _skipHidden(skipHidden)
{
    log_debug("opendir(" << path << ')');
    _handle = ::opendir( path );

    // EACCES Permission denied.
    // EMFILE Too many file descriptors in use by process.
    // ENFILE Too many files are currently open in the system.
    // ENOENT Directory does not exist, or name is an empty string.
    // ENOMEM Insufficient memory to complete the operation.
    // ENOTDIR name is not a directory.

    if( !_handle )
        throwCurrentErrno("opendir", path);

    log_debug("directory \"" << path << "\" => " << static_cast<void*>(_handle));

    // append a trailing slash if not empty, so we can add the
    // directory entry name easily
    if( ! _path.empty() && _path[_path.size()-1] != '/')
        _path += '/';
}


DirectoryIteratorImpl::~DirectoryIteratorImpl()
{
    if(_handle)
    {
        log_debug("closedir(" << static_cast<void*>(_handle) << ')');
        ::closedir(_handle);
    }
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


FileInfo::Type DirectoryIteratorImpl::type() const
{
    switch (_current->d_type)
    {
        case DT_BLK:
            return FileInfo::Blockdev;

        case DT_CHR:
            return FileInfo::Chardev;

        case DT_DIR:
            return FileInfo::Directory;

        case DT_FIFO:
            return FileInfo::Fifo;

        case DT_LNK:
            return FileInfo::Symlink;

        case DT_REG:
            return FileInfo::File;

        case DT_SOCK:
            return FileInfo::Socket;

        default:
            return FileInfo::getType(_path);
    }
}


unsigned int DirectoryIteratorImpl::ref()
{
    return ++_refs;
}


unsigned int DirectoryIteratorImpl::deref()
{
    return --_refs;
}


bool DirectoryIteratorImpl::advance()
{
    _dirty  = true;

    // _current == 0 means end
    do
    {
        log_debug("readdir(" << static_cast<void*>(_handle) << ')');
        _current = ::readdir( _handle );
        if(_current)
        {
            _name = _current->d_name;
            log_debug("name=\"" << _name << '"');
        }
    } while (_skipHidden && _current && _current->d_name[0] == '.');

    log_debug("advance returns " << (_current != 0));
    return _current != 0;
}




void DirectoryImpl::create(const std::string& path, bool fullPath, mode_t mode)
{
    if (fullPath)
    {
        if (exists(path))
            return;

        if (::mkdir(path.c_str(), mode) == 0)
            return;

        if (errno != ENOENT)
            throwCurrentErrno("mkdir", path);

        std::string::size_type p = path.find_last_of(sep());
        if (p == std::string::npos)
            throw SystemError("mkdir", "Could not create directory '" + path + "'");

        create(path.substr(0, p), true, mode);
        create(path, false, mode);
    }
    else if( -1 == ::mkdir(path.c_str(), mode) )
        throwCurrentErrno("mkdir", path);
}

bool DirectoryImpl::exists(const std::string& path)
{
    struct stat buff;
    int err = stat(path.c_str(), &buff);

    if (err == -1 )
    {
        if (errno == ENOENT || errno == ENOTDIR || !S_ISDIR(buff.st_mode))
        {
            return false;
        }

        throwCurrentErrno("stat", path);
    }

    return true;
}


void DirectoryImpl::remove(const std::string& path)
{
    if( -1 == ::rmdir(path.c_str()) )
    {
        throwCurrentErrno("rmdir", path);
    }
}


void DirectoryImpl::move(const std::string& oldName, const std::string& newName)
{
    if (0 != ::rename(oldName.c_str(), newName.c_str()))
    {
        throwCurrentErrno("rename", oldName + ", " + newName);
    }
}


void DirectoryImpl::chdir(const std::string& path)
{
    if( -1 == ::chdir(path.c_str()) )
    {
        throwCurrentErrno("chdir", path);
    }
}


std::string DirectoryImpl::cwd()
{
    std::vector<char> cwd(1024);

    while ( getcwd(&cwd[0], cwd.size()) == 0 )
    {
        if (errno != ERANGE)
            throw SystemError("getcwd");
        cwd.resize(cwd.size() * 2);
    }

    return std::string(&cwd[0]);
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

    return DirectoryImpl::exists("/tmp") ? "/tmp" : ".";
}


char DirectoryImpl::sep()
{
    return '/';
}

} // namespace cxxtools
