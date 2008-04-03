/***************************************************************************
 *   Copyright (C) 2005-2006 by Dr. Marc Boris Duerner                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
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
#ifndef CXXTOOLS_MAIN_H
#define CXXTOOLS_MAIN_H

#ifdef _WIN32_WCE

#include <string.h>

int main(int, char**);

/** @brief A wmain function for wince that calls a regular main function

    Include this header in your main.cpp so it can be compiled with visual
    studio for Windows CE, which has only unicode support. This function
    simply converts the wide string arguments to 8 bit strings and calls
    a regular main function.
*/
int wmain(int argc, wchar_t* wargv[])
{
    char** argv = 0;
    argv = new char*[argc];

    // convert arguments to char*
    for (int i = 0; i < argc; ++i)
    {
        char line[256];
        wcstombs(line, wargv[i], 256);
        char* line2 = new char[ strlen(line) + 1 ];
        strcpy(line2, line);
        argv[i] = line2;
    }

    // call the "regular" main function
    int ret = main(argc, argv);

    for (int n = 0; n < argc; ++n)
    {
        delete argv[n];
    }
    delete argv;

    return ret;
}
#endif


#endif
