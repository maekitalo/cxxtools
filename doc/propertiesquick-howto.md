Properties quick how-to
=======================

Introduction
------------

The properties format is a very simple file format suitable for simple
configuration files. It typically consists of a list of key-value pairs. Keys
and values can be separated by `=`, `:`, or simply by white space.

Here is a simple example:

```properties
encoding = UTF-8
plug-ins.0 = python
plug-ins.1 = c++
plug-ins.2 = ruby
indent.length: 3
indent.use_space true
```

Code example
------------

cxxtools provides a serialization framework that can read properties files.
Its intermediate data structure, `cxxtools::SerializationInfo`, can be used to
represent the parsed properties and access individual elements.

```cpp
    std::ifstream input("myconfiguration.properties");
    cxxtools::SerializationInfo si;
    input >> cxxtools::Properties(si);  // parse properties data into a cxxtools::SerializationInfo

    std::string encoding;
    si.getMember("encoding") >>= encoding;
    std::cout << encoding << std::endl;  // prints "UTF-8"

    unsigned length;
    bool use_space;
    si.getMember("indent.length") >>= length;
    si.getMember("indent.use_space") >>= use_space;

    // prints "length=3 use_space=true":
    std::cout << "length=" << length << " use_space=" << use_space << std::endl;
```

cxxtools uses the dot in keys as a separator, so the `indent` data can also be
read hierarchically:

```cpp
    si.getMember("indent").getMember("length") >>= length;
    si.getMember("indent").getMember("use_space") >>= use_space;

    // prints "length=3 use_space=true":
    std::cout << "length=" << length << " use_space=" << use_space << std::endl;
```

This is fine if you only want to access individual members. The advantage of
operator overloading, however, is that you can easily add your own overloads.
This is possible even for complex classes.

To read the properties file above, we define a corresponding C++ struct and
overload the `>>=` operator for it.

Since the operator used to parse the properties structure also uses the `>>=`
operator, we can easily read our structure from a `std::istream`.

```cpp
    // Configuration options
    struct Configuration
    {
        // Default encoding for text
        std::string encoding;

        // Plug-ins loaded at start-up
        std::vector<std::string> plug_ins;

        // Tab indent size
        struct
        {
            unsigned length;
            bool use_space;
        } indent;
    };

    // overload the operator >>= for our struct Configuration
    void operator>>= (const cxxtools::SerializationInfo& si, Configuration& c)
    {
        si.getMember("encoding") >>= c.encoding;

        // if you want to make encoding optional with a default value use:
        //
        //   if (!si.getMember("encoding", c.encoding))
        //       c.encoding = "UTF-8";

        si.getMember("plug-ins") >>= c.plug_ins;

        const cxxtools::SerializationInfo& si_indent = si.getMember("indent");
        si_indent.getMember("length") >>= c.indent.length;
        si_indent.getMember("use_space") >>= c.indent.use_space;
    }

    ...

    // read configuration from properties file
    std::ifstream input("myconfiguration.properties");
    Configuration configuration;
    input >> cxxtools::Properties(configuration);  // parse properties and put result into the configuration object

    // print out the configuration structure
    std::cout << configuration.encoding << "\n"
              << "length=" << configuration.indent.length << "\n"
              << "use_space=" << configuration.indent.use_space << std::endl;

    for (unsigned n = 0; n < configuration.plug_ins.size(); ++n)
        std::cout << "plugin[" << n << "]=" << configuration.plug_ins[n] << "\n";
```

Note that `cxxtools::SerializationInfo::getMember(string)` throws an exception
of type `cxxtools::SerializationMemberNotFound` if the member is not found.
This makes error handling straightforward because you can either catch the
exception or let it propagate to a higher level.

For a complete example, the header `<cxxtools/properties.h>` must be included.
