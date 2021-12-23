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
#ifndef CXXTOOLS_DIRECTORY_H
#define CXXTOOLS_DIRECTORY_H

#include <cxxtools/fileinfo.h>
#include <cxxtools/ioerror.h>
#include <string>

namespace cxxtools {

/** @brief Iterates over entries of a directory.

    A %DirectoryIterator is created by the Directory class and
    can be used as follows:

    \code
    cxxtools::Directory d("/usr");
    for (cxxtools::DirectoryIterator it = d.begin(); it != d.end(); ++it)
    {
        std::cout << "name: " << *it << std::endl;
    }
    \endcode
*/
class DirectoryIterator
    : public std::iterator<std::forward_iterator_tag, std::string>
{
    public:
        //! @brief Default constructor
        DirectoryIterator();

        //! @brief Constructs an iterator pointing at the file given by a path
        DirectoryIterator(const std::string& path, bool skipHiden = false);

        //! @brief Copy constructor
        DirectoryIterator(const DirectoryIterator& it);

        //! @brief Destructor
        ~DirectoryIterator();

        //! @brief Advances the iterator to the next file
        DirectoryIterator& operator++();

        //! @brief Assignment operator
        DirectoryIterator& operator=(const DirectoryIterator& it);

        //! @brief Equality comparison
        bool operator==(const DirectoryIterator& it) const
        { return _impl == it._impl; }

        //! @brief Inequality comparison
        bool operator!=(const DirectoryIterator& it) const
        { return _impl != it._impl; }

        //! @brief Returns the full path of the file the iterator points at
        const std::string& path() const;

        //! @brief Returns the name of the file the iterator points at
        const std::string& operator*() const;
        const std::string& name() const { return operator*(); }

        const std::string* operator->() const;

        FileInfo::Type type() const;

        bool isDirectory() const
        { return type() == FileInfo::Directory; }

        bool isFile() const
        { return type() == FileInfo::File; }

    private:
        //! @internal
        class DirectoryIteratorImpl* _impl;
};

/** @brief Represents a single directory in the file-system.

    This class contains methods to create, move, delete directories and 
    gives to possibility to iterate over the contents of the directory.

    Iterator Example:
    \code
    cxxtools::Directory d("/usr");
    for (cxxtools::DirectoryIterator it = d.begin(); it != d.end(); ++it)
    {
        std::cout << "name: " << *it << std::endl;
    }
    \endcode
*/
class Directory
{
    public:
        typedef DirectoryIterator const_iterator;

    public:
        //! @brief Default Constructor
        Directory();

        /** @brief Constructs a %Directory object from the path \a path

            If no directory exists at \a path, an exception of type DirectoryNotFound
            is thrown.
        */
        explicit Directory(const std::string& path);

        /** @brief Constructs a %Directory object from a FileInfo object

            An exception of type %DirectoryNotFound is thrown if the %FileInfo
            does not represent a directory.
        */
        explicit Directory(const FileInfo& fi);

        //! @brief Copy constructor
        Directory(const Directory& dir);

        //! @brief Destructor
        ~Directory();

        //! @brief Assignment operator
        Directory& operator=(const Directory& dir);

        /** @brief Returns the path of the directory

            This method may return a relative path, or a fully qualified one
            depending on how this object was constructed.
        */
        const std::string& path() const
        { return _path; }

        //! @brief Returns the size of the directory in bytes
        std::size_t size() const;

        /** @brief Returns the parent directory path

            This method might return an empty string if the node was created
            without a complete path. If the directory is located in the root
            directory of a unix file system, a slash ("/") is returned. A
            returned directory path always ends with a trailing path separator
            character. (A backslash in Windows and a slash in Unix, for example.)
        */
        std::string dirName() const;

        //! @brief Returns the name of the directory excluding the path.
        std::string name() const;

        /** @brief Removes the directory

            This object will be invalid after calling this method.
        */
        void remove();

        /** @brief Moves the directory to the location given by \a to

            The %Directory object will stay valid after this method was called and
            point to the moved directory.
        */
        void move(const std::string& to);

        //! @brief Returns true, if the directory is the root directory
        bool isRoot() const;

        //! @brief Returns the upper directory.
        //! Throws DirectoryNotFound on already the root directory.
        Directory updir() const;

        //! @brief Returns an iterator to the first entry in the directory.
        const_iterator begin(bool skipHiden = false) const;

        //! @brief Returns an iterator to the end of the directory entries.
        const_iterator end() const;

        //! @brief Creates a new directory at the path given by \a path
        static Directory create(const std::string& path, bool fullPath = false, mode_t mode = 0755);

        //! @brief Returns true if a directory exists at \a path, or false otherwise
        static bool exists(const std::string& path);

        //! @brief Changes the current directory
        static void chdir(const std::string& path);

        //! @brief Returns the current directory
        static Directory cwd();

        /** @brief Returns the system root path

            Returns "/" (root) on Linux, "c:\" on Windows
        */
        static Directory rootdir();

        /** @brief Returns the systems tmp directory.

            The environment variables TEMP and TMP are checked first. If not set,
            "/tmp" is returned if it exists. If none of the environment variables
            are set and the default system tmp directory does not exist, the 
            current directory is returned.
        */
        static std::string tmpdir();

        //! @brief Returns the string representng the separator in path names
        static char sep();

    private:
        //! @internal
        std::string _path;
};

inline bool operator<(const Directory& a, const Directory& b)
{
    return a.path() < b.path();
}

inline bool operator==(const Directory& a, const Directory& b)
{
    return a.path() == b.path();
}

inline bool operator!=(const Directory& a, const Directory& b)
{
    return !(a == b);
}

}

#endif
