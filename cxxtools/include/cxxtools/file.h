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
#ifndef CXXTOOLS_FILE_H
#define CXXTOOLS_FILE_H

#include <cxxtools/fileinfo.h>
#include <cxxtools/ioerror.h>
#include <string>

namespace cxxtools
{

/** @brief Provides common operations on files.
 */
class File
{
    public:
        /** @brief Constructs a %File object from the path \a path

            If no file exists at \a path, an exception of type FileNotFound
            is thrown.
        */
        explicit File(const std::string& path);

        /** @brief Constructs a %File object from a FileInfo object

            An exception of type %FileNotFound is thrown if the %FileInfo
            does not represent a file.
        */
        explicit File(const FileInfo& fi);

        //! @brief Copy constructor
        File(const File& file);

        //! @brief Destrctor
        ~File();

        //! @brief Assignment operator
        File& operator=(const File& file);

        /** @brief Returns the full path of file in the file-system

            This method may return a relative path, or a fully qualified one
            depending on how this object was constructed.
        */
        const std::string& path() const
        { return _path; }

        //! @brief Returns the size of the file in bytes
        std::size_t size() const;

        /** @brief Returns the parent directory path

            This method might return an empty string if the node was created
            without a complete path. If the directory is located in the root
            directory of a unix file system, a slash ("/") is returned. A
            returned directory path always ends with a trailing path separator
            character. (A backslash in Windows and a slash in Unix, for example.)
        */
        std::string dirName() const;

        //! @brief Returns the file name including an exension
        std::string name() const;

        //! @brief Returns the file name without the exension
        std::string baseName() const;

        //! @brief Returns the file name extension or an empty string if not present
        std::string extension() const;

        //! @brief Resizes the file to a new size of \a n bytes
        void resize(std::size_t n);

        /** @brief Removes the file.

            This object will be invalid after calling this method.
        */
        void remove();

        /** @brief Moves the file to the location given by \a to

            The %File object will stay valid after this method was called and
            point to the moved file.
        */
        void move(const std::string& to);

        void link(const std::string& newpath);

        void symlink(const std::string& newpath);

    public:
        //! @brief Creates a new file at the path given by \a path
        static File create(const std::string& path);

        //! @brief Returns true if a file exists at \a path, or false otherwise
        static bool exists(const std::string& path);

    protected:
        //! @brief Default Constructor
        File();

    private:
        //! @internal
        std::string _path;

        //! @internal
        class FileImpl* _impl;
};

inline bool operator<(const File& a, const File& b)
{
    return a.path() < b.path();
}

inline bool operator==(const File& a, const File& b)
{
    return a.path() == b.path();
}

inline bool operator!=(const File& a, const File& b)
{
    return !(a == b);
}

} // namespace cxxtools

#endif
