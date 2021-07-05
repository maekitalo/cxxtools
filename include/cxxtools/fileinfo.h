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
#ifndef CXXTOOLS_FILEINFO_H
#define CXXTOOLS_FILEINFO_H

#include <string>

namespace cxxtools
{

class DirectoryIterator;
class UtcDateTime;

/** @brief Provides information about a node in the file-system.
*/
class FileInfo
{
    public:
        //! @brief File-node type
        enum Type
        {
            Invalid   = 0,
            Directory = 1,
            File      = 2,
            Chardev   = 3,
            Blockdev  = 4,
            Fifo      = 5,
            Symlink   = 6,
            Socket    = 7
        };

    public:
        //! @brief Default constructor
        FileInfo();

        /** @brief Constructs a %FileInfo object from the path \a path
         *
         *  If path do not exist type is Type::Invalid.
         */
        explicit FileInfo(const std::string& path);

        explicit FileInfo(const DirectoryIterator& it);

        /** @brief Returns the type of the file node
         *
         *  The type is set on construction of the object and does not change
         */
        Type type() const                   { return _type; }

        bool exists() const                 { return _type != FileInfo::Invalid; }

        /** @brief Returns the path of node in the file-system as passed in ctor
        */
        const std::string& path() const     { return _path; }

        /** @brief Returns the name part of node in the file-system
        */
        std::string name() const;

        /** @brief Returns the parent directory path

            This method might return an empty string if the node was created
            without a complete path. If the directory is located in the root
            directory of a unix file system, a slash ("/") is returned. A
            returned directory path always ends with a trailing path separator
            character. (A backslash in Windows and a slash in Unix, for example.)
        */
        std::string dirName() const;

        //! @brief Returns the size of the file in bytes
        std::size_t size() const;

        //! @brief Returns true if the node is a directory
        bool isDirectory() const
        {
            return _type == FileInfo::Directory;
        }

        //! @brief Returns true if the node is a file
        bool isFile() const
        {
            return _type == FileInfo::File;
        }

        //! @brief Returns the modification time of the file
        UtcDateTime mtime() const;

        //! @brief Returns the create time of the file
        UtcDateTime ctime() const;

        //! @brief Returns true if the user has read access
        bool isReadingAllowed() const;

        //! @brief Returns true if the user has write access
        bool isWritingAllowed() const;

        /** @brief Removes the file node.
        */
        void remove();

        /** @brief Moves the file node to the location given by \a to

            The object will stay valid after this method was called and
            point to the moved file node.
        */
        void move(const std::string& to);

        FileInfo& operator+= (const FileInfo& path);
        FileInfo& operator+= (const std::string& path)
            { return operator+=(FileInfo(path)); }

    public:
        //! @brief Returns true if a file or directory exists at \a path
        static bool exists(const std::string& path);

        //! @brief Returns the type of file at \a path
        static Type getType(const std::string& path);

        //! @brief Returns the modification time of the file at \a path
        static UtcDateTime mtime(const std::string& path);

        //! @brief Returns the create time of the file at \a path
        static UtcDateTime ctime(const std::string& path);

        //! @brief Returns true if the user has read access to \a path
        static bool isReadingAllowed(const std::string& path);

        //! @brief Returns true if the user has write access to \a path
        static bool isWritingAllowed(const std::string& path);

    private:
        //! @internal
        Type _type;

        //! @internal
        std::string _path;
};

inline bool operator<(const FileInfo& a, const FileInfo& b)
{
    return a.path() < b.path();
}

inline bool operator==(const FileInfo& a, const FileInfo& b)
{
    return a.path() == b.path();
}

inline bool operator!=(const FileInfo& a, const FileInfo& b)
{
    return !(a == b);
}


inline FileInfo operator+ (const FileInfo& l, const FileInfo& r)
{
    FileInfo result(l);
    result += r;
    return result;
}

inline FileInfo operator+ (const FileInfo& l, const std::string& r)
{
    FileInfo result(l);
    result += r;
    return result;
}

} // namespace cxxtools

#endif
