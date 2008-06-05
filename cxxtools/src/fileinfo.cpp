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
#include "FileInfoImpl.h"
#include "FileImpl.h"
#include "DirectoryImpl.h"
#include "cxxtools/fileinfo.h"

namespace cxxtools {

FileInfo::FileInfo()
: _type(FileInfo::Invalid)
, _reserved(0)
{}


FileInfo::FileInfo(const std::string& path)
: _path(path)
, _reserved(0)
{
    _type = FileInfoImpl::getType( path.c_str() );
}


FileInfo::FileInfo(const FileInfo& fi)
: _type(fi._type)
, _path(fi._path)
, _reserved(0)
{
}


FileInfo::~FileInfo()
{
}


FileInfo& FileInfo::operator=(const FileInfo& fi)
{
	_type = fi._type;
	_path = fi._path;
	return *this;
}


FileInfo::Type FileInfo::type() const
{
	return _type;
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


const std::string& FileInfo::path() const
{
    return _path;
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
    if(_type == FileInfo::File)
    {
        return FileImpl::size( _path.c_str() );
    }

    return 0;
}


bool FileInfo::isDirectory() const
{
    return _type == FileInfo::Directory;
}


bool FileInfo::isFile() const
{
    return _type == FileInfo::File;
}


void FileInfo::remove()
{
    if(_type == FileInfo::File)
    {
        return FileImpl::remove( _path.c_str() );
    }

    return DirectoryImpl::remove( _path.c_str() );
}


void FileInfo::move(const std::string& to)
{
    if(_type == FileInfo::File)
    {
        return FileImpl::move( _path.c_str(), to.c_str() );
    }

    return DirectoryImpl::move( _path.c_str(), to.c_str() );
}


bool FileInfo::exists(const std::string& path)
{
    return FileInfo::getType( path ) != FileInfo::Invalid;
}


FileInfo::Type FileInfo::getType(const std::string& path)
{
    return FileInfoImpl::getType( path );
}

} // namespace cxxtools
