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
#include "cxxtools/fileinfo.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

namespace cxxtools {

    class FileInfoImpl
    {
        public:
            static FileInfo::Type getType(const std::string& path)
            {
                struct stat st;
                if( 0 != ::lstat(path.c_str(), &st) )
                {
                    return FileInfo::Invalid;
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
    };

} // namespace cxxtools
