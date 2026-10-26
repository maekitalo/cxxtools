Cxxtools
========

cxxtools is a toolbox with reusable C++ components.

See `INSTALL` for installation instructions.


Installation from Git
=====================

The Git repository does not contain a `configure` script, but it contains
`configure.ac`, which is used to generate it. You need `autoconf` and
`automake` to generate the configure script.

Run

```sh
autoreconf -i
```

to generate a configure script.

Documentation
=============

Various how-tos can be found here:

 * [Eventloop](doc/eventloop-howto.md)
 * [Asynchronous I/O](doc/asynchronous-howto.md)
 * [Argument parsing](doc/argument-parsing-howto.md)
 * [Network connections](doc/connection.md)
 * [CSV](doc/csv-howto.md)
 * [JSON quick](doc/jsonquick-howto.md)
 * [JSON](doc/json-howto.md)
 * [Logging](doc/logging-howto.md)
 * [Properties](doc/propertiesquick-howto.md)
 * [RPC](doc/rpc-howto.md)
 * [Serialization](doc/serialization-howto.md)
 * [Time zones](doc/timezone-howto.md)

There are also some useful demo applications that show how to use various
features of cxxtools.

Tools
-----

cxxtools provides two command-line tools.

### siconvert

The `siconvert` tool converts between various serialization formats.

A short introduction is printed when the tool is called without arguments. Data
can be read from standard input, and the result is written to standard output.
The tool can also be used to format files for better readability.
 
The input format is typically specified using a lowercase letter and the output
format using an uppercase letter. The `-d` option enables formatted output
(think of "display").

#### Examples

Convert CSV data to JSON:

```sh
    cat "foo.csv" | siconvert -cJd
    # or
    siconvert -cJd foo.csv
```

Convert cxxtools binary data to XML:

```sh
    cat "foo.bin" | siconvert -bXd
    # or
    siconvert -bXd foo.bin
```

Format an existing JSON file:

```sh
    cat "foo.json" | siconvert -jJd
    # or
    siconvert -jJd foo.json
```

### cxxtz

The `cxxtz` command-line tool handles time zones. A short introduction is
printed when the tool is called without arguments.

#### Examples

List all time zones known to the system:

```sh
    cxxtz -a
```

Print the current time in the `Pacific/Nauru` time zone:

```sh
    cxxtz -c -z Pacific/Nauru
```
