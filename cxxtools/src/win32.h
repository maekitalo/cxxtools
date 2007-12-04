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
