/* cxxtools/dir.h
   Copyright (C) 2003 Tommi Mäkitalo

This file is part of cxxtools.

Cxxtools is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Cxxtools is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with cxxtools; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#ifndef CXXTOOLS_DIR_H
#define CXXTOOLS_DIR_H

#include <string>
#include <iterator>
#include <sys/types.h>
#include <cxxtools/config.h>

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

  /**
   STL-compatible container for directories.
   */
  class dir
  {
    public:
      class const_iterator
        : public std::iterator<std::forward_iterator_tag, std::string>
      {
          std::string name;
          const dir* _dir;

        public:
          const_iterator()
            : _dir(0)
            { }

          explicit const_iterator(const dir& d)
            : _dir(&d)
            {
              if (d)
                operator++();
              else
                _dir = 0;
            }
          const_iterator& operator++ ()
            {
              struct dirent* e = _dir->entry();
              if (e == 0)
              {
                name.clear();
                _dir = 0;
              }
              else
                name = e->d_name;
              return *this;
            }
          bool operator== (const const_iterator& it) const
            { return _dir == it._dir; }
          bool operator!= (const const_iterator& it) const
            { return _dir != it._dir; }
          const std::string* operator-> () const
            { return &name; }
          value_type operator* () const
            { return name; }
      };

      typedef const_iterator iterator;

    private:
      DIR* d;

    public:
      dir(const char* name)
        { d = opendir(name); }
      ~dir()
        {
          if (d)
            closedir(d);
        }

      struct dirent* entry() const
        { return readdir(d); }

      operator bool() const
        { return d != 0; }
      const_iterator begin() const
        { return const_iterator(*this); }
      const_iterator end() const
        { return const_iterator(); }

      iterator begin()
        { return iterator(*this); }
      iterator end()
        { return iterator(); }
  };

}

#endif // DIR_H

