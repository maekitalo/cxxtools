////////////////////////////////////////////////////////////////////////
// query_params.h
//

#ifndef QUERY_PARAMS_H
#define QUERY_PARAMS_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <iostream>
#include <iterator>

class query_params
{
  public:
    typedef std::string string;
    typedef std::vector<string> names_type;
    typedef std::vector<string> unnamed_params_type;
    typedef std::multiset<string> values_set;
    typedef std::map<string, unnamed_params_type> named_params_type;
    typedef names_type::size_type size_type;

    class const_iterator
      : public std::iterator<std::random_access_iterator_tag, const std::string>
    {
        const query_params* params;
        std::string name;
        size_type pos;

        bool is_end() const
        {
          return params == 0
              || (name.empty() ? pos >= params->paramcount()
                               : pos >= params->paramcount(name));
        }

      public:
        // initialize generic end-iterator
        const_iterator()
          : params(0),
            pos(0)
          { }

        // initialize iterator for unnamed params
        const_iterator(const query_params& p)
          : params(&p),
            pos(0)
          { }

        // initialize iterator for named params
        const_iterator(const query_params& p, const std::string& n)
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
          return pos - it.pos;
        }

        reference operator[] (difference_type n) const
        {
          return *(*this + n);
        }
    };

  private:
    unnamed_params_type unnamed_params;
    named_params_type named_params;

    query_params* parent;
    bool use_parent_values;

  public:
    query_params()
      : parent(0),
        use_parent_values(true)
    { }
    query_params(query_params* _parent, bool _use_parent_values = true)
      : parent(_parent),
        use_parent_values(_use_parent_values)
    { }
    query_params(query_params& _parent, bool _use_parent_values = true)
      : parent(&_parent),
        use_parent_values(_use_parent_values)
    { }

    query_params* getParent() const
    { return parent; }

    void setParent(query_params* p, bool _use_parent_values = true)
    {
      parent = p;
      use_parent_values = _use_parent_values;
    }

    void setParent(query_params& p, bool _use_parent_values = true)
    {
      parent = &p;
      use_parent_values = _use_parent_values;
    }

    bool useParentValues() const
    { return parent != 0 && use_parent_values; }

    // returns true, if parent exists
    bool hasParent() const
    { return parent != 0; }

    // read parameters from url
    void parse_url(const std::string& url);
    // read parameters from url
    void parse_url(const char* url);
    // read parameters from stream
    void parse_url(std::istream& url_stream);

    //
    // unnamed parameter
    //

    // get unnamed parameter
    const string& param(size_type n) const
    {
      return useParentValues() && n >= unnamed_params.size()
               ? parent->param(n - unnamed_params.size())
               : unnamed_params[n];
    }

    // get number of unnamed parameters
    size_type paramcount() const
    {
      size_type ret = unnamed_params.size();
      if (useParentValues())
        ret += parent->paramcount();
      return ret;
    }

    // get unnamed parameter with operator[]
    const string& operator[] (size_type n) const   { return unnamed_params[n]; }

    // get all unnamed parameters
    template <typename output_iterator>
    void get(output_iterator& o) const
    { std::copy(unnamed_params.begin(), unnamed_params.end(), o); }

    // add unnamed parameter to parent or this class
    query_params& ret(const string& value)
    {
      if (parent)
        parent->ret(value);
      else
        unnamed_params.push_back(value);
      return *this;
    }

    // remove unnamed parameter
    void eraseUnnamed(const string& name)
    {
      unnamed_params_type::iterator i = std::find(unnamed_params.begin(),
        unnamed_params.end(), name);
      if (i != unnamed_params.end())
        unnamed_params.erase(i);

      if (parent)
        parent->erase(name);
    }

    // add unnamed parameter to this class
    query_params& add(const string& value)
    {
      unnamed_params.push_back(value);
      return *this;
    }

    //
    // named parameter
    //

    // get named parameter
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

    // shortcut for first named parameter
    const string& param(const string& name, const string& def) const
    { return param(name, 0, def); }

    // get number of named parameters
    size_type paramcount(const string& name) const
    {
      size_type ret;
      named_params_type::const_iterator i = named_params.find(name);
      ret = i == named_params.end() ? 0 : i->second.size();
      if (useParentValues())
        ret += parent->paramcount(name);
      return ret;
    }

    // get named parameter with operator[]
    string operator[] (const string& name) const
    { return param(name, std::string()); }

    // get all names
    template <typename output_iterator>
    void getNames(output_iterator o) const
    {
      for (named_params_type::const_iterator i = named_params.begin();
           i != named_params.end(); ++i)
        *o++ = i->first;
      if (useParentValues())
        parent->getNames(o);
    }

    template <typename output_iterator>
    void getValues(const string& name, output_iterator o) const
    {
      named_params_type::const_iterator i = named_params.find(name);
      if (i != named_params.end())
        std::copy(i->second.begin(), i->second.end(), o);
    }

    // checks if the named parameter exists here or in parent
    bool has(const string& name) const
    {
      return named_params.find(name) != named_params.end()
          || useParentValues() && parent->has(name);
    }

    // add named parameter to parent or this class
    query_params& ret(const string& name, const string& value)
    {
      named_params.erase(name);
      if (parent)
        parent->ret(name, value);
      else
        named_params[name].push_back(value);
      return *this;
    }

    // add named parameter to parent or this class
    query_params& addret(const string& name, const string& value)
    {
      if (parent)
        parent->addret(name, value);
      else
        named_params[name].push_back(value);
      return *this;
    }

    // remove named parameter
    void erase(const string& name)
    {
      named_params.erase(name);
      if (parent)
        parent->erase(name);
    }

    // add named parameter to this class
    query_params& add(const string& name, const string& value)
    {
      named_params[name].push_back(value);
      return *this;
    }

    // replace named parameter
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

    bool empty() const
    {
      return unnamed_params.empty()
          && named_params.empty()
          && (!useParentValues() || parent->empty());
    }

    //
    // iterator-methods
    //

    // get iterator to unnamed parameters
    const_iterator begin() const
    { return const_iterator(*this); }
    // get iterator to named parameters
    const_iterator begin(const std::string& name) const
    { return const_iterator(*this, name); }
    // get end-iterator
    const_iterator end() const
    { return const_iterator(); }

    // get parameters in url-syntax
    string getUrl() const;

    // get parameters for debugging
    string dump() const;
};

inline std::ostream& operator<< (std::ostream& out, const query_params& p)
{ return out << p.getUrl(); }

// extends query_params for use in CGI-programs by reading
// parameters from std::cin until eof.
class cgi : public query_params
{
  public:
    cgi();
};

#endif // QUERY_PARAMS_H

