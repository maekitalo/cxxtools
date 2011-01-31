/*
 * Copyright (C) 2008 Tommi Maekitalo
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

#ifndef CXXTOOLS_CACHE_H
#define CXXTOOLS_CACHE_H

#include <deque>
#include <utility>

namespace cxxtools
{
  /**
     Implements a container for caching elements.

     The cache holds a list of key-value-pairs. There are 2 main operations for
     accessing the cache: put and get. Put takes a key and a value and puts the
     element into the list. Get takes a key and optional a value. If the value
     for the key is found, it is returned. The passed value otherwise. By
     default the value is constructed with the empty ctor of the value-type.

     The cache has a maximum size, after which key-value-pairs are dropped,
     when a new item is put into the cache.

     The algorithm for this cache is as follows:
       - when the cache is not full, new elements are appended
       - new elements are put into the middle of the list otherwise
       - the last element of the list is then dropped
       - when getting a value and the value is found, it is put to the
         beginning of the list

     When elements are searched, a linear search is done using the ==-operator
     of the key type.

     The caching algorithm keeps elements, which are fetched more than once in
     the first half of the list. In the second half the elements are either new
     or the elements are pushed from the first half to the second half by other
     elements, which are found in the cache.

     You should be aware, that the key type should be simple. Comparing keys
     must be cheap. Copying elements (both key and value) must be possible and
     should be cheap, since they are moved in the underlying container.

   */
  template <typename Key, typename Value>
  class Cache
  {
      typedef std::deque<std::pair<Key, Value> > DataType;
      DataType data;
      typename DataType::size_type maxElements;
      unsigned hits;
      unsigned misses;

      bool _push(const Key& key, const Value& value)
      {
        for (typename DataType::iterator it = data.begin(); it != data.end(); ++it)
        {
          if (it->first == key)
          {
            data.erase(it);
            data.push_front(typename DataType::value_type(key, value));
            return true;
          }
        }

        return false;
      }

    public:
      typedef typename DataType::size_type size_type;
      typedef typename DataType::value_type value_type;
      typedef typename DataType::iterator iterator;
      typedef typename DataType::const_iterator const_iterator;

      explicit Cache(size_type maxElements_)
        : maxElements(maxElements_),
          hits(0),
          misses(0)
        { }

      /// returns the number of elements currently in the cache
      size_type size() const        { return data.size(); }
      /// returns a iterator to the first element in the cache
      iterator begin()              { return data.begin(); }
      /// returns a iterator to the last element in the cache
      iterator end()                { return data.end(); }
      /// returns a iterator to the first element in the cache
      const_iterator begin() const  { return data.begin(); }
      /// returns a iterator to the last element in the cache
      const_iterator end() const    { return data.end(); }

      /// returns the maximum number of elements in the cache
      size_type getMaxElements() const { return maxElements; }
      void setMaxElements(size_type maxElements_)
      {
        maxElements = maxElements_;
        if (data.size() > maxElements)
          data.erase(data.begin() + maxElements, data.end());
      }

      /// removes a element from the cache and returns true, if found
      bool erase(const Key& key)
      {
        for (typename DataType::iterator it = data.begin(); it != data.end(); ++it)
        {
          if (it->first == key)
          {
            data.erase(it);
            return true;
          }
        }

        return false;
      }

      /// clears the cache.
      void clear(bool stats = false)
      {
        data.clear();
        if (stats)
          hits = misses = 0;
      }

      /// puts a new element in the cache. If the element is already found in
      /// the cache, it is considered a cache hit and pushed to the top of the
      /// list.
      void put(const Key& key, const Value& value)
      {
        if (_push(key, value))
          return;

        // A element is normally searched first in the cache using the get
        // method and if not found, read from the source and put into the
        // cache. Therefore it is considered a cache miss, when a new object
        // is put into the cache.
        ++misses;

        if (data.size() < maxElements / 2)
          data.push_back(typename DataType::value_type(key, value));
        else
          data.insert(data.begin() + maxElements / 2, typename DataType::value_type(key, value));

        if (data.size() > maxElements)
          data.pop_back();
      }

      /// puts a new element on the top of the cache. If the element is already
      /// found in the cache, it is considered a cache hit and pushed to the
      /// top of the list. This method actually overrides the need, that a element
      /// needs a hit to get to the top of the cache.
      void put_top(const Key& key, const Value& value)
      {
        if (_push(key, value))
          return;

        ++misses;

        data.push_front(typename DataType::value_type(key, value));

        if (data.size() > maxElements)
          data.pop_back();
      }

      /// returns a pair of values - a flag, if the value was found and the
      /// value if found or the passed default otherwise. If the value is
      /// found it is a cahce hit and pushed to the top of the list.
      std::pair<bool, Value> getx(const Key& key, Value def = Value())
      {
        for (typename DataType::iterator it = data.begin(); it != data.end(); ++it)
        {
          if (it->first == key)
          {
            typename DataType::value_type v = *it;
            data.erase(it);
            data.push_front(v);
            ++hits;
            return std::pair<bool, Value>(true, v.second);
          }
        }

        return std::pair<bool, Value>(false, def);
      }

      /// returns the value to a key or the passed default value if not found.
      /// If the value is found it is a cahce hit and pushed to the top of the
      /// list.
      Value get(const Key& key, Value def = Value())
      {
        return getx(key, def).second;
      }

      /// returns the number of hits.
      unsigned getHits() const    { return hits; }
      /// returns the number of misses.
      unsigned getMisses() const  { return misses; }
      /// returns the cache hit ratio between 0 and 1.
      double hitRatio() const     { return hits+misses > 0 ? static_cast<double>(hits)/static_cast<double>(hits+misses) : 0; }
      /// returns the ratio, between held elements and maximum elements.
      double fillfactor() const   { return static_cast<double>(data.size()) / static_cast<double>(maxElements); }
  };

}

#endif // CXXTOOLS_CACHE_H
