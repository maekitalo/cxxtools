/* cxxtools/query_params.h
 *
 * cxxtools - general purpose C++-toolbox
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
 *
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

/**
 QueryParams represents parameters from a HTML-Form.

 There are 2 types of Query-Parameters from a HTML-Form: named
 and unnamed. And names are not unique. This results in a
 combination of a multiset and a set. this class uses a vector
 instead of a set, because order does matter in unnamed parameters.

 QueryParams can reference a parent-object. Parent-objects might
 return values or just store them. This is for passing QueryParams as
 parameter to functions. You can pass the object itself or a new
 object, which has additional parameters, without changing the original
 object. For the called function it looks like the object contains
 all parameter. The function can still modify the parent-object by
 using the method QueryParams::ret.

 The class has a parser to extract parameters from a string or from a
 input-stream.
 */
class QueryParams
{
  public:
    typedef std::string string;
    typedef std::vector<string> names_type;
    typedef std::vector<string> unnamed_params_type;
    typedef std::multiset<string> values_set;
    typedef std::map<string, unnamed_params_type> named_params_type;
    typedef names_type::size_type size_type;

    /**
     Iterator for named and unnamed parameters in QueryParams.
     */
    class const_iterator
      : public std::iterator<std::random_access_iterator_tag, const std::string>
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

        const_iterator& operator+= (difference_type n)
        {
          pos += n;
          return *this;
        }

        const_iterator operator+ (difference_type n) const
        {
          const_iterator it(*this);
          it += n;
          return it;
        }

        const_iterator& operator-= (difference_type n)
        {
          pos -= n;
          return *this;
        }

        const_iterator operator- (difference_type n) const
        {
          const_iterator it(*this);
          it -= n;
          return it;
        }

        difference_type operator- (const const_iterator& it) const
        {
          return is_end() ? it.is_end() ? 0 : it.size() - it.pos
                          : it.is_end() ? pos - size() : pos - it.pos;
        }

        reference operator[] (difference_type n) const
        {
          return *(*this + n);
        }
    };

  private:
    unnamed_params_type unnamed_params;
    named_params_type named_params;

    QueryParams* parent;
    bool use_parent_values;

  public:
    /// default constructor
    QueryParams()
      : parent(0),
        use_parent_values(true)
    { }
    /// initializes class with parent
    QueryParams(QueryParams* _parent, bool _use_parent_values = true)
      : parent(_parent),
        use_parent_values(_use_parent_values)
    { }
    /// initializes class with parent
    QueryParams(QueryParams& _parent, bool _use_parent_values)
      : parent(&_parent),
        use_parent_values(_use_parent_values)
    { }
    QueryParams(const QueryParams& src)
      : unnamed_params(src.unnamed_params),
        named_params(src.named_params),
        parent(src.parent),
        use_parent_values(use_parent_values)
    { }

    /// returns the parent-object
    QueryParams* getParent() const
    { return parent; }

    /// changes parent-object
    void setParent(QueryParams* p, bool _use_parent_values = true)
    {
      parent = p;
      use_parent_values = _use_parent_values;
    }

    /// changes parent-object
    void setParent(QueryParams& p, bool _use_parent_values = true)
    {
      parent = &p;
      use_parent_values = _use_parent_values;
    }

    /// returns true, when parent-object delivers values
    bool useParentValues() const
    { return parent != 0 && use_parent_values; }

    /// returns true, if parent exists
    bool hasParent() const
    { return parent != 0; }

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
    const string& param(size_type n) const
    {
      return useParentValues() && n >= unnamed_params.size()
               ? parent->param(n - unnamed_params.size())
               : unnamed_params[n];
    }

    /// get number of unnamed parameters
    size_type paramcount() const
    {
      size_type ret = unnamed_params.size();
      if (useParentValues())
        ret += parent->paramcount();
      return ret;
    }

    /// get unnamed parameter with operator[] (no range-check!)
    const string& operator[] (size_type n) const
    { return unnamed_params[n]; }

    /// get all unnamed parameters
    template <typename output_iterator>
    void get(output_iterator& o) const
    { std::copy(unnamed_params.begin(), unnamed_params.end(), o); }

    /// add unnamed parameter to parent or this class if no parent
    /// exists.
    QueryParams& ret(const string& value)
    {
      if (parent)
        parent->ret(value);
      else
        unnamed_params.push_back(value);
      return *this;
    }

    /// remove unnamed parameter by value
    void eraseUnnamed(const string& value)
    {
      unnamed_params_type::iterator i = std::find(unnamed_params.begin(),
        unnamed_params.end(), value);
      if (i != unnamed_params.end())
        unnamed_params.erase(i);

      if (parent && use_parent_values)
        parent->erase(value);
    }

    /// add unnamed parameter to this class.
    QueryParams& add(const string& value)
    {
      unnamed_params.push_back(value);
      return *this;
    }

    //
    // named parameter
    //

    /// get named parameter.
    const string& param(const string& name, size_type n = 0,
      const string& def = std::string()) const
    {
      named_params_type::const_iterator i = named_params.find(name);
      if (i != named_params.end() && n < i->second.size())
        return i->second[n];
      else if (!useParentValues())
        return def;
      else
      {
        if (i != named_params.end())
          n -= i->second.size();
        return parent->param(name, n, def);
      }
    }

    /// shortcut for first named parameter
    const string& param(const string& name, const string& def) const
    { return param(name, 0, def); }

    /// get number of parameters with the given name
    size_type paramcount(const string& name) const
    {
      size_type ret;
      named_params_type::const_iterator i = named_params.find(name);
      ret = i == named_params.end() ? 0 : i->second.size();
      if (useParentValues())
        ret += parent->paramcount(name);
      return ret;
    }

    /// get first named parameter with operator[]
    string operator[] (const string& name) const
    { return param(name, std::string()); }

    /// get all names
    template <typename output_iterator>
    void getNames(output_iterator o) const
    {
      for (named_params_type::const_iterator i = named_params.begin();
           i != named_params.end(); ++i)
        *o++ = i->first;
      if (useParentValues())
        parent->getNames(o);
    }

    /// get all values with a given name
    template <typename output_iterator>
    void getValues(const string& name, output_iterator o) const
    {
      named_params_type::const_iterator i = named_params.find(name);
      if (i != named_params.end())
        std::copy(i->second.begin(), i->second.end(), o);
    }

    /// checks if the named parameter exists
    bool has(const string& name) const
    {
      return named_params.find(name) != named_params.end()
          || useParentValues() && parent->has(name);
    }

    /// replace named parameter in parent or this class if no parent exists
    QueryParams& ret(const string& name, const string& value)
    {
      named_params.erase(name);
      if (parent)
        parent->ret(name, value);
      else
        named_params[name].push_back(value);
      return *this;
    }

    /// add named parameter to parent or this class if no parent exists
    QueryParams& addret(const string& name, const string& value)
    {
      if (parent)
        parent->addret(name, value);
      else
        named_params[name].push_back(value);
      return *this;
    }

    /// remove named parameter
    void erase(const string& name)
    {
      named_params.erase(name);
      if (parent && useParentValues())
        parent->erase(name);
    }

    /// add named parameter to this class
    QueryParams& add(const string& name, const string& value)
    {
      named_params[name].push_back(value);
      return *this;
    }

    /// replace named parameter
    void replace(const string& name, const string& value, bool to_parent = true)
    {
      if (to_parent && parent)
        parent->replace(name, value);
      else
      {
        named_params[name].clear();
        named_params[name].push_back(value);
      }
    }

    /// removes all data
    void clear()
    {
      unnamed_params.clear();
      named_params.clear();
      if (parent && use_parent_values)
        parent->clear();
    }

    /// returns true, when no parameters exist (named and unnamed)
    bool empty() const
    {
      return unnamed_params.empty()
          && named_params.empty()
          && (!useParentValues() || parent->empty());
    }

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

    /// get parameters in url-syntax
    string getUrl() const;

    /// get readable parameters for debugging
    string dump() const;
};

/// output QueryParams in url-syntax
inline std::ostream& operator<< (std::ostream& out, const QueryParams& p)
{ return out << p.getUrl(); }

}

#endif // CXXTOOLS_QUERY_PARAMS_H

