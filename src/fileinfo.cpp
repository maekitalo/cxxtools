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
#include "fileimpl.h"
#include "directoryimpl.h"
#include "cxxtools/fileinfo.h"
#include "cxxtools/directory.h"
#include "cxxtools/datetime.h"
#include "cxxtools/timespan.h"
#include "cxxtools/systemerror.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <config.h>

namespace cxxtools
{

static FileInfo::Type doGetType(const std::string& path)
{
    struct stat st;
    if( 0 != ::lstat(path.c_str(), &st) )
    {
        if (errno == ENOENT || errno == ENOTDIR)
            return FileInfo::Invalid;
        else
            throw SystemError(("lstat(" + path + ')').c_str());
    }

    if( S_ISDIR(st.st_mode) )
    {
        return FileInfo::Directory;
    }
    else if( S_ISCHR(st.st_mode) )
    {
        return FileInfo::Chardev;
    }
    else if( S_ISBLK(st.st_mode) )
    {
        return FileInfo::Blockdev;
    }
    else if( S_ISFIFO(st.st_mode) )
    {
        return FileInfo::Fifo;
    }
    else if( S_ISLNK(st.st_mode) )
    {
        return FileInfo::Symlink;
    }
    else if( S_ISSOCK(st.st_mode) )
    {
        return FileInfo::Socket;
    }

    return FileInfo::File;
}

#ifdef HAVE_STATX
static struct statx doStatX(const std::string& path)
{
    struct statx st;
    int ret = statx(AT_FDCWD, path.c_str(), 0, STATX_SIZE|STATX_MTIME|STATX_BTIME, &st);
    if (ret == -1)
    {
        if (errno == ENOENT)
            throw FileNotFound(path);
        else
            throw SystemError(("statx(" + path + ')').c_str());
    }

    return st;
}
#endif

static struct stat doStat(const std::string& path)
{
    struct stat st;
    if (::stat(path.c_str(), &st) == -1)
    {
        if (errno == ENOENT)
            throw FileNotFound(path);
        else
            throw SystemError(("stat(" + path + ')').c_str());
    }

    return st;
}


FileInfo::FileInfo()
: _type(FileInfo::Invalid)
{}


FileInfo::FileInfo(const std::string& path)
: _path(path)
{
    _type = doGetType( path );
}


FileInfo::FileInfo(const DirectoryIterator& it)
: _path(it.path())
{
    _type = it.type();
}


std::string FileInfo::name() const
{
    std::string::size_type pos = _path.rfind( DirectoryImpl::sep() );

    if (pos != std::string::npos)
    {
        return _path.substr(pos + 1);
    }
    else
    {
        return _path;
    }
}


std::string FileInfo::dirName() const
{
    // Find last slash. This separates the file name from the path.
    std::string::size_type pos = _path.find_last_of( DirectoryImpl::sep() );

    // If there is no separator, the file is relative to the current
    // directory. So an empty path is returned.
    if (pos == std::string::npos)
    {
        return "";
    }

    // Include trailing separator to be able to distinguish between no
    // path ("") and a path which is relative to the root ("/"), for example.
    return _path.substr(0, pos + 1);
}


std::size_t FileInfo::size() const
{
    if(_type == FileInfo::File || _type == FileInfo::Symlink)
    {
        return FileImpl::size( _path );
    }

    return 0;
}

UtcDateTime FileInfo::mtime() const
{
    return mtime(_path);
}

UtcDateTime FileInfo::ctime() const
{
    return ctime(_path);
}

bool FileInfo::isReadingAllowed() const
{
    return isReadingAllowed(_path);
}

bool FileInfo::isWritingAllowed() const
{
    return isWritingAllowed(_path);
}

void FileInfo::remove()
{
    if (_type == FileInfo::Directory)
        DirectoryImpl::remove(_path);
    else
        FileImpl::remove(_path);
    _type = FileInfo::Invalid;
}


void FileInfo::move(const std::string& to)
{
    if(_type == FileInfo::Directory)
        return DirectoryImpl::move(_path, to);
    else
        return FileImpl::move(_path, to);
}

FileInfo& FileInfo::operator+= (const FileInfo& path)
{
    if (_path.empty())
        _path = path.path();
    else if (path.path().empty())
        ;
    else if (_path.back() == DirectoryImpl::sep())
    {
        if (path.path().front() == DirectoryImpl::sep())
            _path += path.path().substr(1);
        else
            _path += path.path();
    }
    else if (path.path().front() == DirectoryImpl::sep())
        _path += path.path();
    else
    {
        _path += DirectoryImpl::sep();
        _path += path.path();
    }

    return *this;
}

bool FileInfo::exists(const std::string& path)
{
    return FileInfo::getType( path ) != FileInfo::Invalid;
}


FileInfo::Type FileInfo::getType(const std::string& path)
{
    return doGetType( path );
}


UtcDateTime FileInfo::mtime(const std::string& path)
{
#ifdef HAVE_STATX
    auto st = doStatX(path);

    return DateTime::fromMSecsSinceEpoch(
        Seconds(st.stx_mtime.tv_sec)
            + Microseconds(st.stx_mtime.tv_nsec / 1000));
#else
    struct stat st = doStat(path);

    return DateTime::fromMSecsSinceEpoch(
        Seconds(st.st_mtim.tv_sec)
            + Microseconds(st.st_mtim.tv_nsec / 1000));
#endif
}


UtcDateTime FileInfo::ctime(const std::string& path)
{
#ifdef HAVE_STATX
    auto st = doStatX(path);

    return DateTime::fromMSecsSinceEpoch(
        Seconds(st.stx_btime.tv_sec)
            + Microseconds(st.stx_btime.tv_nsec / 1000));
#else
    struct stat st = doStat(path);

    return DateTime::fromMSecsSinceEpoch(
        Seconds(st.st_ctim.tv_sec)
            + Microseconds(st.st_ctim.tv_nsec / 1000));
#endif
}


bool FileInfo::isReadingAllowed(const std::string& path)
{
    struct stat st = doStat(path);
    return (st.st_mode & S_IRUSR) != 0;
}


bool FileInfo::isWritingAllowed(const std::string& path)
{
    struct stat st = doStat(path);
    return (st.st_mode & S_IWUSR) != 0;
}


} // namespace cxxtools
