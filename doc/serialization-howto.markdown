Serialization with cxxtools
===========================

Introduction
------------

Serialization is a technology, which converts data structures to a series of
bytes and back to the data structure.

There are many situations where serialization can be useful like:

 * store structures to a file and restore it back
 * transfer data structures over a network or pipe
 * pass objects to other applications using a standard format

C++ do not support serialization directly since the philosophy is, that you do
not pay for what you do not use. After compilation objects are just some
collection of bytes without any type information. It is not possible to ask for
the type or the properties of a type at runtime. Hence a generic serialization
framework need some help to convert generic objects into a byte stream and back.

Anatomy of cxxtools::SerializationInfo
--------------------------------------

A `cxxtools::SerializationInfo` is a generic dynamic data structure which can
hold almost arbitrary data. The attributes, which can be used to store data into
it are:

 * a type name
 * a category
 * a value
 * a ordered list of name value pairs

The type name is a string, which should be filled in the serialization operator.
Typically the type name is not used in the deserialization operator. But in some
serialization formats it is useful to have it.

The category is `Void`, `Value`, `Object` or `Array`.

A `Value` is just a scalar. The value of the scalar is held in the value
attribute of the `cxxtools::SerializationInfo`.

A `Object` is typically a representation of a `struct` or `class`. The value of
the `cxxtools::SerializationInfo` is not used but the attributes of the object
is stored in the ordered list with the member names as keys.

A `Array` is just a list of unnamed values. Here the elements are held again in
the list of name value pairs but with empty names.

The value field holds a scalar value. The value may be a byte string, unicode
string (using `cxxtools::String`), a char, bool, integer or floating point
value. The actual type is transparent. When the value is requested automatic
conversion takes place whenever needed. It is not possible to ask, which value
is currently held in the instance and actually never needed.

The ordered list of name value pairs makes a `cxxtools::SerializationInfo`
hierarchical. The name is just a `std::string`. The value is another
`cxxtools::SerializationInfo`, which can held also any type.

The members can be accessed by name, index or using a iterator. The actual name
of the member is also held in the value to make it easy to identify the object.

Making C++ data structures serializable
---------------------------------------

To make a C++ data structure serializable and deserializable 2 operators must be
defined. One stores the data from the data structure to a object of type
`cxxtools::SerializationInfo` and the other reconstructs the object back.

The signature for the operator is always the same. Lets look at a example:

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

The `operator<<=` is the serialization operator and takes a non const reference
to a `cxxtools::SerializationInfo` and a const reference to the object to be
serialized. The operator must store all attributes, needed do restore the object
into the `cxxtools::SerializationInfo`. Since all built in types including the
standard container classes have such a serialization operator, we can use it to
serialize the members.

The `operator>>=` is the deserialization operator and must reconstruct the
object from the passed `cxxtools::SerializationInfo`. The
`cxxtools::SerializationInfo` is a const reference here since it must not be
changed. The object is passed as a non const reference and must be filled from
the first parameter.

When those operators are defined, not only `Foo` can be serialized and
deserialized but also complex structures, which has a `Foo` member like e.g. a
`std::vector<Foo>`.

The member names in the `cxxtools::SerializationInfo` typically match the member
names of the C++ structure but they do not need to. Also not every member need
to be serialized when not needed for deserialization. It is just necessary, that
the serialization operator and deserialization operator use the same structure.
It is perfectly OK to define the operators like that:

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

You can even omit e.g. the string value from serialization when it is empty.
But you need to be prepared for it when deserializing the object. The
`getMember` method here throws a exception of type
`cxxtools::SerialzationMemberNotFound` when the member is not found. So a
variant, which omits the string value when empty is here:

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

The deserialization is not perfectly efficient since the string member is
searched twice. `findMember` returns a const pointer to the member when found
and a null pointer otherwise. So more efficient is to use:

    const cxxtools::SerializationInfo *p = si.findMember("stringMember")
    if (p)
        *p >>= foo.stringMember;

But since it is so common to have optional parameters, another variant of
`getMember` is available. It expects a non const reference to a object as a
second argument, which is filled and returns a boolean value, which indicates if
it has found the value. So the above string member can be deserialized using:

    si.getMember("stringMember", foo.stringMember);

In case the member is not found, the `foo.stringMember` is not touched at all.
If you need to do something, when the member is not found, it is easy also:

    if (!si.getMember("stringMember", foo.stringMember))
    {
        // do something
    }

Common patterns for defining serialization and deserialization operators
------------------------------------------------------------------------

There are 2 aspects, which have to be taken care of when defining operators.

As always we have to make sure, that the operators are defined only once. A
common pattern is to define the operators inline in the header file, where the
object is defined. Another possibility is to just declare the operators in the
header and define them in the source file. In the latter case it is perfectly OK
to forward declare the `cxxtools::SerializationInfo` object instead if including
`cxxtools/serialzationinfo.h`. Forward declaration can be done like that:

    namespace cxxtools
    {
        class SerializationInfo;
    }

The advantages of forward declaration are less dependencies and faster compile
times. We do not go into detail about that here since it is described in any
good C++ book.

The other aspect is accessibility of members. Above we saw a simple `struct`.
But if we have a `class` with private data members, we have to think, how to
make the members accessible in the operators.

Lets change our `Foo` to a `class` with private members:

    class Foo
    {
        std::string stringMember;
        int intMember;
        std::vector<double> values;

    public:
        // make default constructable
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

The nice thing about that class is, that we make sure, that all members are
initialized in the constructor. And we have better control, what can be done
with the data.

When we try to write our operators we run into problems. We can't just access
the private member data as above since the operators are not members of our
class.

One simple solution is to create getter methods, which return non const
references to the members. But this destroys the strictness of our class. Anyway
lets look at the example. We define additional public members:

        std::string& getStringMember()
        { return stringMember; }

        int& getIntMember()
        { return intMember; }

        std::vector<double>& getValues()
        { return values; }

Then we can write our deserialzation operator almost like before:

    inline void operator>>= (const cxxtools::SerializationInfo& si, Foo& foo)
    {
        si.getMember("stringMember") >>= foo.getStringMember();
        si.getMember("intMember") >>= foo.getIntMember();
        si.getMember("values") >>= foo.getValues();
    }

Works, but not really what we want to have.

Luckily there are better solutions. Either make the operators friend or make
just public serialize and deserialize member functions, which do the actual
work.

The friend variant is straight forward:

    class Foo
    {
        friend void operator<<= (cxxtools::SerializationInfo& si, const Foo& foo);
        friend void operator>>= (const cxxtools::SerializationInfo& si, Foo& foo);
        ...

You may be scared about giving access to private members to a non member
function. But note, that the operators are somewhat a immanent part of the
interface. We still have good control over the public interface to our class.
The operators become something like members of our class.

Another variant is to write public member functions like that:

    class Foo
    {
        ...
    public:
        ...
        void serialize(cxxtools::SerializationInfo& si) const;
        void deserialize(cxxtools::SerializationInfo& si);
        ...

Now we can write our operator like that:

    inline void operator<<= (cxxtools::SerializationInfo& si, const Foo& foo)
    { foo.serialize(si); }

    inline void operator>>= (const cxxtools::SerializationInfo& si, Foo& foo)
    { foo.deserialize(si); }

The member functions have full access to the private members and do the actual
work. Sometimes it is useful to make the members virtual. Then you do not need
to define the operators for derived classes any more but just override those
members when needed.

Limitations of cxxtools serialization
-------------------------------------

Serialization becomes very complex when the object have pointers to other
objects. The pointers may or may not point to objects in the structure, which
need to be serialized. This makes serialization and deserialization much more
complex.

Many serialization libraries support such things and cxxtools had also some
support for it but since it was never perfect, slows down deserialization
significantly and is rarely needed, the developers of cxxtools have
decided to drop the support for it.

Also most serialization formats do not support pointers. Hence it is not that
useful and not really worth the overhead for cxxtools.

Example
-------

So what can be done with serialization? There are many use cases, where
serialization is useful. Lets look at the most obvious one: storing data in a
file and restoring it.

Lets assume, we have a time series with double values, for example a test series
from a sensor and want to make the series persistent for later processing. To
make it really simple we store a sample in a `std::pair<time_t, double>`. In a
real world process I would prefer to use something better than `std::pair` and
write the operators myself but it is just so convenient for now.

We choose to use the xml serializer to store the values in a xml format. So here
is the code fragment:

    #include <cxxtools/xml/xmlserializer.h>

    void storeTimeSeries(
        const std::vector<std::pair<time_t, double> >& timeSeries)
    {
        std::ofstream out("theTimeSeries.xml");
        cxxtools::xml::XmlSerializer serializer(out);
        serializer.serialize(timeSeries, "timeSeries");
    }

With cxxtools version 2.3 it will become even shorter. There we have a little
wrapper `cxxtools::Xml`, which makes it even easier:

    #include <cxxtools/xml/xml.h>

    void storeTimeSeries(
        const std::vector<std::pair<time_t, double> >& timeSeries)
    {
        std::ofstream out("theTimeSeries.xml");
        out << cxxtools::Xml(timeSeries, "timeSeries");
    }

With those few lines we create a xml file with the data.

We omit error handling of `std::ofstream` here. The xml serializer will throw
exceptions when any problem occur.

Note that the serializers do not include unnecessary white space. If you want to
make the file readable, you should enable beautification. In cxxtools 2.3 add a
method call to the object returned by the wrapper:

    out << cxxtools::Xml(timeSeries, "timeSeries").beautify(true);

Restoring the data is also easy:

    std::vector<std::pair<time_t, double> >
        restoreTimeSeries();
    {
        std::ifstream in("theTimeSeries.xml");
        cxxtools::xml::XmlDeserializer deserializer(in);

        std::vector<std::pair<time_t, double> > result;
        deserializer.deserialize(result);

        return result;
    }

Again with cxxtools version 2.3 it become easier:

    std::vector<std::pair<time_t, double> >
        restoreTimeSeries();
    {
        std::ifstream in("theTimeSeries.xml");
        std::vector<std::pair<time_t, double> > result;
        in >> cxxtools::Xml(result);
        return result;
    }

If you prefer not to use xml since it is so verbose or for other reasons, it is
as easy to use json or a cxxtools specific binary format, which is very compact.

Further reading or what can be done with cxxtools serialization
---------------------------------------------------------------

Other use cases for serialization is:

    * transferring objects over a network using rpc
    * export or import csv files
    * reading configuration files
    * make complex data structures visible for debugging purposes

For network communication there are several rpc protocols supported by cxxtools
directly. Look at the [xmlrpc howto](xmlrpc-howto.html), the [jsonrpc
howto](jsonrpc-howto.html) and the [binaryrpc howto](binaryrpc-howto.html).

Cxxtools also offers a serializer and deseriailzer for the csv format.

Reading configuration files is obvious now. You can define a `struct` with your
configuration variables and fill it using a suitable deseriailzer. Note that you
do not need to define the serialization operator when you just read the file.

The cxxtools logging is fine for debugging and since the logging macros use
`std::ostream` it becomes easy (with cxxtools >= 2.3):

    log_debug(cxxtools::Json(myObject).beautify(true));
