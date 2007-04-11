/* cxxtools/dir.h
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2003 Tommi Maekitalo
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
  class Dir
  {
    public:
      class const_iterator
        : public std::iterator<std::forward_iterator_tag, std::string>
      {
          std::string name;
          const Dir* _dir;

        public:
          const_iterator()
            : _dir(0)
            { }

          explicit const_iterator(const Dir& d)
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
          const_iterator operator++ (int)
            {
              const_iterator current = *this;
              this->operator++();
              return current;
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
      Dir(const char* name)
        { d = opendir(name); }
      ~Dir()
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

