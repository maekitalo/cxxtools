////////////////////////////////////////////////////////////////////////
// dir.h
//

#ifndef CXXTOOLS_DIR_H
#define CXXTOOLS_DIR_H

#include <string>
#include <iterator>
#include <sys/types.h>
#include <dirent.h>

namespace cxxtools
{

  //////////////////////////////////////////////////////////////////////
  /// Klasse zum lesen von Verzeichnissen.
  ///
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

