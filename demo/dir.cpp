/*
 * Copyright (C) 2003 Tommi Maekitalo
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

#include <iostream>
#include <cxxtools/directory.h>
#include <cxxtools/fileinfo.h>
#include <cxxtools/arg.h>
#include <iterator>

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::Arg<bool> longdir(argc, argv, 'l');
    cxxtools::Arg<bool> showHidden(argc, argv, 'h');

    for (int a = 1; a < argc; ++a)
    {
      std::cout << "directory content of \"" << argv[a] << "\":\n";

      cxxtools::Directory d(argv[a]);

      for (cxxtools::Directory::const_iterator it = d.begin(!showHidden); it != d.end(); ++it)
      {
        if (longdir)
        {
          cxxtools::FileInfo fi(it);
          switch (fi.type())
          {
            case cxxtools::FileInfo::Directory: std::cout << 'D'; break;
            case cxxtools::FileInfo::File:      std::cout << '-'; break;
            case cxxtools::FileInfo::Chardev:   std::cout << 'C'; break;
            case cxxtools::FileInfo::Blockdev:  std::cout << 'B'; break;
            case cxxtools::FileInfo::Fifo:      std::cout << 'F'; break;
            case cxxtools::FileInfo::Symlink:   std::cout << 'S'; break;
            default:                            std::cout << '?'; break;
          }

          std::cout << '\t' << fi.size() << '\t' << fi.name() << '\n';
        }
        else
        {
          std::cout << *it << '\n';
        }
      }
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

