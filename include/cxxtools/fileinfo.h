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
        */
        explicit FileInfo(const std::string& path);

        explicit FileInfo(const DirectoryIterator& it);

        //! @brief Copy constructor
        FileInfo(const FileInfo& fi);

        //! @brief Destructor
        ~FileInfo();

        //! @brief Assignment operator
        FileInfo& operator=(const FileInfo& fi);

        //! @brief Returns the type of the file node
        Type type() const;

        const std::string& path() const;

        /** @brief Returns the full path of node in the file-system

            This method may return a relative path, or a fully qualified one
            depending on how this object was constructed.
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

        /** @brief Removes the file node.

            This object will be invalid after calling this method.
        */
        void remove();

        /** @brief Moves the file node to the location given by \a to

            The object will stay valid after this method was called and
            point to the moved file node.
        */
        void move(const std::string& to);

    public:
        //! @brief Returns true if a file or directory exists at \a path
        static bool exists(const std::string& path);

        //! @brief Returns the type of file at \a path
        static Type getType(const std::string& path);

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

} // namespace cxxtools

#endif
