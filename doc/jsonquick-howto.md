JSON quick howto
================

Introduction
------------

JSON (JavaScript Object Notation) is a lightweight data-interchange format that
is easy for humans to read and write. Here is an example of JSON data:

```json
    {
        "encoding" : "UTF-8",
        
        "plug-ins" : [
            "python",
            "c++",
            "ruby"
            ],
            
        "indent" : { "length" : 3, "use_space": true }
    }
```

Code example
------------

cxxtools provides a serialization framework with JSON support. Its intermediate
data structure, `cxxtools::SerializationInfo`, can be used to represent a parsed
JSON structure and access individual elements.

The `operator >>=` is overloaded for all standard types, including standard
containers, allowing values to be extracted from a `SerializationInfo` object.

```cpp
    std::ifstream input("myconfiguration.json");
    cxxtools::SerializationInfo si;
    input >> cxxtools::Json(si);  // parse JSON into a cxxtools::SerializationInfo

    std::string encoding;
    si.getMember("encoding") >>= encoding;
    std::cout << encoding << std::endl;  // prints "UTF-8"

    unsigned length;
    bool use_space;
    si.getMember("indent").getMember("length") >>= length;
    si.getMember("indent").getMember("use_space") >>= use_space;
    // prints "length=3 use_space=true":
    std::cout << "length=" << length << " use_space=" << use_space << std::endl;
```

This is fine if you only want to access individual members. The nice thing
about operator overloading is that you can easily add your own overloads. This
is possible even for complex classes.

To read the above JSON structure, we define a corresponding C++ struct and
overload the `operator >>=` for it.

Since the operator used to parse the JSON structure also uses the `operator >>=`,
we can easily read our structure from a `std::istream`.

```cpp
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

    // read configuration from JSON file
    std::ifstream input("myconfiguration.json");
    Configuration configuration;
    input >> cxxtools::Json(configuration);  // parse JSON and put result into the configuration object

    // print out the configuration structure
    std::cout << configuration.encoding << "\n"
              << "length=" << configuration.indent.length << "\n"
              << "use_space=" << configuration.indent.use_space << std::endl;
    for (std::size_t n = 0; n < configuration.plug_ins.size(); ++n)
        std::cout << "plugin[" << n << "]=" << configuration.plug_ins[n] << "\n";
```

Note that `cxxtools::SerializationInfo::getMember(string)` throws an exception
of type `cxxtools::SerializationMemberNotFound` if the member is not found.
This makes error handling easy, since you can either catch the exception or
just let it propagate to a higher level.

Writing JSON
------------

Writing JSON is just as easy. As with reading, there are two variants.
Let's look at the first example, which again uses `cxxtools::SerializationInfo`
directly. This time we use the opposite `operator <<=`:

```cpp
    cxxtools::SerializationInfo si;
    si.addMember("encoding") <<= "UTF-8";
    cxxtools::SerializationInfo& psi = si.addMember("plug-ins");
    psi.addMember() <<= "python";
    psi.addMember() <<= "c++";
    psi.addMember() <<= "ruby";
    cxxtools::SerializationInfo& isi = si.addMember("indent");
    isi.addMember("length") <<= 3;
    isi.addMember("use_space") <<= true;

    std::ofstream output("myconfiguration.json");

    // print unformatted and compact JSON without white space:
    output << cxxtools::Json(si);

    // print formatted, human-readable JSON:
    output << cxxtools::Json(si, true);

    // same as above:
    output << cxxtools::Json(si).beautify(true);
```

The <<= operator can be overloaded for cxxtools::SerializationInfo as well:

```cpp
    // overload the operator <<= for our struct Configuration
    void operator<<= (cxxtools::SerializationInfo& si, const Configuration& c)
    {
        si.addMember("encoding") <<= c.encoding;
        si.addMember("plug-ins") <<= c.plug_ins;
        cxxtools::SerializationInfo& si_indent = si.addMember("indent");
        si_indent.addMember("length") <<= c.indent.length;
        si_indent.addMember("use_space") <<= c.indent.use_space;
    }

    ...

    Configuration configuration;
    configuration.encoding = "UTF-8";
    configuration.plug_ins.push_back("python");
    configuration.plug_ins.push_back("c++");
    configuration.plug_ins.push_back("ruby");
    configuration.indent.length = 3;
    configuration.indent.use_space = true;

    std::ofstream output("myconfiguration.json");

    // print unformatted and compact JSON without white space:
    output << cxxtools::Json(configuration);

    // print formatted, human-readable JSON:
    output << cxxtools::Json(configuration, true);

    // same as above:
    output << cxxtools::Json(configuration).beautify(true);
```

For a full example, the header `<cxxtools/json.h>` must be included.
