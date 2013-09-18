Properties quick howto
======================

Note: This HOWTO does not work with cxxtools <= 2.2.

Introduction
------------

The properties format is a very simple file format which is suitable for simple
configuration files. It is normally not more than a list of key value pairs. The
keys and values can be separated by '=', ':' or just white space. Here is a
simple example:

    encoding = UTF-8
    plug-ins.0 = python
    plug-ins.1 = c++
    plug-ins.2 = ruby
    indent.length: 3
    indent.use_space true

Code example
------------

`cxxtools` has a serialization framework which can read properties files.  It
has a intermediate data structure `cxxtools::SerializationInfo`, which can be
used to read the properties file and access specific elements.

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

`cxxtools` uses the point in keys as a separator and so the `indent` data can be
read hierarchically as well:

    si.getMember("indent").getMember("length") >>= length;
    si.getMember("indent").getMember("use_space") >>= use_space;
    // prints "length=3 use_space=true":
    std::cout << "length=" << length << " use_space=" << use_space << std::endl;

This is fine, if you want to access single members. But the nice thing about
operator overloading is, that you can easily add your own overloads. That is
possible even for complex classes.

To read the above properties file we define the structure as a C++ `struct` and
overload the operator `>>=` for that.

Since the `operator(std::istream&, cxxtools::Properties)`, which we use to parse
the properties structure uses the operator `>>=` also, we can read our structure
easily from a `std::istream`.

    // Configuration options
    struct Configuration
    {
        // Default encoding for text
        std::string encoding;

        // Plug-ins loaded at start-up
        std::vector<std::string> plug_ins;

        // Tab indent size
        struct {
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

Note that `cxxtools::SerializationInfo::getMember(string)` throws an exception
of type `cxxtools::SerializationMemberNotFound`, if the member is not found.
This makes error handling easy, since you can either catch the exception or just
let it propagate to a higher level.

For a full example, the header `<cxxtools/properties.h>` must be included.
