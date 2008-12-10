/*
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
#ifndef CXXTOOLS_WIN32_H
#define CXXTOOLS_WIN32_H

#include "cxxtools/api.h"
#include <vector>
#include <string>
#include <cstring>
#include <windows.h>
#include <tchar.h>

namespace cxxtools {

namespace win32 {

    inline std::string toMultiByte(const wchar_t* from)
    {
        size_t length = WideCharToMultiByte(CP_ACP, 0, from, -1, NULL, 0, NULL, NULL);

        std::vector<char> str(length + 1);
        WideCharToMultiByte(CP_ACP, 0, from, -1, &str[0], length, NULL, NULL);

        std::string ret(&str[0], length);
        return ret;
    }


    inline std::string toMultiByte(const char* from)
    {
        return std::string(from);
    }


#ifdef _UNICODE
    inline std::wstring fromMultiByte(const std::string& from)
    {
        size_t length = MultiByteToWideChar(CP_ACP, 0, from.c_str(), -1, NULL, 0);

        // condsider using a vector<wchar_t> as buffer
        wchar_t* wbuf = new wchar_t[length + 1];
        MultiByteToWideChar(CP_ACP, 0, from.c_str(), -1, wbuf, length);
        std::wstring wstr(wbuf, length);
        delete[] wbuf;
        return wstr;
    }
#else
    inline std::string fromMultiByte(const std::string& from)
    {
        return from;
    }
#endif

} // namespace win32

} // namespace cxxtools

#endif
