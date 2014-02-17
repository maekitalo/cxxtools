Reading and writing CSV with cxxtools serialization
===================================================

CSV is a simple format, which can be used to store and read tabular data. The
cxxtools serialization framework supports reading and writing CSV files. The CSV
serializer makes it easy and robust to create CSV files from serializable
objects and the CSV deserializer reads deserializable objects from a CSV file.

Although CSV is quite simple it has its traps and pitfalls like Data, which
contain the used delimiter or line feeds. When CSV files are written straight
forward, frequently those details will be wrong. Cxxtools helps here.

Serialization in cxxtools supports quite complex hierarchical data structures.
CSV does not. CSV has just tabular data. Hence not every object, which is
serializable can be written into a CSV. And vice versa not every object can be
retrieved from a CSV file.

Actually there are just 2 kinds of objects, which can are really supported by
CSV: arrays of arrays of scalars or arrays of objects with attributes.

Writing CSV
-----------

Lets start with a little example. We have a std::vector of std::vector of
std::string. Note that a std::vector is serialized as a array, so the structure
perfectly fulfils the requirements for the CSV serializer.

    #include <iostream>
    #include <cxxtools/csvserializer.h>
    #include <cxxtools/csv.h> // <= this header is avaialable in cxxtools >= 2.3
    #include <vector>

    void writeCsv()
    {
        // create a data structure to write
        std::vector<std::vector<std::string> > data;
        data.resize(2);
        data[0].push_back("Hello");
        data[0].push_back("World");
        data[1].push_back("Hi");
        data[1].push_back("There");

        // do the serialization:
        cxxtools::CsvSerializer serializer(std::cout);  // we write the data to std::cout
        serializer.serialize(data);

        // or simpler with cxxtools >= 2.3:
        std::cout << cxxtools::Csv(data);
    }

This prints a CSV file with 2 columns and 2 lines.

### Delimiters

The fields are delimited by comma, which is the default delimiter. We can change
the delimiter to somethings else by using the member function
`cxxtools::CsvSerializer::delimiter(Char)`:

        ...
        // do the serialization:
        cxxtools::CsvSerializer serializer(std::cout);  // we write the data to std::cout
        serializer.delimiter('\t');  // change delmiter to tab
        serializer.serialize(data);

Using the helper in cxxtools >= 2.3:

        std::cout << cxxtools::Csv(data).delimiter('\t');

Every method in the `cxxtools::CsvSerializer` is available in the helper
function also. We omit the examples using this helper from now on.

Note that it is no problem when the data contains the delimiter itself. The CSV
serializer handles it correctly by enclosing the data into quotes (which can be
selected also).

### Setting the line ending

By default a line feed is used as a line ending. Windows used a carriage return
and line feed as a line ending and as long as there are Windows users left, it
may be needed to produce CSV files with different line ending. The line ending
can be set by calling:

        serializer.lineEnding(L"\r\n");

Note that the line ending is actually a unicode string and hence this capital L
prefix must be used.

### Adding titles to our CSV

The file do not have titles. The CSV serializer don't know, what to write into
the title line. So we have to help him:

        cxxtools::CsvSerializer serializer(std::cout);  // we write the data to std::cout
        serializer.selectColumn("column1");
        serializer.selectColumn("column2");
        serializer.serialize(data);

Now our CSV file gets column titles.

### Writing objects to CSV

Instead of converting everything into a `std::vector<std::vector<std::string>
>`, std::vector of objects can be written.

We need a object with a cxxtools serialization operator to handle it. For
simplicity we define a struct with a serialization operator:

    struct TestObject
    {
        int intValue;
        std::string stringValue;
    };

    void operator<<= (cxxtools::SerializationInfo& si, const TestObject& obj)
    {
        si.addMember("intValue") <<= obj.intValue;
        si.addMember("stringValue") <<= obj.stringValue;

        // the type name is not not really needed for CSV but good to set for
        // other serializers:
        si.setTypeName("TestObject");
    }

For the examples we create a `std::vector` of `TestObject` instances:

    std::vector<TestObject> data(2);
    data[0].intValue = 1;
    data[0].stringValue = "one";
    data[1].intValue = 2;
    data[1].stringValue = "two";

Now it is easy to write the data as CSV:

    cxxtools::CsvSerializer serializer(std::cout);  // we write the data to std::cout
    serializer.serialize(data);

This time the serializer titles are added automatically since the members of
the object have suitable member names.

Optionally we can serialize just selected columns. If we want to create a CSV
with just the int value of our test object we can use `selectColumn` again. By
default all members are written. When `selectColumn` is called, just the
selected ones are written.

    cxxtools::CsvSerializer serializer(std::cout);  // we write the data to std::cout
    serializer.selectColumn("intValue");
    serializer.serialize(data);

We can also select a different title for the column. But we have to specify the
member name, which should be printed.

    serializer.selectColumn("intValue", "column 1");

Reading CSV
-----------

CSV input can be read using the `cxxtools::CsvDeserializer`. In the simplest
form the class can be instantiated with a reference to a std::istream. Here is a
example, where CSV is read from std::cin into a vector of vector of strings:

    void readCsv()
    {
        std::vector<std::vector<std::string> > data;
        cxxtools::CsvDeserializer deserializer(std::cin);
        deserializer.deserialize(data);
        // now "data" contains data, read from cin
    }

The `cxxtools::CsvDeserializer` reads the first line and interprets the first
non alphanumeric character as the delimiter. Based on that the number of columns
is determined. A different number of columns in the data is interpreted as an
error of type `cxxtoos::SerializationError` is thrown.

The data is read line by line and collected in the deserializer. After end of
input the data in the deserializer is converted to the requested type. In this
case the vector of vector of string.

Instead of autodetecting the delimiter, a delimiter can be set using
`cxxtools::CsvDeserializer::delimter(Char)`. When the delimiter is set
explicitly, the title is also optional. Reading title can then be suppressed by
`cxxtools::CsvDeseriailzer::readTitle(bool)`. Lets for example read a semicolon
separated file without titles into a vector of vector of strings:

    void readCsv()
    {
        std::vector<std::vector<std::string> > data;
        cxxtools::CsvDeserializer deserializer(std::cin);
        deserializer.delimiter(';');
        deserializer.readTitle(false);
        deserializer.deserialize(data);
        // now "data" contains data, read from cin
    }

As in writing CSV a higher level abstraction can be achieved by reading array of
objects from CSV. This time we need a cxxtools deseriailzation operatorќj

    struct TestObject
    {
        int intValue;
        std::string stringValue;
    };

    void operator>>= (const cxxtools::SerializationInfo& si, TestObject& obj)
    {
        si.getMember("intValue") >>= obj.intValue;
        si.getMember("stringValue") >>= obj.stringValue;
    }

    void readCsv()
    {
        std::vector<TestObject> data;
        cxxtools::CsvDeserializer deserializer(std::cin);
        deserializer.deserialize(data);
    }

Now a list of objects is read from `std::cin` into our vector. Note that the
titles must match the member names here. Also the title can not be omitted since
they are needed to map the data to the members of the object. But the delimiter
is handled as before. In the previous example the first non alphanumeric
character is again interpreted as a delimiter.
