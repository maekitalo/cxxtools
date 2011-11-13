/*
 * Copyright (C) 2011 Tommi Maekitalo
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

#ifndef CXXTOOLS_STDOUT_H
#define CXXTOOLS_STDOUT_H

#include <iostream>
#include <fstream>
#include <cxxtools/arg.h>

namespace cxxtools
{
    /**
     * Helper class for redirecting output to stdout or file using command line switch.
     *
     * Using this class it is easy to provide a command line switch to the user
     * to redirect output to a file.
     *
     * Examples:
     * \code
     * int main(int argc, char* argv[])
     * {
     *   cxxtools::ArgOut out(argc, argv, 'o');
     *   out << "this is printed to std::cout or to a file when a file name with the -o option is passed" << std::endl;
     * }
     * \endcode
     *
     * \code
     * int main(int argc, char* argv[])
     * {
     *   cxxtools::ArgOut out(argc, argv);
     *   out << "this is printed to std::cout or to a file when a file name is passed as a parameter" << std::endl;
     * }
     * \endcode
     *
     */
    class ArgOut : public std::ostream
    {
            std::ofstream _ofile;

            void doInit(const Arg<const char*>& arg)
            {
                std::streambuf* buf;
                if (arg.isSet())
                {
                    _ofile.open(arg);
                    buf = _ofile.rdbuf();
                }
                else
                    buf = std::cout.rdbuf();
                init(buf);
            }

        public:
            ArgOut(int& argc, char* argv[], char option)
            {
                Arg<const char*> ofile(argc, argv, option);
                doInit(ofile);
            }

            ArgOut(int& argc, char* argv[], const char* option)
            {
                Arg<const char*> ofile(argc, argv, option);
                doInit(ofile);
            }

            ArgOut(int& argc, char* argv[])
            {
                Arg<const char*> ofile(argc, argv);
                doInit(ofile);
            }

            bool redirected() const
            {
                return rdbuf() != std::cout.rdbuf();
            }
    };
}

#endif // CXXTOOLS_STDOUT_H
