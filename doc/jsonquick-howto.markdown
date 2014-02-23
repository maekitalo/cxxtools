JSON quick howto
================

Note: This HOWTO does not work with cxxtools <= 2.2. With 2.2 it is slightly
more complicated. See the last section here, how to do it with version 2.2.

Introduction
------------

JSON (JavaScript Object Notation) is a lightweight data-interchange format. It is
easy for humans to read and write.

Here is an example of JSON data:

    // Configuration options
    {
        // Default encoding for text
        "encoding" : "UTF-8",
        
        // Plug-ins loaded at start-up
        "plug-ins" : [
            "python",
            "c++",
            "ruby"
            ],
            
        // Tab indent size
        "indent" : { "length" : 3, "use_space": true }
    }

Code example
------------

`cxxtools` has a serialization framework which supports json. It has a
intermediate data structure `cxxtools::SerializationInfo`, which can be used to
read the json structure and access specific elements.

The operator `>>=` is overloaded for all standard types including std containers
to convert a SerializationInfo into some value.

    std::ifstream input("myconfiguration.json");
    cxxtools::SerializationInfo si;
    input >> cxxtools::Json(si);  // parse json into a cxxtools::SerializationInfo

    std::string encoding;
    si.getMember("encoding") >>= encoding;
    std::cout << encoding << std::endl;  // prints "UTF-8"

    unsigned length;
    bool use_space;
    si.getMember("indent").getMember("length") >>= length;
    si.getMember("indent").getMember("use_space") >>= use_space;
    // prints "length=3 use_space=true":
    std::cout << "length=" << length << " use_space=" << use_space << std::endl;

This is fine, if you want to access single members. But the nice thing about
operator overloading is, that you can easily add your own overloads. That is
possible even for complex classes.

To read the above json structure we define the structure as a C++ `struct` and
overload the operator `>>=` for that.

Since the `operator(std::istream&, cxxtools::Json)`, which we use to parse the
json structure uses the operator `>>=` also, we can read our structure easily
from a `std::istream`.

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

    // read configuration from json file
    std::ifstream input("myconfiguration.json");
    Configuration configuration;
    input >> cxxtools::Json(configuration);  // parse json and put result into the configuration object

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

Writing JSON
------------

Writing json is also easily possible. Like reading there are 2 variants. Lets
look at the first exmple, which again uses `cxxtools::SerializationInfo`
directly. This time we use the opposite operator `<<=`:

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
    output << cxxtools::Json(si);  // this prints unformatted and very compact json without any white space
    output << cxxtools::Json(si, true);  // this adds indentatsion and line feeds to make it human readable
    output << cxxtools::Json(si).beautify(true);  // same as above

The operator `<<=` for `cxxtools::SerializationInfo` is overloadable as well:

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
    output << cxxtools::Json(configuration);  // this prints unformatted and very compact json without any white space
    output << cxxtools::Json(configuration, true);  // this prints unformatted and very compact json without any white space
    output << cxxtools::Json(configuration).beautify(true);  // same as above

For a full example, the header `<cxxtools/json.h>` must be included.

Using cxxtools 2.2
------------------

The class cxxtools::Json is new in cxxtools and currenty only available in the
git repository. But the json serializer and deserializer are available and hence
with a small change the examples work with cxxtools 2.2 as well.

For deserialization (reading json), a explicit instance of the JsonSerializer
must be instantiated:

    std::ifstream input("myconfiguration.json");
    cxxtools::SerializationInfo si;
    cxxtools::JsonDeseriailzer deserializer(input);
    deserializer.deserialize(si);  // parse json into a cxxtools::SerializationInfo

Or using our overloaded operator `>>=`:

    cxxtools::JsonDeseriailzer deserializer(input);
    deseriailzer.deserialize(configuration);  // parse json and put result into the configuration object

Writing JSON needs a cxxtools::JsonSerializer:

    std::ofstream output("myconfiguration.json");
    cxxtools::JsonSeriailzer serializer(output);
    output.serialize(si);

For beautification a attribute must be set:

    cxxtools::JsonSeriailzer serializer(output);
    serializer.beautify(true);
    output.serialize(si);

Again our overloaded operator `<<=` can be used also:

    cxxtools::JsonSeriailzer serializer(output);
    output.serialize(configuration);

For reading we need the header `<cxxtools/jsondeserializer.h>` and for writing
`<cxxtools/jsonserializer.h>`.
