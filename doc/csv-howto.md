## Reading and writing CSV with cxxtools serialization

CSV is a simple format for storing and exchanging tabular data. The cxxtools
serialization framework supports reading and writing CSV files. The CSV
serializer makes it easy and robust to create CSV files from serializable
objects, while the CSV deserializer reads deserializable objects from CSV
files.

Although CSV is a relatively simple format, it has a few traps and pitfalls,
such as data containing the delimiter itself or line feeds. When CSV files are
written manually, these details are easily handled incorrectly. cxxtools takes
care of them automatically.

Serialization in cxxtools supports complex hierarchical data structures, while
CSV only supports tabular data. Therefore, not every serializable object can be
written to CSV, and conversely, not every object can be read from a CSV file.

There are essentially two kinds of data structures that can be represented
directly as CSV:

* arrays of arrays of scalar values
* arrays of objects with scalar attributes

### Writing CSV

Let's start with a simple example. We have a `std::vector` of `std::vector` of
`std::string`. A `std::vector` is serialized as an array, so this structure
perfectly fulfills the requirements of the CSV serializer.

```cpp
#include <iostream>
#include <cxxtools/csvserializer.h>
#include <cxxtools/csv.h>
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

    // serialize the data
    cxxtools::CsvSerializer serializer(std::cout);  // write the data to std::cout
    serializer.serialize(data);

    // or, more simply (from <cxxtools/csv.h>)
    std::cout << cxxtools::Csv(data);
}
```

This produces CSV output with two columns and two rows.

#### Delimiters

Fields are separated by commas by default. The delimiter can be changed using
the member function `cxxtools::CsvSerializer::delimiter(Char)`:

```cpp
...
// serialize the data
cxxtools::CsvSerializer serializer(std::cout);  // write the data to std::cout
serializer.delimiter('\t');  // change the delimiter to a tab
serializer.serialize(data);
```

Using the helper:

```cpp
std::cout << cxxtools::Csv(data).delimiter('\t');
```

All options provided by `cxxtools::CsvSerializer` are also available through
the helper. For brevity, examples using the helper are omitted from this point
on.

It is not a problem if the data itself contains the delimiter. The CSV
serializer handles this correctly by enclosing the data in quotes. The quote
character can also be configured.

#### Setting the line ending

By default, a line feed is used as the line ending. Windows commonly uses a
carriage return followed by a line feed, so it may be necessary to produce CSV
files with a different line ending.

The line ending can be set by calling:

```cpp
serializer.lineEnding(L"\r\n");
```

Note that the line ending is a Unicode string, so the `L` prefix is required.

#### Adding titles to our CSV

Our CSV file does not have column titles yet. Since the CSV serializer does not
know which titles to use, we have to specify them explicitly:

```cpp
cxxtools::CsvSerializer serializer(std::cout);  // write the data to std::cout
serializer.selectColumn("column1");
serializer.selectColumn("column2");
serializer.serialize(data);
```

Now the CSV file contains column titles.

#### Writing objects to CSV

Instead of converting everything into a `std::vector<std::vector<std::string> >`,
we can write a `std::vector` of objects directly.

For this, we need an object with a cxxtools serialization operator. For
simplicity, we define a struct with an appropriate serialization operator:

```cpp
struct TestObject
{
    int intValue;
    std::string stringValue;
};

void operator<<= (cxxtools::SerializationInfo& si, const TestObject& obj)
{
    si.addMember("intValue") <<= obj.intValue;
    si.addMember("stringValue") <<= obj.stringValue;

    // The type name is not really needed for CSV, but it is useful for other
    // serializers:
    si.setTypeName("TestObject");
}
```

For the following examples, we create a `std::vector` containing two
`TestObject` instances:

```cpp
std::vector<TestObject> data(2);
data[0].intValue = 1;
data[0].stringValue = "one";
data[1].intValue = 2;
data[1].stringValue = "two";
```

The data can now easily be written as CSV:

```cpp
cxxtools::CsvSerializer serializer(std::cout);  // write the data to std::cout
serializer.serialize(data);
```

This time, column titles are added automatically because the members of the
objects have suitable member names.

Optionally, we can serialize only selected columns. For example, if we want to
create a CSV file containing only the integer value of our test object, we can
use `selectColumn()` again. By default, all members are written. Once
`selectColumn()` is called, only the selected members are written:

```cpp
cxxtools::CsvSerializer serializer(std::cout);  // write the data to std::cout
serializer.selectColumn("intValue");
serializer.serialize(data);
```

We can also specify a different title for a column. In that case, we have to
provide both the member name to be serialized and the desired column title:

```cpp
serializer.selectColumn("intValue", "column 1");
```

### Reading CSV

CSV input can be read using `cxxtools::CsvDeserializer`. In the simplest case,
the class can be instantiated with a reference to a `std::istream`.

The following example reads CSV data from `std::cin` into a vector of vectors
of strings:

```cpp
void readCsv()
{
    std::vector<std::vector<std::string> > data;
    cxxtools::CsvDeserializer deserializer(std::cin);
    deserializer.deserialize(data);

    // "data" now contains the data read from std::cin

    // or the short form:
    std::cin >> cxxtools::Csv(data);
}
```

Note that the delimiter is detected automatically here. The first line contains
the column titles, and the first non-alphanumeric character is used as the
delimiter.

We can also read a vector of objects. For this, we use the struct defined above
and add a deserialization operator:

```cpp
void operator>>= (const cxxtools::SerializationInfo& si, TestObject& obj)
{
    si.getMember("intValue") >>= obj.intValue;
    si.getMember("stringValue") >>= obj.stringValue;
}
```

We can then read a list of objects from a CSV file:

```cpp
void readCsv()
{
    std::vector<TestObject> data;
    cxxtools::CsvDeserializer deserializer(std::cin);
    deserializer.deserialize(data);

    // "data" now contais the data read from std::cin

    // or again the short form:
    std::cin >> cxxtools::Csv(data);
}
```
