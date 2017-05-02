/*
 * Copyright (C) 2003,2004 Tommi Maekitalo
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

#ifndef CXXTOOLS_QUERY_PARAMS_H
#define CXXTOOLS_QUERY_PARAMS_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <iostream>
#include <iterator>

namespace cxxtools
{

class SerializationInfo;

/**
 QueryParams represents parameters from a HTML-Form.

 There are 2 types of Query-Parameters from a HTML-Form: named
 and unnamed. And names are not unique. This results in a
 combination of a multiset and a set. this class uses a vector
 instead of a set, because order does matter in unnamed parameters.

 The class has a parser to extract parameters from a std::string or from a
 input-stream.
 */
class QueryParams
{
    friend void operator<<= (cxxtools::SerializationInfo& si, const QueryParams& p);

  public:
    struct value_type
    {
      std::string name;
      std::string value;

      value_type() { }
      value_type(const std::string& name_, const std::string& value_)
        : name(name_),
          value(value_)
      { }
    };
    typedef std::vector<value_type> values_type;
    typedef values_type::size_type size_type;

    /**
     Iterator for named and unnamed parameters in QueryParams.
     */
    class const_iterator
      : public std::iterator<std::bidirectional_iterator_tag, const std::string>
    {
        const QueryParams* params;
        std::string name;
        size_type pos;

        size_type size() const
        { 
          return (name.empty() ? params->paramcount()
                               : params->paramcount(name));
        }

        bool is_end() const
        {
          return params == 0 || pos >= size();
        }

        size_type getpos() const
        {
          return is_end() ? size() : pos;
        }

      public:
        /// initialize generic end-iterator
        const_iterator()
          : params(0),
            pos(0)
          { }

        /// initialize iterator for unnamed params
        explicit const_iterator(const QueryParams& p)
          : params(&p),
            pos(0)
          { }

        /// initialize iterator for named params
        const_iterator(const QueryParams& p, const std::string& n)
          : params(&p),
            name(n),
            pos(0)
          { }

        bool operator== (const const_iterator& it) const
        {
          bool e = it.is_end();
          return is_end() ? e
                          : !e && pos == it.pos;
        }

        bool operator!= (const const_iterator& it) const
        {
          bool e = it.is_end();
          return is_end() ? !e
                          : e || pos != it.pos;
        }

        const_iterator& operator++()
        {
          ++pos;
          return *this;
        }

        const_iterator operator++(int)
        {
          const_iterator it = *this;
          ++pos;
          return it;
        }

        const_iterator& operator--()
        {
          --pos;
          return *this;
        }

        const_iterator operator--(int)
        {
          const_iterator it = *this;
          --pos;
          return it;
        }

        reference operator*() const
        {
          return name.empty() ? params->param(pos)
                              : params->param(name, pos);
        }

        pointer operator->() const
        {
          return &(operator*());
        }

    };

  private:
    values_type _values;

  public:
    /// default constructor
    QueryParams()
    { }

    explicit QueryParams(const std::string& url)
    {
      parse_url(url);
    }

    explicit QueryParams(const char* url)
    {
      parse_url(url);
    }

    /// read parameters from url
    void parse_url(const std::string& url);
    /// read parameters from url
    void parse_url(const char* url);
    /// read parameters from stream
    void parse_url(std::istream& url_stream);

    //
    // unnamed parameter
    //

    /// get unnamed parameter by number (no range-check!)
    const std::string& param(size_type n) const
    { return param(std::string(), n); }

    /// get number of unnamed parameters
    size_type paramcount() const
    { return paramcount(std::string()); }

    /// get unnamed parameter with operator[] (no range-check!)
    const std::string& operator[] (size_type n) const
    { return param(n); }

    /// add unnamed parameter
    QueryParams& add(const std::string& value)
    {
      _values.push_back(value_type(std::string(), value));
      return *this;
    }

    /// remove unnamed parameter
    QueryParams& remove()
    { remove(std::string()); return *this; }

    /// set unnamed parameter
    QueryParams& set(const std::string& value)
    {
      remove();
      add(value);
      return *this;
    }


    //
    // named parameter
    //

    /// get nth named parameter.
    const std::string& param(const std::string& name, size_type n = 0) const;

    /// get nth named parameter with default value.
    std::string param(const std::string& name, size_type n,
      const std::string& def) const;

    /// get named parameter or default value.
    std::string param(const std::string& name, const std::string& def) const
    { return param(name, 0, def); }

    /// get number of parameters with the given name
    size_type paramcount(const std::string& name) const;

    /// get first named parameter with operator[]
    std::string operator[] (const std::string& name) const
    { return param(name, 0, std::string()); }

    /// checks if the named parameter exists
    bool has(const std::string& name) const;

    /// add named parameter
    QueryParams& add(const std::string& name, const std::string& value)
    {
      _values.push_back(value_type(name, value));
      return *this;
    }

    /// remove named parameter
    QueryParams& remove(const std::string& name);

    /// sets named parameter
    QueryParams& set(const std::string& name, const std::string& value)
    {
      remove(name);
      add(name, value);
      return *this;
    }

    QueryParams& add(const QueryParams& other)
    {
      _values.insert(_values.end(), other._values.begin(), other._values.end());
      return *this;
    }

    /// returns all names of parameters
    template <typename output_iterator>
    void getNames(output_iterator o) const
    {
      for (values_type::const_iterator i = _values.begin(); i != _values.end(); ++i)
        *o++ = (*i).name;
    }

    /// removes all data
    void clear()
    { _values.clear(); }

    /// returns true, when no parameters exist (named and unnamed)
    bool empty() const
    { return _values.empty(); }

    //
    // iterator-methods
    //

    /// get iterator to unnamed parameters
    const_iterator begin() const
    { return const_iterator(*this); }
    /// get iterator to named parameter
    const_iterator begin(const std::string& name) const
    { return const_iterator(*this, name); }
    /// get end-iterator (named and unnamed)
    const_iterator end() const
    { return const_iterator(); }

    /// get parameters as url
    std::string getUrl() const;

};


/// output QueryParams in url-syntax
inline std::ostream& operator<< (std::ostream& out, const QueryParams& p)
{ return out << p.getUrl(); }

/// make QueryParams serializable
void operator<<= (cxxtools::SerializationInfo& si, const QueryParams& p);

}

#endif // CXXTOOLS_QUERY_PARAMS_H

