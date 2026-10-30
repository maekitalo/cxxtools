Serialization with cxxtools
===========================

Introduction
------------

Serialization is the process of converting data structures into a sequence of
bytes and reconstructing the original data structures from that sequence.

Serialization is useful in many situations, for example:

 * storing data structures in a file and restoring them later
 * transferring data structures over a network or pipe
 * passing objects to other applications using a standard format

C++ does not directly support serialization. Following the principle that you
do not pay for what you do not use, objects do not provide the runtime type
information that would be required by a generic serialization framework.
Therefore, the serialization framework needs some additional information to
convert objects into a byte stream and reconstruct them from it.

Anatomy of cxxtools::SerializationInfo
--------------------------------------

A `cxxtools::SerializationInfo` is a generic dynamic data structure that can
hold almost arbitrary data. It has the following properties:

- a type name
- a category
- a value
- an ordered list of name-value pairs

The type name is a string that should be set by the serialization operator.
Typically, the type name is not used by the deserialization operator, but it is
useful for some serialization formats.

The category is `Void`, `Value`, `Object`, or `Array`.

A `Value` is a scalar. Its value is stored in the value attribute of the
`cxxtools::SerializationInfo`.

An `Object` typically represents a struct or class. The value attribute of the
`cxxtools::SerializationInfo` is not used. Instead, the members of the object
are stored in an ordered list, with the member names as keys.

An `Array` is a list of unnamed values. As with an `Object`, the elements are
stored in a list of name-value pairs, but the names are empty.

The value field holds a scalar value. The value may be a byte string, a Unicode
string using `cxxtools::String`, a `char`, a `bool`, an integer, or a
floating-point value. The actual type is transparent. When the value is
requested, automatic conversion takes place as needed. During serialization or
deserialization, it is not possible to query which type of value is currently
stored in a `cxxtools::SerializationInfo`, and this should normally not be
necessary.

The ordered list of name-value pairs makes a `cxxtools::SerializationInfo`
hierarchical. The name is a `std::string`, while the value is another
`cxxtools::SerializationInfo`, which can in turn hold any supported data.

Members can be accessed by name or index, or through an iterator. The member
name is also stored in the value, making it easy to identify the member.

Making C++ data structures serializable
---------------------------------------

To make a C++ data structure serializable and deserializable, two operators
must be defined. One stores the data from the object in a
`cxxtools::SerializationInfo`, while the other reconstructs the object from
it.

The signatures of these operators are always the same. Let's look at an
example:

```cpp
struct Foo
{
    std::string stringMember;
    int intMember;
    std::vector<double> values;
};

inline void operator<<= (cxxtools::SerializationInfo& si, const Foo& foo)
{
    si.setTypeName("Foo");
    si.addMember("stringMember") <<= foo.stringMember;
    si.addMember("intMember") <<= foo.intMember;
    si.addMember("values") <<= foo.values;
}

inline void operator>>= (const cxxtools::SerializationInfo& si, Foo& foo)
{
    si.getMember("stringMember") >>= foo.stringMember;
    si.getMember("intMember") >>= foo.intMember;
    si.getMember("values") >>= foo.values;
}
```

The `operator<<=` is the serialization operator. It takes a non-const reference
to a `cxxtools::SerializationInfo` and a const reference to the object to be
serialized. The operator must store all data needed to reconstruct the object
in the `cxxtools::SerializationInfo`.

Since all built-in types, including the standard container classes, provide
such serialization operators, we can use them directly to serialize the
members.

The `operator>>=` is the deserialization operator and must reconstruct the
object from the supplied `cxxtools::SerializationInfo`. The
`cxxtools::SerializationInfo` is passed as a const reference because it must
not be modified. The `foo` object is passed as a non-const reference and is
filled from the first parameter.

Once these operators are defined, `Foo` itself can be serialized and
deserialized, as can more complex structures containing `Foo`, such as a
`std::vector<Foo>`.

The member names in `cxxtools::SerializationInfo` typically match the member
names of the C++ structure, but they do not have to. Furthermore, not every
member needs to be serialized if it is not needed during deserialization. The
only requirement is that the serialization and deserialization operators use
the same structure.

For example, the operators can also be defined as follows:

```cpp
inline void operator<<= (cxxtools::SerializationInfo& si, const Foo& foo)
{
    si.setTypeName("Foo");
    si.addMember("a") <<= foo.stringMember;
    si.addMember("b") <<= foo.intMember;
    si.addMember("c") <<= foo.values;
}

inline void operator>>= (const cxxtools::SerializationInfo& si, Foo& foo)
{
    si.getMember("b") >>= foo.intMember;
    si.getMember("c") >>= foo.values;
    si.getMember("a") >>= foo.stringMember;
}
```

You can even omit the string value during serialization when it is empty.
However, the deserialization operator must be prepared for the member to be
missing. The `getMember()` method throws an exception of type
`cxxtools::SerializationMemberNotFound` if the requested member is not found.

A variant that omits the string value when it is empty could therefore look
like this:

```cpp
inline void operator<<= (cxxtools::SerializationInfo& si, const Foo& foo)
{
    si.setTypeName("Foo");

    if (!foo.stringMember.empty())
        si.addMember("stringMember") <<= foo.stringMember;

    si.addMember("intMember") <<= foo.intMember;
    si.addMember("values") <<= foo.values;
}

inline void operator>>= (const cxxtools::SerializationInfo& si, Foo& foo)
{
    if (si.findMember("stringMember"))
        si.getMember("stringMember") >>= foo.stringMember;

    si.getMember("intMember") >>= foo.intMember;
    si.getMember("values") >>= foo.values;
}
```

This deserialization is not perfectly efficient because the string member is
searched for twice. `findMember()` returns a const pointer to the member if it
is found and a null pointer otherwise. A more efficient variant is therefore:

```cpp
const cxxtools::SerializationInfo* p = si.findMember("stringMember");
if (p)
    *p >>= foo.stringMember;
```

Since optional members are common, another variant of `getMember()` is
available. It takes a non-const reference to an object as its second argument
and fills that object if the member is found. The method returns a boolean
indicating whether the member was found.

The string member above can therefore be deserialized simply with:

```cpp
si.getMember("stringMember", foo.stringMember);
```

If the member is not found, `foo.stringMember` remains unchanged.

If additional handling is required when the member is not found, this can be
written as:

```cpp
if (!si.getMember("stringMember", foo.stringMember))
{
    // do something
}
```

### Verifying object types during deserialization

The type name stored in a `cxxtools::SerializationInfo` is optional. In many
cases, deserialization works correctly without checking it because the
deserialization operator simply reads the members it requires.

For simple applications this is often sufficient. However, in larger systems
different object types may have similar structures. In such cases it is
possible that an object is accidentally deserialized into the wrong type.

To detect this situation, the deserialization operator can verify the type
name and reject unexpected objects:

```cpp
inline void operator>>= (
        const cxxtools::SerializationInfo& si,
        Foo& msg)
{
    if (!si.typeName().empty() && si.typeName() != "Foo")
        throw cxxtools::SerializationError("is no Foo");

    si.getMember("text") >>= msg.text;
    si.getMember("number") >>= msg.number;
}
```

This approach is particularly useful when objects are received from external
sources such as configuration files, network protocols, RPC interfaces, or
publish-subscribe systems.

The check is intentionally performed only when a type name is present. This
allows the deserialization operator to continue working with formats or data
sources that do not provide type information.

Applications that rely on strict type safety often set a type name during
serialization:

```cpp
inline void operator<<= (
        cxxtools::SerializationInfo& si,
        const Foo& msg)
{
    si.setTypeName("Foo");
    si.addMember("text") <<= msg.text;
    si.addMember("number") <<= msg.number;
}
```

and verify that type name during deserialization.

Whether such checks are necessary depends on the application. For small and
well-controlled systems they are often omitted, while larger systems usually
benefit from the additional validation.

Common patterns for defining serialization and deserialization operators
------------------------------------------------------------------------

There are two aspects to consider when defining serialization and
deserialization operators.

First, as usual, we have to ensure that the operators are defined only once. A
common pattern is to define the operators inline in the header file in which
the object is defined.

Another possibility is to declare the operators in the header and define them
in the source file. In this case, it is perfectly valid to forward-declare

`cxxtools::SerializationInfo` instead of including
`<cxxtools/serializationinfo.h>`.

The forward declaration looks like this:

```cpp
namespace cxxtools
{
    class SerializationInfo;
}
```

The advantages of forward declarations include fewer dependencies and shorter
compile times. We will not discuss this in detail here, as the concept is
covered in most C++ books.

The second aspect is access to class members. The examples above use a simple
`struct`. If we use a `class` with private data members, however, we have to
consider how the members can be accessed by the serialization operators.

Let's change our `Foo` into a class with private members:

```cpp
class Foo
{
    std::string stringMember;
    int intMember;
    std::vector<double> values;

public:
    // make default-constructible
    Foo()
    { }
    Foo(const std::string& stringMember_, int intMember_,
        const std::vector<double>& values_)
      : stringMember(stringMember_),
        intMember(intMember_),
        values(values_)
    { }

    const std::string& getStringMember() const
    { return stringMember; }
    int getIntMember() const
    { return intMember; }

    unsigned getValuesCount() const
    { return values.size(); }

    double getValue(unsigned idx) const
    { return values[idx]; }
};
```

One advantage of this class is that we can ensure that all members are
initialized by the constructor. We also have better control over how the data
can be accessed and modified.

When we try to implement our serialization operators, however, we encounter a
problem. We cannot directly access the private members because the operators
are not members of the class.

One simple solution is to provide getter methods that return non-const
references to the members. However, this weakens the encapsulation of our
class. Nevertheless, let's look at this approach by adding the following
public member functions:

```cpp
std::string& getStringMember()
{ return stringMember; }

int& getIntMember()
{ return intMember; }

std::vector<double>& getValues()
{ return values; }
```

Then we can write the deserialization operator almost as before:

```cpp
inline void operator>>= (const cxxtools::SerializationInfo& si, Foo& foo)
{
    si.getMember("stringMember") >>= foo.getStringMember();
    si.getMember("intMember") >>= foo.getIntMember();
    si.getMember("values") >>= foo.getValues();
}
```

This works, but it is not really what we want.

Fortunately, there are better solutions. We can either make the operators
friends of the class or provide public `serialize()` and `deserialize()` member
functions that perform the actual work.

The friend variant is straightforward:

```cpp
class Foo
{
    friend void operator<<= (cxxtools::SerializationInfo& si, const Foo& foo);
    friend void operator>>= (const cxxtools::SerializationInfo& si, Foo& foo);

    // ...
};
```

Giving non-member functions access to private members may initially seem
undesirable. However, the serialization operators are effectively part of the
interface of the class. The public interface remains under our control, while
the operators can access exactly the data required for serialization.

Another option is to provide public member functions:

```cpp
    class Foo
    {
        ...
    public:
        ...
        void serialize(cxxtools::SerializationInfo& si) const;
        void deserialize(cxxtools::SerializationInfo& si);
        ...
```

The operators can then simply delegate to these functions:

```cpp
    inline void operator<<= (cxxtools::SerializationInfo& si, const Foo& foo)
    { foo.serialize(si); }

    inline void operator>>= (const cxxtools::SerializationInfo& si, Foo& foo)
    { foo.deserialize(si); }
```

The member functions have full access to the private members and perform the
actual serialization and deserialization work. In some cases, it is useful to
make these member functions virtual. Derived classes can then override them
when necessary instead of defining additional operators.

Limitations of cxxtools serialization
-------------------------------------

Serialization becomes significantly more complex when objects contain pointers
to other objects. These pointers may or may not refer to objects within the
data structure being serialized, which makes both serialization and
deserialization considerably more complicated.

Many serialization libraries support such structures, and cxxtools also
provided some support for them. However, this support was never perfect,
significantly slowed down deserialization, and was rarely needed. The cxxtools
developers therefore decided to remove it.

Furthermore, most serialization formats do not support pointers directly.
Therefore, the additional complexity and overhead are generally not worthwhile
for cxxtools.

Example
-------

What can we do with serialization? There are many possible use cases. Let's
look at one of the most obvious: storing data in a file and restoring it later.

Suppose we have a time series containing double values, for example values
from a sensor, and we want to make this series persistent for later processing.

To keep the example simple, we store each sample in a
`std::pair<time_t, double>`. In a real-world application, a dedicated type with
its own serialization operators might be preferable, but std::pair is
convenient for this example.

We use the XML serializer to store the values in XML format:

```cpp
    #include <cxxtools/xml/xmlserializer.h>

    void storeTimeSeries(
        const std::vector<std::pair<time_t, double> >& timeSeries)
    {
        std::ofstream out("theTimeSeries.xml");
        cxxtools::xml::XmlSerializer serializer(out);
        serializer.serialize(timeSeries, "timeSeries");
    }
```cpp

The helper wrapper `cxxtools::xml::Xml` provides a shorter and more convenient
form:

```cpp
    #include <cxxtools/xml/xml.h>

    void storeTimeSeries(
        const std::vector<std::pair<time_t, double> >& timeSeries)
    {
        std::ofstream out("theTimeSeries.xml");
        out << cxxtools::xml::Xml(timeSeries, "timeSeries");
    }
```cpp

With these few lines, we create an XML file containing the data.

Error handling for `std::ofstream` is omitted here. The XML serializer will
throw exceptions if any problem occurs.

By default, the serializers do not add unnecessary white space. To make the
output more readable, you can enable beautification. Using the wrapper:

```cpp
    out << cxxtools::xml::Xml(timeSeries, "timeSeries").beautify(true);
```

Restoring the data is also easy:

```cpp
    std::vector<std::pair<time_t, double> > restoreTimeSeries()
    {
        std::ifstream in("theTimeSeries.xml");
        cxxtools::xml::XmlDeserializer deserializer(in);

        std::vector<std::pair<time_t, double> > result;
        deserializer.deserialize(result);

        return result;
    }
```

Or using the wrapper:

```cpp
    std::vector<std::pair<time_t, double> > restoreTimeSeries()
    {
        std::ifstream in("theTimeSeries.xml");
        std::vector<std::pair<time_t, double> > result;
        in >> cxxtools::Xml(result);
        return result;
    }
```

If you prefer not to use XML, for example because of its verbosity, you can use
JSON or the cxxtools-specific binary format instead. The latter is very
compact.

Further reading on what can be done with cxxtools serialization
---------------------------------------------------------------

Other use cases for serialization are:
    * transferring objects over a network using RPC
    * exporting or importing CSV files
    * reading configuration files
    * make complex data structures visible for debugging purposes

cxxtools directly supports several RPC protocols for network communication. See
the [XML-RPC how-to](xmlrpc-howto.md), the [JSON-RPC how-to](jsonrpc-howto.md)
and the [binary RPC how-to](binaryrpc-howto.md) for details.

cxxtools also provides a serializer and deserializer for the CSV format.

Reading configuration files follows the same principle. You can define a
`struct` with your configuration variables and populate it using a suitable
deserializer. Note that you do not need to define the serialization operator
when you just read the file.

cxxtools logging can also be useful for debugging. Since the logging macros use
std::ostream, serialized objects can easily be included in log output:

```cpp
    log_debug(cxxtools::Json(myObject).beautify(true));
```
