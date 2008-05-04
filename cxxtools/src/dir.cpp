/* dir.cpp
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2008 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <cxxtools/dir.h>
#include <config.h>
#include <sys/types.h>

#if HAVE_DIRENT_H
# include <dirent.h>
#else
# define dirent direct
# if HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# if HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# if HAVE_NDIR_H
#  include <ndir.h>
# endif
#endif


namespace cxxtools
{
  namespace
  {
    class DirImpl : public Dir::IDir
    {
        DIR* d;

      public:
        DirImpl(const std::string& dirname)
          : d(::opendir(dirname.c_str()))
        { }
        ~DirImpl();

        bool next();
    };

    DirImpl::~DirImpl()
    {
      if (d)
        ::closedir(d);
    }

    bool DirImpl::next()
    {
      if (d)
      {
        struct dirent* dirent = ::readdir(d);
        if (dirent)
        {
          fname = dirent->d_name;
          return true;
        }
        else
        {
          ::closedir(d);
          d = 0;
          fname.clear();
        }
      }
      return false;
    }
  }

  Dir::const_iterator::const_iterator(const std::string& dirname)
    : impl(new DirImpl(dirname))
  {
    if (!impl->next())
      impl = 0;
  }

}
