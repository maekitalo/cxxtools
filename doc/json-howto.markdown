JSON serialization Howto
========================

Introduction
------------

JSON (JavaScript Object Notation) is a lightweight data-interchange format. It is
easy for humans to read and write.

It is a text format, which is also easy to parse and generate. There are plenty
of libraries in different programming languages, which support JSON so that it
is perfect for interchanging data.

Cxxtools has a serialization system which supports JSON. With little effort it
is easy to convert C++ data structures to JSON and back. In the serialization
system a intermediate data format independent data structure
`cxxtools::SerializationInfo` is used.

JSON structure
--------------

JSON data is built from 3 entity types:
 * scalars like numbers and strings
 * collections of name value pairs
 * ordered list of values

The values of collections or lists may be also be complex values like other
collections or lists.

Cxxtools serialization
----------------------

Since C++ do not support reflection, so that a library could just ask the object
for its members, the developer has to help `cxxtools` a little. This is done by
defining special operators, which convert a user defined object into a
`cxxtools::SerializationInfo` and back.

Lets look at an example. We have defined a struct with the name `Configuration`
and want to convert it into JSON and read it back. The struct looks like this:

    struct Configuration
    {
      std::string encoding;
      std::vector<std::string> plugIns;
      struct
      {
        int length;
        bool useSpace;
      } indent;
    };

We now define a serialization operator like that:

    void operator<<= (cxxtools::SerializationInfo& si, const Configuration& config)
    {
      si.addMember("encoding") <<= config.encoding;
      si.addMember("plug-ins") <<= config.plugIns;
      cxxtools::SerializationInfo& sii = si.addMember("indent");
      sii.addMember("length") <<= config.indent.length;
      sii.addMember("use_space") <<= config.indent.useSpace;
    }

And a deserialization operator:

    void operator>>= (const cxxtools::SerializationInfo& si, Configuration& config)
    {
      si.getMember("encoding") >>= config.encoding;
      si.getMember("plug-ins") >>= config.plugIns;
      const cxxtools::SerializationInfo& sii = si.getMember("indent");
      sii.getMember("length") >>= config.indent.length;
      sii.getMember("use_space") >>= config.indent.useSpace;
    }

Now we are ready to convert a C++ structure of that type into json. Lets try it
out using this little program:

    int main(int argc, char* argv[])
    {
      try
      {
        // create a configuration object:
        Configuration config;
        config.encoding = "UTF-8";
        config.plugIns.push_back("python");
        config.plugIns.push_back("c++");
        config.plugIns.push_back("ruby");
        config.indent.length = 3;
        config.indent.useSpace = true;

        // serialize to json
        cxxtools::JsonSerializer serializer(std::cout);
        serializer.beautify(true);   // this makes it just nice to read
        serializer.serialize(config).finish();
      }
      catch (const std::exception& e)
      {
        std::cerr << e.what() << std::endl;
      }
    }

We need of course a bunch of includes for that:

    #include <iostream>
    #include <cxxtools/jsonserializer.h>
    #include <cxxtools/serializationinfo.h>

When we copy everything together and compile, link into jsonwrite and run the
program, we get that output:

    {
            "encoding": "UTF-8",
            "plug-ins": [
                    "python",
                    "c++",
                    "ruby"
            ],
            "indent": {
                    "length": 3,
                    "use_space": true
            }
    }

Which is nicely formatted JSON.

I used this command to compile it:

    g++ -o jsonwrite -lcxxtools jsonwrite.cpp

To read the JSON structure back into C++ we use this little program:

    #include <cxxtools/jsondeserializer.h>
    #include <cxxtools/serializationinfo.h>
    #include "configuration.h"

    int main(int argc, char* argv[])
    {
      try
      {
        // define a empty config object
        Configuration config;

        // read json configuration struct from stdin:
        cxxtools::JsonDeserializer deserializer(std::cin);
        deserializer.deserialize(config);

        // print configuration
        std::cout << "encoding=" << config.encoding << '\n'
                  << "plugIns=";
        for (unsigned n = 0; n < config.plugIns.size(); ++n)
          std::cout << config.plugIns[n] << ' ';
        std::cout << '\n'
                  << "indent.length=" << config.indent.length << '\n'
                  << "indent.useSpace=" << config.indent.useSpace << '\n';
      }
      catch (const std::exception& e)
      {
        std::cerr << e.what() << std::endl;
      }
    }

I already added the needed includes here. I assume, the structure is defined in
the header configuration.h.

The program reads a JSON structure from standard input and prints the content to
standard output. We compile and link that into `jsonread`:

    g++ -o jsonread -lcxxtools jsonread.cpp

When running both processes like that:

    ./jsonwrite | ./jsonread

We can see the configuration structure.

Cxxtools extensions to JSON
---------------------------

JSON is borrowed from JavaScript but has compared to that some limitations,
which make writing JSON objects somewhat more cumbersome. Hence cxxtools accept
some extensions, which are quite common in other JSON implementations as well.
Note that cxxtools always writes valid JSON but accept JSON with those
extension.

The extensions are:

 * Single line C++ style comments are accepted. Text, which starts with two
   slashes (`//`) is ignored until the end of line.
 * Multi line C style comments are accepted as well. Sections starting with `/*`
   are ignored until `*/`
 * Keys in structures which start with a character and contain only alphanumeric
   characters and underscores do not need to be enclosed in quotes.
