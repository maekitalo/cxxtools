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
#include <cxxtools/smartptr.h>
#include <cxxtools/refcounted.h>

namespace cxxtools
{

  /**
   STL-compatible container for directories.
   */
  class Dir
  {
      friend class const_iterator;

    public:
      class IDir : public RefCounted
      {
        protected:
          std::string fname;

        public:
          virtual ~IDir()  { }
          const std::string& current() const  { return fname; }
          virtual bool next() = 0;
      };

      typedef SmartPtr<IDir> IDirPtr;

      class const_iterator
        : public std::iterator<std::forward_iterator_tag, std::string>
      {
          IDirPtr impl;
          std::string current;

        public:
          const_iterator()
            { }

          explicit const_iterator(const std::string& dirname);

          const_iterator& operator++ ()
            {
              if (impl->next())
                current = impl->current();
              else
              {
                current.clear();
                impl = 0;
              }

              return *this;
            }
          const_iterator operator++ (int)
            {
              const_iterator previous = *this;
              this->operator++();
              return previous;
            }
          bool operator== (const const_iterator& it) const
            { return impl == it.impl; }
          bool operator!= (const const_iterator& it) const
            { return impl != it.impl; }
          const std::string* operator-> () const
            { return &impl->current(); }
          value_type operator* () const
            { return impl->current(); }
      };

      typedef const_iterator iterator;

    private:
      std::string dirname;

    public:
      explicit Dir(const char* dirname_)
        : dirname(dirname_)
        { }

      const_iterator begin() const
        { return const_iterator(dirname); }
      const_iterator end() const
        { return const_iterator(); }

      iterator begin()
        { return iterator(dirname); }
      iterator end()
        { return iterator(); }
  };

}

#endif // DIR_H

